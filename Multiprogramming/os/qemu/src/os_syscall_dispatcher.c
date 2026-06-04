#include "os_qemu.h"
#include "../pcb.h"
#include "../../cpu/cpu.h"

extern pcb_t g_pcbs[2];
extern uint32_t g_current_proc;

static uint32_t next_runnable_after(uint32_t current) {
    uint32_t i;

    for (i = 1u; i <= 2u; i = i + 1u) {
        uint32_t candidate = (current + i) % 2u;
        if (g_pcbs[candidate].state != PROC_TERMINATED) {
            return candidate;
        }
    }

    return current;
}

static void trace_return(uint32_t syscall_num, int32_t rc) {
    char extra[16];

    extra[0] = 'i';
    extra[1] = 'd';
    extra[2] = '=';
    extra[3] = (char)('0' + (char)syscall_num);
    extra[4] = ' ';
    extra[5] = 'r';
    extra[6] = 'c';
    extra[7] = '=';
    if (rc < 0) {
        extra[8] = '-';
        extra[9] = (char)('0' + (char)(-rc));
        extra[10] = '\0';
    } else {
        extra[8] = (char)('0' + (char)(rc % 10));
        extra[9] = '\0';
    }

    os_trace_mode_switch(g_pcbs[g_current_proc].pid, "KERNEL_TO_USER", "syscall_return", extra);
}

static int32_t handle_sys_write(uint32_t fd, const char *buf, uint32_t len) {
    uint32_t base = (g_current_proc == 0u) ? 0x00020000u : 0x00030000u;
    uint32_t limit = base + 0x00010000u;
    uint32_t addr = (uint32_t)buf;
    uint32_t i;

    if (fd != 1u || len > 256u) {
        return -2;
    }

    if (addr < base || addr > limit || len > (limit - addr)) {
        return -3;
    }

    for (i = 0u; i < len; i = i + 1u) {
        os_uart_putc(buf[i]);
    }

    return (int32_t)len;
}

/* Despacha syscalls con traces de modo (Inciso 3.7). Afecta: UART0, CPU, PCB. Deps: os_trace_mode_switch, os_handle_sys_yield. */
void os_syscall_dispatcher(uint32_t *frame) {
    uint32_t syscall_num = frame[0];
    uint32_t spsr = frame[14];
    char extra[16];
    int32_t rc = -1;

    if ((spsr & 0x1Fu) != 0x10u) {
        os_uart_puts("\n[KERNEL] Syscall rejected: not from USR mode\n");
        frame[0] = 0xFFFFFFFFu;
        return;
    }

    extra[0] = 'i'; extra[1] = 'd'; extra[2] = '=';
    extra[3] = (char)('0' + (char)syscall_num);
    extra[4] = '\0';
    os_trace_mode_switch(g_pcbs[g_current_proc].pid, "USER_TO_KERNEL", "syscall", extra);

    if (syscall_num == 0u) {
        os_handle_sys_yield(frame);
        rc = 0;
    } else if (syscall_num == 1u) {
        uint32_t usr_sp;
        uint32_t usr_lr;
        uint32_t next_proc;

        (void)frame[1];
        g_pcbs[g_current_proc].state = PROC_TERMINATED;
        next_proc = next_runnable_after(g_current_proc);
        if (next_proc == g_current_proc) {
            os_uart_puts("\n[KERNEL] all user tasks terminated\n");
            for (;;) {
            }
        }

        g_current_proc = next_proc;
        pcb_restore_to_irq_frame(&g_pcbs[g_current_proc], frame, &usr_sp, &usr_lr);
        cpu_set_user_regs(usr_sp, usr_lr);
        pcb_set_state(&g_pcbs[g_current_proc], PROC_RUNNING);
        rc = 0;
    } else if (syscall_num == 2u) {
        rc = handle_sys_write(frame[1], (const char *)frame[2], frame[3]);
        frame[0] = (uint32_t)rc;
    } else {
        os_uart_puts("\n[KERNEL] Unknown syscall\n");
        frame[0] = 0xFFFFFFFFu;
        rc = -1;
    }

    trace_return(syscall_num, rc);
}
