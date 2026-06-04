#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

#define MMIO32(addr) (*(volatile uint32_t *)(addr))

static inline void mmio_write(uint32_t addr, uint32_t value) {
    MMIO32(addr) = value;
}

static inline uint32_t mmio_read(uint32_t addr) {
    return MMIO32(addr);
}

#endif
