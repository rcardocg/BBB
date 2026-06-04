#include "cpu.h"
void cpu_set_user_regs(uint32_t sp, uint32_t lr) {
    __asm__ volatile (
        "mrs r3, cpsr\n\t"
        "bic r2, r3, #0x1F\n\t"
        "orr r2, r2, #0x1F\n\t"
        "orr r2, r2, #0x80\n\t"
        "msr cpsr_c, r2\n\t"
        "mov sp, %0\n\t"
        "mov lr, %1\n\t"
        "msr cpsr_c, r3\n\t"
        :: "r"(sp), "r"(lr)
        : "r2", "r3", "cc", "memory");
}
