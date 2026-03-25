#ifndef PCB_H
#define PCB_H

#include <stdint.h>

#define PCB_GPR_COUNT 13u

/* IRQ frame built in root.s: r0-r12, lr_irq, spsr_irq */
typedef enum {
    IRQ_FRAME_R0 = 0,
    IRQ_FRAME_R12 = 12,
    IRQ_FRAME_LR_IRQ = 13,
    IRQ_FRAME_SPSR = 14,
    IRQ_FRAME_WORDS = 15
} irq_frame_index_t;

typedef enum {
    PROC_READY = 0,
    PROC_RUNNING = 1
} proc_state_t;

typedef struct {
    uint32_t pid;
    uint32_t sp; /* saved SVC stack pointer */
    uint32_t pc; /* resume address */
    uint32_t lr; /* saved SVC link register */
    uint32_t spsr;
    uint32_t r[PCB_GPR_COUNT]; /* r0-r12 */
    proc_state_t state;
} pcb_t;

void pcb_init(pcb_t *pcb,
              uint32_t pid,
              uint32_t entry_pc,
              uint32_t stack_top,
              uint32_t initial_spsr,
              proc_state_t state);
void pcb_set_state(pcb_t *pcb, proc_state_t state);
void pcb_save_from_irq_frame(pcb_t *pcb,
                             const uint32_t *irq_frame,
                             uint32_t svc_sp,
                             uint32_t svc_lr);
void pcb_restore_to_irq_frame(const pcb_t *pcb,
                              uint32_t *irq_frame,
                              uint32_t *svc_sp,
                              uint32_t *svc_lr);

#endif
