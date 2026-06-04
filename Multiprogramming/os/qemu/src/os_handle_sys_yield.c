#include "os_qemu.h"
#include "../pcb.h"
#include "../../cpu/cpu.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

static uint32_t next_runnable_after(uint32_t current) {
    uint32_t i;

    for (i = 1u; i <= 2u; i = i + 1u) {
        uint32_t candidate = (current + i) % 2u;
        if (g_pcbs[candidate].state != PROC_TERMINATED) {
            return candidate;
        }
    }

    return current;
}

/* Manejador de SYS_YIELD con cambio de contexto (Inciso 3.7). Deps: cpu_get_user_regs, pcb_save/restore. */
void os_handle_sys_yield(uint32_t *frame) {
    uint32_t usr_sp;
    uint32_t usr_lr;
    uint32_t next_proc;

    cpu_get_user_regs(&usr_sp, &usr_lr);
    pcb_save_from_irq_frame(&g_pcbs[g_current_proc], frame, usr_sp, usr_lr);
    if (g_pcbs[g_current_proc].state != PROC_TERMINATED) {
        pcb_set_state(&g_pcbs[g_current_proc], PROC_READY);
    }

    next_proc = next_runnable_after(g_current_proc);
    g_current_proc = next_proc;
    pcb_restore_to_irq_frame(&g_pcbs[g_current_proc], frame, &usr_sp, &usr_lr);
    cpu_set_user_regs(usr_sp, usr_lr);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_RUNNING);
    frame[0] = 0u;
}
