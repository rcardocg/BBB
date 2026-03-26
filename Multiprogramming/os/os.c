#include <stdint.h>
#include "pcb.h"
#include "cpu.h"
#include "drivers/uart.h"
#include "drivers/wdt.h"
#include "drivers/timer.h"
#include "scheduler/scheduler.h"

void timer_irq_handler(uint32_t *irq_frame) {
    uint32_t svc_sp;
    uint32_t svc_lr;
    pcb_t *current_pcb;
    pcb_t *next_pcb;

    /* ACK timer + EOI so interrupts continue */
    timer_ack_interrupt();

    read_svc_sp_lr(&svc_sp, &svc_lr);
    
    current_pcb = scheduler_get_current_pcb();
    pcb_save_from_irq_frame(current_pcb, irq_frame, svc_sp, svc_lr);
    pcb_set_state(current_pcb, PROC_READY);

    scheduler_next();

    next_pcb = scheduler_get_current_pcb();
    pcb_restore_to_irq_frame(next_pcb, irq_frame, &svc_sp, &svc_lr);
    write_svc_sp_lr(svc_sp, svc_lr);
    pcb_set_state(next_pcb, PROC_RUNNING);
}

typedef void (*entry_fn_t)(void);

void kmain(void) {
    wdt_disable();
    uart_puts("\n[OS] boot (Modular RR)\n");

    scheduler_init();

    timer_init();
    intc_init();
    enable_irq();

    uart_puts("[OS] jumping to P1\n");

    pcb_t *current = scheduler_get_current_pcb();
    write_svc_sp_lr(current->sp, current->lr);
    ((entry_fn_t)(uintptr_t)current->pc)();

    for (;;) {
    }
}
