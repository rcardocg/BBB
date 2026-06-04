#include <stdint.h>
#include "pcb.h"
#include "cpu.h"
#include "drivers/uart.h"
#include "drivers/wdt.h"
#include "drivers/timer.h"
#include "scheduler/scheduler.h"

void os_timer_irq_handler(uint32_t *irq_frame);

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

    if ((spsr & 0x1Fu) != 0x10u) {
        frame[0] = 0xFFFFFFFFu;
        return;
    }

    if (syscall_num == 0u) {
        os_timer_irq_handler(frame);
        frame[0] = 0u;
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
        frame[0] = 0u;
    } else if (syscall_num == 2u) {
        frame[0] = (uint32_t)os_handle_sys_write(frame[1], (const char *)frame[2], frame[3]);
    } else {
        frame[0] = 0xFFFFFFFFu;
    }
}

void os_fault_handler(uint32_t *frame) {
    (void)frame;
    scheduler_terminate_current();

    if (!scheduler_has_runnable()) {
        uart_puts("\n[KERNEL] all user tasks faulted/terminated\n");
        for (;;) {
        }
    }

    scheduler_next();
    {
        pcb_t *next_pcb = scheduler_get_current_pcb();
        cpu_set_user_regs(next_pcb->sp, next_pcb->lr);
        pcb_set_state(next_pcb, PROC_RUNNING);
        __asm__ volatile (
            "msr spsr_cxsf, %0\n\t"
            "mov lr, %1\n\t"
            "movs pc, lr\n\t"
            :
            : "r"(next_pcb->spsr), "r"(next_pcb->pc)
            : "memory");
    }

    for (;;) {
    }
}

void os_timer_irq_handler(uint32_t *irq_frame) {
    uint32_t usr_sp;
    uint32_t usr_lr;
    pcb_t *current_pcb;
    pcb_t *next_pcb;

    /* ACK timer + EOI so interrupts continue */
    timer_ack_interrupt();

    cpu_get_user_regs(&usr_sp, &usr_lr);
    
    current_pcb = scheduler_get_current_pcb();
    pcb_save_from_irq_frame(current_pcb, irq_frame, usr_sp, usr_lr);
    if (current_pcb->state != PROC_TERMINATED) {
        pcb_set_state(current_pcb, PROC_READY);
    }

    scheduler_next();

    next_pcb = scheduler_get_current_pcb();
    pcb_restore_to_irq_frame(next_pcb, irq_frame, &usr_sp, &usr_lr);
    cpu_set_user_regs(usr_sp, usr_lr);
    pcb_set_state(next_pcb, PROC_RUNNING);
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
    cpu_switch_to_user_mode(current->pc, current->sp, current->lr);

    for (;;) {
    }
}
