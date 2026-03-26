#include "timer.h"
#include "mmio.h"

/* DMTimer2 */
#define DMT2_BASE        0x48040000u
#define TISR             0x28u
#define TIER             0x2Cu
#define TCLR             0x38u
#define TCRR             0x3Cu
#define TLDR             0x40u
#define TTGR             0x44u
#define TISR_OVF_IT_FLAG (1u << 1)
#define TIER_OVF_EN      (1u << 1)
#define TCLR_ST          (1u << 0)
#define TCLR_AR          (1u << 1)

/* CM_PER (clock control) */
#define CM_PER_BASE           0x44E00000u
#define CM_PER_TIMER2_CLKCTRL 0x80u

/* INTC */
#define INTC_BASE        0x48200000u
#define INTC_MIR_CLEAR2  0xC8u
#define INTC_CONTROL     0x48u
#define INTC_NEWIRQAGR   (1u << 0)
#define IRQ68_MASK       (1u << 4)

/* 3 seconds at 24MHz */
#define TIMER2_RELOAD 0xFBB55E00u

void timer_init(void) {
    mmio_write(CM_PER_BASE + CM_PER_TIMER2_CLKCTRL, 0x2u);
    while ((mmio_read(CM_PER_BASE + CM_PER_TIMER2_CLKCTRL) & 0x3u) != 0x2u) {
    }

    mmio_write(INTC_BASE + 0x100u + (68u * 4u), 0x0u);

    mmio_write(DMT2_BASE + TCLR, 0u);
    mmio_write(DMT2_BASE + TLDR, TIMER2_RELOAD);
    mmio_write(DMT2_BASE + TCRR, TIMER2_RELOAD);
    mmio_write(DMT2_BASE + TISR, 0x7u);
    mmio_write(DMT2_BASE + TIER, TIER_OVF_EN);
    mmio_write(DMT2_BASE + TTGR, 1u);
    mmio_write(DMT2_BASE + TCLR, TCLR_ST | TCLR_AR);
}

void intc_init(void) {
    mmio_write(INTC_BASE + INTC_MIR_CLEAR2, IRQ68_MASK);
}

void timer_ack_interrupt(void) {
    /* ACK timer + EOI so interrupts continue */
    mmio_write(DMT2_BASE + TISR, TISR_OVF_IT_FLAG);
    mmio_write(INTC_BASE + INTC_CONTROL, INTC_NEWIRQAGR);
}
