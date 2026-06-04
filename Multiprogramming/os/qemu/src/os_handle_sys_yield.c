#include "os_qemu.h"
#include "../pcb.h"
#include "../../cpu/cpu.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

/* Manejador de SYS_YIELD con cambio de contexto (Inciso 3.7). Deps: cpu_get_user_regs, pcb_save/restore. */
void os_handle_sys_yield(uint32_t *frame) {
    uint32_t usr_sp;
    uint32_t usr_lr;

    cpu_get_user_regs(&usr_sp, &usr_lr);
    pcb_save_from_irq_frame(&g_pcbs[g_current_proc], frame, usr_sp, usr_lr);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_READY);
    g_current_proc = (g_current_proc + 1u) % 2u;
    pcb_restore_to_irq_frame(&g_pcbs[g_current_proc], frame, &usr_sp, &usr_lr);
    cpu_set_user_regs(usr_sp, usr_lr);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_RUNNING);
    frame[0] = 0u;
}
