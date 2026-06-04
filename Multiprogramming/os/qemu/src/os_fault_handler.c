#include "os_qemu.h"
#include "../../cpu/cpu.h"
#include "../pcb.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

/* Clasifica fallos, termina tarea y reanuda otra si existe (Inciso 3.6). Deps: os_trace_fault, cpu_set_user_regs. */
void os_fault_handler(uint32_t *frame) {
    uint32_t dfsr = cpu_read_dfsr();
    const char *type = (dfsr != 0u) ? "DATA_ABORT" : "PREFETCH_ABORT";
    pcb_t *next;
    uint32_t i;

    (void)frame;
    os_trace_fault(g_pcbs[g_current_proc].pid, type);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_TERMINATED);

    for (i = 0u; i < 2u; i = i + 1u) {
        uint32_t candidate = (g_current_proc + 1u + i) % 2u;
        if (g_pcbs[candidate].state == PROC_READY) {
            g_current_proc = candidate;
            next = &g_pcbs[g_current_proc];
            pcb_set_state(next, PROC_RUNNING);
            os_trace_mode_switch(next->pid, "KERNEL_TO_USER", "fault_recovery", 0);
            cpu_set_user_regs(next->sp, next->lr);
            __asm__ volatile (
                "msr spsr_cxsf, %0\n\t"
                "mov lr, %1\n\t"
                "movs pc, lr\n\t"
                :: "r"(next->spsr), "r"(next->pc)
                : "memory");
        }
    }

    for (;;) {
    }
}
