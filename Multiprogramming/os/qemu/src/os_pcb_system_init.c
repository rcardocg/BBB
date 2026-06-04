#include "os_qemu.h"
#include "../pcb.h"
#define P1_ENTRY 0x00020000u
#define P2_ENTRY 0x00030000u
#define P1_STACK_TOP 0x00030000u
#define P2_STACK_TOP 0x00040000u
#define INITIAL_PROC_SPSR 0x10u
pcb_t g_pcbs[2];
uint32_t g_current_proc;
/* Inicializa la tabla de procesos. Afecta: g_pcbs, g_current_proc. Deps: pcb_init. */
void os_pcb_system_init(void) {
    pcb_init(&g_pcbs[0], 1, P1_ENTRY, P1_STACK_TOP, INITIAL_PROC_SPSR, PROC_RUNNING);
    pcb_init(&g_pcbs[1], 2, P2_ENTRY, P2_STACK_TOP, INITIAL_PROC_SPSR, PROC_READY);
    g_current_proc = 0;
    __asm__ volatile("dsb\n\tisb" ::: "memory");
}
