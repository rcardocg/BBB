#include <stdint.h>

#include "pcb.h"
#include "cpu.h"
#include "timer.h"
#include "intc.h"

typedef uint32_t u32;

#define MMIO32(addr) (*(volatile u32 *)(addr))

/* UART0 - QEMU PL011 */
#define UART0_BASE 0x101f1000u
#define UART_DR 0x00u
#define UART_FR 0x18u
#define UART_FR_TXFF (1u << 5)

/* WDT1 */
#define WDT1_BASE 0x44E35000u
#define WDT_WSPR 0x48u
#define WDT_WWPS 0x34u
#define WDT_WWPS_W_PEND (1u << 4)

/* Process addresses (loaded by loader) */
#define P1_ENTRY 0x00020000u
#define P2_ENTRY 0x00030000u
#define P1_STACK_TOP 0x00030000u
#define P2_STACK_TOP 0x00040000u

#define P1_PID 1u
#define P2_PID 2u
#define NUM_USER_PROCS 2u
#define INITIAL_PROC_SPSR 0x10u

static inline void mmio_write(u32 addr, u32 value) { MMIO32(addr) = value; }
static inline u32 mmio_read(u32 addr) { return MMIO32(addr); }

static void uart_putc(char c)
{
    while (mmio_read(UART0_BASE + UART_FR) & UART_FR_TXFF)
    {
    }
    mmio_write(UART0_BASE + UART_DR, (u32)c);
}

static void uart_puts(const char *s)
{
    while (*s != '\0')
    {
        if (*s == '\n')
        {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}

static void disable_wdt1(void)
{
    mmio_write(WDT1_BASE + WDT_WSPR, 0xAAAAu);
    while ((mmio_read(WDT1_BASE + WDT_WWPS) & WDT_WWPS_W_PEND) != 0u)
    {
    }

    mmio_write(WDT1_BASE + WDT_WSPR, 0x5555u);
    while ((mmio_read(WDT1_BASE + WDT_WWPS) & WDT_WWPS_W_PEND) != 0u)
    {
    }
}

static pcb_t g_pcbs[NUM_USER_PROCS];
static u32 g_current_proc;

void timer_irq_handler(u32 *irq_frame);

static void pcb_system_init(void)
{
    pcb_init(&g_pcbs[0], P1_PID, P1_ENTRY, P1_STACK_TOP, INITIAL_PROC_SPSR, PROC_RUNNING);
    pcb_init(&g_pcbs[1], P2_PID, P2_ENTRY, P2_STACK_TOP, INITIAL_PROC_SPSR, PROC_READY);
    g_current_proc = 0u;
    __asm__ volatile("dsb\n\tisb" ::: "memory");
}

void syscall_dispatcher(u32 *frame)
{
    u32 syscall_num = frame[7]; // R7

    switch (syscall_num)
    {
    case 1: // SYS_WRITE
        uart_putc((char)frame[0]);
        break;
    case 2: // SYS_YIELD
        timer_irq_handler(frame);
        break;
    default:
        uart_puts("\n[KERNEL] Unknown syscall\n");
        break;
    }
}

void fault_handler(u32 *frame)
{
    (void)frame;
    uart_puts("\n\n**********************************");
    uart_puts("\n[KERNEL] FATAL ERROR: PROCESS ABORT");
    uart_puts("\n**********************************\n");
    for (;;)
    {
    }
}

void timer_irq_handler(u32 *irq_frame)
{
    u32 usr_sp;
    u32 usr_lr;
    u32 next_proc;

    /* ACK timer + EOI so interrupts continue */
    mmio_write(0x101E2000u + 0x0C, 1u); // clear timer interrupt
    mmio_write(0x10140000u + 0x30, 0u); // EOI VIC

    cpu_get_user_regs(&usr_sp, &usr_lr);
    pcb_save_from_irq_frame(&g_pcbs[g_current_proc], irq_frame, usr_sp, usr_lr);
    pcb_set_state(&g_pcbs[g_current_proc], PROC_READY);

    next_proc = (g_current_proc + 1u) % NUM_USER_PROCS;

    pcb_restore_to_irq_frame(&g_pcbs[next_proc], irq_frame, &usr_sp, &usr_lr);
    cpu_set_user_regs(usr_sp, usr_lr);
    pcb_set_state(&g_pcbs[next_proc], PROC_RUNNING);
    g_current_proc = next_proc;
}

void kmain(void)
{
    disable_wdt1();
    uart_puts("\n[OS] boot (RR)\n");

    pcb_system_init();

    timer_init();
    intc_init();
    enable_irq();

    uart_puts("[OS] jumping to P1\n");

    cpu_switch_to_user_mode(g_pcbs[g_current_proc].pc,
                            g_pcbs[g_current_proc].sp,
                            g_pcbs[g_current_proc].lr);

    for (;;)
    {
    }
}
