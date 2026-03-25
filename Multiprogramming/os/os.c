#include <stdint.h>

#include "pcb.h"

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
#define P2_STACK_TOP  0x82212000u

#define P1_PID 1u
#define P2_PID 2u
#define NUM_USER_PROCS 2u
#define INITIAL_PROC_SPSR 0x13u

/* 3 seconds at 24MHz */
#define TIMER2_RELOAD 0xFBB55E00u
/* 100ms = 0xFFE92800*/

static inline void mmio_write(u32 addr, u32 value) { MMIO32(addr) = value; }
static inline u32 mmio_read(u32 addr) { return MMIO32(addr); }

static inline u32 read_cpsr(void) {
    u32 v;
    __asm__ volatile ("mrs %0, cpsr" : "=r"(v));
    return v;
}

static inline void write_cpsr_c(u32 v) {
    __asm__ volatile ("msr cpsr_c, %0" :: "r"(v) : "cc", "memory");
}

static inline void enable_irq(void) {
    __asm__ volatile ("cpsie i" ::: "memory");
}

static void read_svc_sp_lr(u32 *sp_out, u32 *lr_out) {
    u32 old_cpsr;
    u32 svc_cpsr;
    u32 sp_val;
    u32 lr_val;

    old_cpsr = read_cpsr();
    svc_cpsr = (old_cpsr & ~0x1Fu) | 0x13u | 0x80u;

    write_cpsr_c(svc_cpsr);
    __asm__ volatile (
        "mov %0, sp\n\t"
        "mov %1, lr\n\t"
        : "=r"(sp_val), "=r"(lr_val)
        :
        : "memory");
    write_cpsr_c(old_cpsr);

    *sp_out = sp_val;
    *lr_out = lr_val;
}

static void write_svc_sp_lr(u32 sp, u32 lr) {
    u32 old_cpsr;
    u32 svc_cpsr;

    old_cpsr = read_cpsr();
    svc_cpsr = (old_cpsr & ~0x1Fu) | 0x13u | 0x80u;

    write_cpsr_c(svc_cpsr);
    __asm__ volatile (
        "mov sp, %0\n\t"
        "mov lr, %1\n\t"
        :
        : "r"(sp), "r"(lr)
        : "memory");
    write_cpsr_c(old_cpsr);
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

static pcb_t g_pcbs[NUM_USER_PROCS];
static u32 g_current_proc;

static void pcb_system_init(void) {
    pcb_init(&g_pcbs[0], P1_PID, P1_ENTRY, P1_STACK_TOP, INITIAL_PROC_SPSR, PROC_RUNNING);
    pcb_init(&g_pcbs[1], P2_PID, P2_ENTRY, P2_STACK_TOP, INITIAL_PROC_SPSR, PROC_READY);
    g_current_proc = 0u;
    __asm__ volatile ("dsb\n\tisb" ::: "memory");
}

void timer_irq_handler(u32 *irq_frame) {
    u32 svc_sp;
    u32 svc_lr;
    u32 next_proc;

    /* ACK timer + EOI so interrupts continue */
    mmio_write(DMT2_BASE + TISR, TISR_OVF_IT_FLAG);
    mmio_write(INTC_BASE + INTC_CONTROL, INTC_NEWIRQAGR);

    read_svc_sp_lr(&svc_sp, &svc_lr);
    pcb_save_from_irq_frame(&g_pcbs[g_current_proc], irq_frame, svc_sp, svc_lr);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_READY);

    next_proc = (g_current_proc + 1u) % NUM_USER_PROCS;

    pcb_restore_to_irq_frame(&g_pcbs[next_proc], irq_frame, &svc_sp, &svc_lr);
    write_svc_sp_lr(svc_sp, svc_lr);
    pcb_set_state(&g_pcbs[next_proc], PROC_RUNNING);
    g_current_proc = next_proc;
}

typedef void (*entry_fn_t)(void);

void kmain(void) {
    disable_wdt1();
    uart_puts("\n[OS] boot (RR)\n");

    pcb_system_init();

    timer2_init();
    intc_init();
    enable_irq();

    uart_puts("[OS] jumping to P1\n");

    write_svc_sp_lr(g_pcbs[g_current_proc].sp, g_pcbs[g_current_proc].lr);
    ((entry_fn_t)(uintptr_t)g_pcbs[g_current_proc].pc)();

    for (;;) {
    }
}
