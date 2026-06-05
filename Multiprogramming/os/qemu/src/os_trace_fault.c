#include "os_qemu.h"
/* Trace de fallos del Inciso 3.6. Afecta: UART0. Deps: os_trace_mode_switch. */
void os_trace_fault(uint32_t pid, const char *type) {
    char extra[32];
    uint32_t i;
    for (i = 0u; i < 31u && type[i] != '\0'; i = i + 1u) {
        extra[i] = type[i];
    }
    extra[i] = '\0';
    os_trace_mode_switch(pid, "USER_TO_KERNEL", "fault", extra);
}
