#include <stdint.h>

typedef uint32_t u32;

#define MMIO32(addr) (*(volatile u32 *)(addr))

#define VIC_BASE        0x10140000u
#define VIC_IRQSTATUS   0x00u
#define VIC_INTENABLE   0x10u
#define VIC_VECTADDR    0x30u

/* Timer0 IRQ en versatilepb = 4 */
#define TIMER0_IRQ (1u << 4)

static inline void mmio_write(u32 addr, u32 val) {
    MMIO32(addr) = val;
}

void intc_init(void) {
    /* habilitar interrupción del timer */
    mmio_write(VIC_BASE + VIC_INTENABLE, TIMER0_IRQ);
}