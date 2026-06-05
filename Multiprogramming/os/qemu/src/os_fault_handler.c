#include "os_qemu.h"
#include "../../cpu/cpu.h"
#include "../pcb.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

static const char* decode_fsr(uint32_t fsr) {
    uint32_t status = (fsr & 0xFu) | ((fsr & (1u << 10)) >> 6);
    if (status == 0x1u) return "type=alignment";
    if (status == 0x5u) return "type=translation_section";
    if (status == 0x7u) return "type=translation_page";
    if (status == 0xDu) return "type=permission_section";
    if (status == 0xFu) return "type=permission_page";
    return "type=unknown";
}

/* Clasifica fallos, termina tarea y reanuda otra si existe */
void os_fault_handler(uint32_t *frame, uint32_t fault_type) {
    pcb_t *next;
    uint32_t i;
    const char *type = "type=undefined";

    (void)frame;
    
    if (fault_type == 1u) {
        type = decode_fsr(cpu_read_ifsr());
    } else if (fault_type == 2u) {
        type = decode_fsr(cpu_read_dfsr());
    }

    os_trace_fault(g_pcbs[g_current_proc].pid, type);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_TERMINATED);

    for (i = 0u; i < 2u; i = i + 1u) {
        uint32_t candidate = (g_current_proc + 1u + i) % 2u;
        if (g_pcbs[candidate].state == PROC_READY) {
            uint32_t usr_sp;
            uint32_t usr_lr;
            g_current_proc = candidate;
            next = &g_pcbs[g_current_proc];
            pcb_set_state(next, PROC_RUNNING);
            os_trace_mode_switch(next->pid, "KERNEL_TO_USER", "fault_recovery", 0);
            
            /* Preparar el frame en la pila actual para que el handler de ensamblador 
               (root.s) restaure r0-r12, lr y spsr correctamente usando ldmia */
            pcb_restore_to_irq_frame(next, frame, &usr_sp, &usr_lr);
            cpu_set_user_regs(usr_sp, usr_lr);
            
            return; /* Retornar a root.s para hacer el context switch seguro */
        }
    }

    for (;;) {
    }
}
