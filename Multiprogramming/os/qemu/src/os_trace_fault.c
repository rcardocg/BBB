#include "os_qemu.h"
/* Trace de fallos del Inciso 3.6. Afecta: UART0. Deps: os_trace_mode_switch. */
void os_trace_fault(uint32_t pid, const char *type) {
    char extra[32];
    uint32_t i;
    extra[0] = 't';
    extra[1] = 'y';
    extra[2] = 'p';
    extra[3] = 'e';
    extra[4] = '=';
    for (i = 0u; i < 26u && type[i] != '\0'; i = i + 1u) {
        extra[5u + i] = type[i];
    }
    extra[5u + i] = '\0';
    os_trace_mode_switch(pid, "USER_TO_KERNEL", "fault", extra);
}
