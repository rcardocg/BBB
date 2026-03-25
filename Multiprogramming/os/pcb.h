#ifndef PCB_H
#define PCB_H

#include <stdint.h>

typedef enum {
    PROC_READY = 0,
    PROC_RUNNING = 1
} proc_state_t;

typedef struct {
    uint32_t pid;
    uint32_t sp; /* saved SVC stack pointer for context frame */
    proc_state_t state;
} pcb_t;

void pcb_init(pcb_t *pcb, uint32_t pid, uint32_t sp, proc_state_t state);
void pcb_set_state(pcb_t *pcb, proc_state_t state);
void pcb_set_sp(pcb_t *pcb, uint32_t sp);

#endif
