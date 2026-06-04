#include "scheduler.h"
#include <stdint.h>

#define P1_ENTRY      0x82100000u
#define P2_ENTRY      0x82200000u
#define P1_STACK_TOP  0x82112000u
#define P2_STACK_TOP  0x82212000u

#define P1_PID 1u
#define P2_PID 2u
#define NUM_USER_PROCS 2u
#define INITIAL_PROC_SPSR 0x10u //0x13u es modo supervisor, 0x10 es privilegiado

static pcb_t g_pcbs[NUM_USER_PROCS];
static uint32_t g_current_proc;

void scheduler_init(void) {
    pcb_init(&g_pcbs[0], P1_PID, P1_ENTRY, P1_STACK_TOP, INITIAL_PROC_SPSR, PROC_RUNNING);
    pcb_init(&g_pcbs[1], P2_PID, P2_ENTRY, P2_STACK_TOP, INITIAL_PROC_SPSR, PROC_READY);
    g_current_proc = 0u;
    __asm__ volatile ("dsb\n\tisb" ::: "memory");
}

pcb_t* scheduler_get_current_pcb(void) {
    return &g_pcbs[g_current_proc];
}

void scheduler_next(void) {
    uint32_t i;

    for (i = 1u; i <= NUM_USER_PROCS; i = i + 1u) {
        uint32_t candidate = (g_current_proc + i) % NUM_USER_PROCS;
        if (g_pcbs[candidate].state != PROC_TERMINATED) {
            g_current_proc = candidate;
            return;
        }
    }
}

void scheduler_terminate_current(void) {
    g_pcbs[g_current_proc].state = PROC_TERMINATED;
}

int scheduler_has_runnable(void) {
    uint32_t i;

    for (i = 0u; i < NUM_USER_PROCS; i = i + 1u) {
        if (g_pcbs[i].state != PROC_TERMINATED) {
            return 1;
        }
    }

    return 0;
}
