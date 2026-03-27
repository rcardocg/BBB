#include <stdint.h>

typedef uint32_t u32;

#define MMIO32(addr) (*(volatile u32 *)(addr))

/* SP804 Timer0 */
#define TIMER0_BASE   0x101E2000u
#define TIMER_LOAD    0x00u
#define TIMER_VALUE   0x04u
#define TIMER_CTRL    0x08u
#define TIMER_INTCLR  0x0Cu
#define TIMER_RIS     0x10u

/* Control bits */
#define TIMER_CTRL_ENABLE   (1u << 7)
#define TIMER_CTRL_PERIODIC (1u << 6)
#define TIMER_CTRL_INTEN    (1u << 5)
#define TIMER_CTRL_32BIT    (1u << 1)

/* Valor para la velocidad del RR */
#define TIMER_RELOAD  1000000u

static inline void mmio_write(u32 addr, u32 val) {
    MMIO32(addr) = val;
}

void timer_init(void) {
    /* cargar valor */
    mmio_write(TIMER0_BASE + TIMER_LOAD, TIMER_RELOAD);

    /* limpiar interrupciones */
    mmio_write(TIMER0_BASE + TIMER_INTCLR, 1u);

    /* modo periódico + IRQ + enable */
    mmio_write(TIMER0_BASE + TIMER_CTRL,
        TIMER_CTRL_ENABLE |
        TIMER_CTRL_PERIODIC |
        TIMER_CTRL_INTEN |
        TIMER_CTRL_32BIT
    );
}