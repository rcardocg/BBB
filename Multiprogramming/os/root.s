.syntax unified
.cpu cortex-a8
.arch armv7-a

.global _start
.global irq_handler

.extern os_kmain
.extern os_timer_irq_handler
.extern os_syscall_dispatcher
.extern os_fault_handler
.extern __bss_start
.extern __bss_end
.extern __vectors_start
.extern __irq_stack_top
.extern __svc_stack_top
.extern __abt_stack_top
.extern __und_stack_top

.section .vectors, "ax"
.align 5
__vectors_start:
    b _start          @ Reset
    b undefined_handler
    b svc_handler     @ SVC
    b prefetch_handler @ Prefetch abort
    b abort_handler    @ Data abort
    b .               @ Reserved
    b irq_handler     @ IRQ
    b .               @ FIQ

.section .text.boot, "ax"
.align 4
_start:
    cpsid if

    @ Point VBAR to our vector table
    ldr r0, =__vectors_start
    mcr p15, 0, r0, c12, c0, 0
    dsb
    isb

    @ IRQ stack
    mrs r0, cpsr
    bic r1, r0, #0x1F
    orr r1, r1, #0x12
    msr cpsr_c, r1
    ldr sp, =__irq_stack_top

    @ Abort stack
    bic r1, r0, #0x1F
    orr r1, r1, #0x17
    msr cpsr_c, r1
    ldr sp, =__abt_stack_top

    @ Undefined stack
    bic r1, r0, #0x1F
    orr r1, r1, #0x1B
    msr cpsr_c, r1
    ldr sp, =__und_stack_top

    @ SVC stack
    bic r1, r0, #0x1F
    orr r1, r1, #0x13
    msr cpsr_c, r1
    ldr sp, =__svc_stack_top

    @ Clear .bss
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
1:
    cmp r0, r1
    bhs 2f
    str r2, [r0], #4
    b 1b
2:
    dsb
    isb

    bl os_kmain

hang:
    b hang

undefined_handler:
    sub lr, lr, #4
    b common_abort

svc_handler:
    @ Save r0-r12/lr_svc + spsr_svc in a frame
    sub sp, sp, #64
    stmia sp, {r0-r12, lr}
    mrs r0, spsr
    str r0, [sp, #56]
    @ Adjust LR for IRQ-frame convention used by pcb_save/restore
    ldr r0, [sp, #52]
    add r0, r0, #4
    str r0, [sp, #52]
    mov r0, sp
    bl os_syscall_dispatcher
    @ Restore original LR for correct SVC return
    ldr r0, [sp, #52]
    sub r0, r0, #4
    str r0, [sp, #52]
    ldr r0, [sp, #56]
    msr spsr_cxsf, r0
    ldmia sp, {r0-r12, lr}
    add sp, sp, #64
    movs pc, lr

prefetch_handler:
    sub lr, lr, #4
    b common_abort

abort_handler:
    sub lr, lr, #8
    b common_abort

common_abort:
    sub sp, sp, #64
    stmia sp, {r0-r12, lr}
    mrs r0, spsr
    str r0, [sp, #56]
    mov r0, sp
    bl os_fault_handler
    b .

irq_handler:
    @ Save r0-r12/lr_irq + spsr_irq in an IRQ frame and call C scheduler.
    sub sp, sp, #64
    stmia sp, {r0-r12, lr}
    mrs r0, spsr
    str r0, [sp, #56]
    mov r0, sp
    bl os_timer_irq_handler
    ldr r0, [sp, #56]
    msr spsr_cxsf, r0
    ldmia sp, {r0-r12, lr}
    add sp, sp, #64
    subs pc, lr, #4
