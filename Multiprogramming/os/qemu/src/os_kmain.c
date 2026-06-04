#include "os_qemu.h"
#include "../timer.h"
#include "../intc.h"
#include "../../cpu/cpu.h"
#include "../pcb.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

/* Punto de entrada. Afecta: HW, CPU. Deps: os_pcb_system_init, cpu_switch_to_user_mode. */
void os_kmain(void) {
    os_wdt_disable();
    os_uart_puts("\n[OS] boot (Atomic RR)\n");
    os_pcb_system_init();
    timer_init();
    intc_init();
    
    /* IRQ habilitadas al entrar a USR (SPSR=0x10). */
    os_trace_mode_switch(g_pcbs[g_current_proc].pid, "KERNEL_TO_USER", "initial_launch", 0);
    
    cpu_switch_to_user_mode(g_pcbs[g_current_proc].pc, 
                            g_pcbs[g_current_proc].sp, 
                            g_pcbs[g_current_proc].lr);
}
