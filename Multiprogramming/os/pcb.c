#include "pcb.h"

void pcb_init(pcb_t *pcb,
              uint32_t pid,
              uint32_t entry_pc,
              uint32_t stack_top,
              uint32_t initial_spsr,
              proc_state_t state) {
    uint32_t i;

    if (pcb == 0) {
        return;
    }

    pcb->pid = pid;
    pcb->sp = stack_top;
    pcb->pc = entry_pc;
    pcb->lr = entry_pc;
    pcb->spsr = initial_spsr;
    for (i = 0; i < PCB_GPR_COUNT; i++) {
        pcb->r[i] = 0u;
    }
    pcb->state = state;
}

void pcb_set_state(pcb_t *pcb, proc_state_t state) {
    if (pcb == 0) {
        return;
    }

    pcb->state = state;
}

void pcb_save_from_irq_frame(pcb_t *pcb,
                             const uint32_t *irq_frame,
                             uint32_t svc_sp,
                             uint32_t svc_lr) {
    uint32_t i;

    if (pcb == 0 || irq_frame == 0) {
        return;
    }

    for (i = 0; i < PCB_GPR_COUNT; i++) {
        pcb->r[i] = irq_frame[i];
    }
    pcb->sp = svc_sp;
    pcb->lr = svc_lr;
    pcb->pc = irq_frame[IRQ_FRAME_LR_IRQ] - 4u;
    pcb->spsr = irq_frame[IRQ_FRAME_SPSR];
}

void pcb_restore_to_irq_frame(const pcb_t *pcb,
                              uint32_t *irq_frame,
                              uint32_t *svc_sp,
                              uint32_t *svc_lr) {
    uint32_t i;

    if (pcb == 0 || irq_frame == 0 || svc_sp == 0 || svc_lr == 0) {
        return;
    }

    for (i = 0; i < PCB_GPR_COUNT; i++) {
        irq_frame[i] = pcb->r[i];
    }
    irq_frame[IRQ_FRAME_LR_IRQ] = pcb->pc + 4u;
    irq_frame[IRQ_FRAME_SPSR] = pcb->spsr;
    *svc_sp = pcb->sp;
    *svc_lr = pcb->lr;
}
