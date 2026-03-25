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

#endif
