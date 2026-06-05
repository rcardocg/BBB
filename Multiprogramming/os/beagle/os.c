#include <stdint.h>
#include "pcb.h"
#include "cpu.h"
#include "drivers/uart.h"
#include "drivers/wdt.h"
#include "drivers/timer.h"
#include "scheduler/scheduler.h"

void os_timer_irq_handler(uint32_t *irq_frame);

void os_trace_mode_switch(uint32_t pid, const char *direction, const char *reason, const char *extra) {
    uart_puts("\nMODE_SWITCH ");
    uart_puts(direction);
    uart_puts(" pid=");
    uart_putc((char)(pid + '0'));
    uart_puts(" reason=");
    uart_puts(reason);
    if (extra != 0) {
        uart_puts(" ");
        uart_puts(extra);
    }
    uart_puts("\n");
}

static void trace_return(uint32_t syscall_num, int32_t rc) {
    char extra[16];
    pcb_t *current = scheduler_get_current_pcb();

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

    os_trace_mode_switch(current->pid, "KERNEL_TO_USER", "syscall_return", extra);
}

void os_handle_sys_yield(uint32_t *frame) {
    uint32_t usr_sp;
    uint32_t usr_lr;
    pcb_t *current_pcb;
    pcb_t *next_pcb;

    cpu_get_user_regs(&usr_sp, &usr_lr);
    
    current_pcb = scheduler_get_current_pcb();
    pcb_save_from_irq_frame(current_pcb, frame, usr_sp, usr_lr);
    if (current_pcb->state != PROC_TERMINATED) {
        pcb_set_state(current_pcb, PROC_READY);
    }

    scheduler_next();

    next_pcb = scheduler_get_current_pcb();
    pcb_restore_to_irq_frame(next_pcb, frame, &usr_sp, &usr_lr);
    cpu_set_user_regs(usr_sp, usr_lr);
    pcb_set_state(next_pcb, PROC_RUNNING);
    frame[0] = 0u;
}

static int32_t os_handle_sys_write(uint32_t fd, const char *buf, uint32_t len) {
    pcb_t *current = scheduler_get_current_pcb();
    uint32_t base = (current->pid == 1u) ? 0x82100000u : 0x82200000u;
    uint32_t limit = base + 0x00012000u;
    uint32_t addr = (uint32_t)buf;
    uint32_t i;

    if (fd != 1u || len > 256u) {
        return -2;
    }

    if (addr < base || addr > limit || len > (limit - addr)) {
        return -3;
    }

    for (i = 0u; i < len; i = i + 1u) {
        uart_putc(buf[i]);
    }

    return (int32_t)len;
}

void os_syscall_dispatcher(uint32_t *frame) {
    uint32_t syscall_num = frame[0];
    uint32_t spsr = frame[14];
    char extra[16];
    int32_t rc = -1;
    pcb_t *current = scheduler_get_current_pcb();

    if ((spsr & 0x1Fu) != 0x10u) {
        uart_puts("\n[KERNEL] Syscall rejected: not from USR mode\n");
        frame[0] = 0xFFFFFFFFu;
        return;
    }

    extra[0] = 'i'; extra[1] = 'd'; extra[2] = '=';
    extra[3] = (char)('0' + (char)syscall_num);
    extra[4] = '\0';
    os_trace_mode_switch(current->pid, "USER_TO_KERNEL", "syscall", extra);

    if (syscall_num == 0u) {
        os_handle_sys_yield(frame);
        rc = 0;
    } else if (syscall_num == 1u) {
        uint32_t usr_sp;
        uint32_t usr_lr;
        pcb_t *next_pcb;

        scheduler_terminate_current();
        if (!scheduler_has_runnable()) {
            uart_puts("\n[KERNEL] all user tasks terminated\n");
            for (;;) {
            }
        }

        scheduler_next();
        next_pcb = scheduler_get_current_pcb();
        pcb_restore_to_irq_frame(next_pcb, frame, &usr_sp, &usr_lr);
        cpu_set_user_regs(usr_sp, usr_lr);
        pcb_set_state(next_pcb, PROC_RUNNING);
        rc = 0;
    } else if (syscall_num == 2u) {
        rc = os_handle_sys_write(frame[1], (const char *)frame[2], frame[3]);
        frame[0] = (uint32_t)rc;
    } else {
        uart_puts("\n[KERNEL] Unknown syscall\n");
        frame[0] = 0xFFFFFFFFu;
        rc = -1;
    }

    trace_return(syscall_num, rc);
}

static const char* decode_fsr(uint32_t fsr) {
    uint32_t status = (fsr & 0xFu) | ((fsr & (1u << 10)) >> 6);
    if (status == 0x1u) return "type=alignment";
    if (status == 0x5u) return "type=translation_section";
    if (status == 0x7u) return "type=translation_page";
    if (status == 0xDu) return "type=permission_section";
    if (status == 0xFu) return "type=permission_page";
    return "type=unknown";
}

void os_fault_handler(uint32_t *frame, uint32_t fault_type) {
    (void)frame;
    pcb_t *current = scheduler_get_current_pcb();
    const char *reason = "type=undefined";

    if (fault_type == 1u) {
        reason = decode_fsr(cpu_read_ifsr());
    } else if (fault_type == 2u) {
        reason = decode_fsr(cpu_read_dfsr());
    }

    os_trace_mode_switch(current->pid, "USER_TO_KERNEL", "fault", reason);

    scheduler_terminate_current();

    if (!scheduler_has_runnable()) {
        uart_puts("\n[KERNEL] all user tasks faulted/terminated\n");
        for (;;) {
        }
    }

    scheduler_next();
    {
        uint32_t usr_sp;
        uint32_t usr_lr;
        pcb_t *next_pcb = scheduler_get_current_pcb();
        os_trace_mode_switch(next_pcb->pid, "KERNEL_TO_USER", "fault_recovery", 0);
        
        pcb_restore_to_irq_frame(next_pcb, frame, &usr_sp, &usr_lr);
        cpu_set_user_regs(usr_sp, usr_lr);
        pcb_set_state(next_pcb, PROC_RUNNING);
        
        return;
    }
}

void os_timer_irq_handler(uint32_t *irq_frame) {
    uint32_t usr_sp;
    uint32_t usr_lr;
    pcb_t *current_pcb;
    pcb_t *next_pcb;

    current_pcb = scheduler_get_current_pcb();
    os_trace_mode_switch(current_pcb->pid, "USER_TO_KERNEL", "timer_irq", 0);

    /* ACK timer + EOI so interrupts continue */
    timer_ack_interrupt();

    cpu_get_user_regs(&usr_sp, &usr_lr);
    
    pcb_save_from_irq_frame(current_pcb, irq_frame, usr_sp, usr_lr);
    if (current_pcb->state != PROC_TERMINATED) {
        pcb_set_state(current_pcb, PROC_READY);
    }

    scheduler_next();

    next_pcb = scheduler_get_current_pcb();
    pcb_restore_to_irq_frame(next_pcb, irq_frame, &usr_sp, &usr_lr);
    cpu_set_user_regs(usr_sp, usr_lr);
    pcb_set_state(next_pcb, PROC_RUNNING);
    os_trace_mode_switch(next_pcb->pid, "KERNEL_TO_USER", "dispatch", 0);
}

void os_kmain(void) {
    wdt_disable();
    uart_puts("\n[OS] boot (Modular RR)\n");

    scheduler_init();

    timer_init();
    intc_init();
    enable_irq();

    uart_puts("[OS] jumping to P1\n");

    pcb_t *current = scheduler_get_current_pcb();
    os_trace_mode_switch(current->pid, "KERNEL_TO_USER", "initial_launch", 0);
    cpu_switch_to_user_mode(current->pc, current->sp, current->lr);

    for (;;) {
    }
}
