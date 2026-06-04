#include "os_qemu.h"
#include "../../cpu/cpu.h"
#include "../pcb.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

/* Clasifica y reporta fallos. Afecta: UART0. Deps: os_trace_fault, cpu_read_dfsr, cpu_read_ifsr. */
void os_fault_handler(uint32_t *frame) {
    (void)frame;
    uint32_t dfsr = cpu_read_dfsr();
    os_trace_fault(g_pcbs[g_current_proc].pid, (dfsr != 0) ? "DATA_ABORT" : "PREFETCH_ABORT");
    for (;;); // Hang for now as per Section 3.6
}
