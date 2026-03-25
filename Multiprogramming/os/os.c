#include <stdint.h>

typedef uint32_t u32;

#define MMIO32(addr) (*(volatile u32 *)(addr))

/* UART0 */
#define UART0_BASE      0x44E09000u
#define UART_THR        0x00u
#define UART_LSR        0x14u
#define UART_LSR_THRE   (1u << 5)

/* WDT1 */
#define WDT1_BASE       0x44E35000u
#define WDT_WSPR        0x48u
#define WDT_WWPS        0x34u
#define WDT_WWPS_W_PEND (1u << 4)

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

/* Process addresses (loaded by U-Boot) */
#define P1_ENTRY      0x82100000u
#define P2_ENTRY      0x82200000u
#define P1_STACK_TOP  0x82112000u

/* 3 seconds at 24MHz */
#define TIMER2_RELOAD 0xFBB55E00u
/* 100ms = 0xFFE92800*/

static inline void mmio_write(u32 addr, u32 value) { MMIO32(addr) = value; }
static inline u32 mmio_read(u32 addr) { return MMIO32(addr); }

static inline void enable_irq(void) {
    __asm__ volatile ("cpsie i" ::: "memory");
}

static void uart_putc(char c) {
    while ((mmio_read(UART0_BASE + UART_LSR) & UART_LSR_THRE) == 0u) {
    }
    mmio_write(UART0_BASE + UART_THR, (u32)c);
}

static void uart_puts(const char *s) {
    while (*s != '\0') {
        uart_putc(*s++);
    }
}

static void disable_wdt1(void) {
    mmio_write(WDT1_BASE + WDT_WSPR, 0xAAAAu);
    while ((mmio_read(WDT1_BASE + WDT_WWPS) & WDT_WWPS_W_PEND) != 0u) {
    }

    mmio_write(WDT1_BASE + WDT_WSPR, 0x5555u);
    while ((mmio_read(WDT1_BASE + WDT_WWPS) & WDT_WWPS_W_PEND) != 0u) {
    }
}

static void timer2_init(void) {
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

static void intc_init(void) {
    mmio_write(INTC_BASE + INTC_MIR_CLEAR2, IRQ68_MASK);
}

void timer_irq_handler(void) {
    /* ACK timer + EOI so interrupts continue */
    mmio_write(DMT2_BASE + TISR, TISR_OVF_IT_FLAG);
    mmio_write(INTC_BASE + INTC_CONTROL, INTC_NEWIRQAGR);

    /* Minimal marker to prove IRQ is firing */
    uart_putc('.');
}

typedef void (*entry_fn_t)(void);

void kmain(void) {
    disable_wdt1();
    uart_puts("\n[OS] boot (simple)\n");

    timer2_init();
    intc_init();
    enable_irq();

    uart_puts("[OS] jumping to P1\n");

    /* Set P1 stack (SVC mode) and jump. */
    __asm__ volatile ("mov sp, %0" :: "r"(P1_STACK_TOP) : "memory");
    ((entry_fn_t)(uintptr_t)P1_ENTRY)();

    for (;;) {
    }
}
