#include "os_qemu.h"
#include "../pcb.h"
#include "../../cpu/cpu.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

/* Manejador de Timer con Traces 3.5. Afecta: CPU, UART. Deps: os_trace_mode_switch. */
void os_timer_irq_handler(uint32_t *irq_frame) {
    uint32_t usr_sp, usr_lr;
    
    /* Trace: USER_TO_KERNEL (Interrupt Path) */
    os_trace_mode_switch(g_pcbs[g_current_proc].pid, "USER_TO_KERNEL", "timer_irq", 0);
    
    /* ACK timer + EOI so interrupts continue */
    (*(volatile uint32_t *)(0x101E2000u + 0x0C)) = 1u;
    (*(volatile uint32_t *)(0x10140000u + 0x30)) = 0u;

    cpu_get_user_regs(&usr_sp, &usr_lr);
    pcb_save_from_irq_frame(&g_pcbs[g_current_proc], irq_frame, usr_sp, usr_lr);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_READY);

    g_current_proc = (g_current_proc + 1) % 2;

    pcb_restore_to_irq_frame(&g_pcbs[g_current_proc], irq_frame, &usr_sp, &usr_lr);
    cpu_set_user_regs(usr_sp, usr_lr);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_RUNNING);
    
    /* Trace: KERNEL_TO_USER (Dispatch) */
    os_trace_mode_switch(g_pcbs[g_current_proc].pid, "KERNEL_TO_USER", "dispatch", 0);
}
