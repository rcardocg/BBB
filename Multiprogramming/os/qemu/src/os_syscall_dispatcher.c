#include "os_qemu.h"
#include "../pcb.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

/* Despacha syscalls con traces de modo (Inciso 3.7). Afecta: UART0, CPU, PCB. Deps: os_trace_mode_switch, os_handle_sys_yield. */
void os_syscall_dispatcher(uint32_t *frame) {
    uint32_t syscall_num = frame[7];
    uint32_t spsr = frame[14];
    char extra[16];

    if ((spsr & 0x1Fu) != 0x10u) {
        os_uart_puts("\n[KERNEL] Syscall rejected: not from USR mode\n");
        frame[0] = 0xFFFFFFFFu;
        return;
    }

    extra[0] = 'i'; extra[1] = 'd'; extra[2] = '=';
    extra[3] = (char)('0' + (char)syscall_num);
    extra[4] = '\0';
    os_trace_mode_switch(g_pcbs[g_current_proc].pid, "USER_TO_KERNEL", "syscall", extra);

    if (syscall_num == 1u) {
        os_uart_putc((char)frame[0]);
        frame[0] = 0u;
    } else if (syscall_num == 2u) {
        os_handle_sys_yield(frame);
    } else {
        os_uart_puts("\n[KERNEL] Unknown syscall\n");
        return;
    }

    extra[4] = ' '; extra[5] = 'r'; extra[6] = 'c'; extra[7] = '=';
    extra[8] = '0'; extra[9] = '\0';
    os_trace_mode_switch(g_pcbs[g_current_proc].pid, "KERNEL_TO_USER", "syscall_return", extra);
}
