#ifndef CPU_H
#define CPU_H

#include <stdint.h>

static inline uint32_t read_cpsr(void) {
    uint32_t v;
    __asm__ volatile ("mrs %0, cpsr" : "=r"(v));
    return v;
}

static inline void write_cpsr_c(uint32_t v) {
    __asm__ volatile ("msr cpsr_c, %0" :: "r"(v) : "cc", "memory");
}

static inline void enable_irq(void) {
    __asm__ volatile ("cpsie i" ::: "memory");
}

static inline void read_svc_sp_lr(uint32_t *sp_out, uint32_t *lr_out) {
    uint32_t old_cpsr;
    uint32_t svc_cpsr;
    uint32_t sp_val;
    uint32_t lr_val;

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

static inline void write_svc_sp_lr(uint32_t sp, uint32_t lr) {
    uint32_t old_cpsr;
    uint32_t svc_cpsr;

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

#endif
