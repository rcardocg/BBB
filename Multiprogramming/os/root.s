.syntax unified
.cpu cortex-a8
.arch armv7-a

.global _start
.global irq_handler

.extern kmain
.extern timer_irq_handler
.extern __bss_start
.extern __bss_end
.extern __vectors_start
.extern __irq_stack_top
.extern __svc_stack_top

.section .vectors, "ax"
.align 5
__vectors_start:
    b _start          @ Reset
    b .               @ Undefined
    b .               @ SVC
    b .               @ Prefetch abort
    b .               @ Data abort
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

    bl kmain

hang:
    b hang

irq_handler:
    @ Simple IRQ handler: save GPRs, call C, restore, return.
    sub sp, sp, #56
    stmia sp, {r0-r12, lr}
    bl timer_irq_handler
    ldmia sp, {r0-r12, lr}
    add sp, sp, #56
    subs pc, lr, #4
