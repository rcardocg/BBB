#include "pcb.h"

// Inicializa el PCB
void pcb_init(pcb_t *pcb, uint32_t pid, uint32_t sp, proc_state_t state) {
    if (pcb == 0) {
        return;
    }

    pcb->pid = pid;
    pcb->sp = sp;
    pcb->state = state;
}

// Cambia el estado
void pcb_set_state(pcb_t *pcb, proc_state_t state) {
    if (pcb == 0) {
        return;
    }

    pcb->state = state;
}

// Actualiza el stack pointer
void pcb_set_sp(pcb_t *pcb, uint32_t sp) {
    if (pcb == 0) {
        return;
    }

    pcb->sp = sp;
}
