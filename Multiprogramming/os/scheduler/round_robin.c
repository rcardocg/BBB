#include "scheduler.h"
#include <stdint.h>

#define P1_ENTRY      0x82100000u
#define P2_ENTRY      0x82200000u
#define P1_STACK_TOP  0x82112000u
#define P2_STACK_TOP  0x82212000u

#define P1_PID 1u
#define P2_PID 2u
#define NUM_USER_PROCS 2u
#define INITIAL_PROC_SPSR 0x13u

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
    g_current_proc = (g_current_proc + 1u) % NUM_USER_PROCS;
}
