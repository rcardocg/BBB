#include "cpu.h"
void cpu_get_user_regs(uint32_t *sp_out, uint32_t *lr_out) {
    uint32_t sp_val, lr_val;
    __asm__ volatile (
        "mrs r3, cpsr\n\t"
        "bic r2, r3, #0x1F\n\t"
        "orr r2, r2, #0x1F\n\t"
        "orr r2, r2, #0x80\n\t"
        "msr cpsr_c, r2\n\t"
        "mov %1, lr\n\t"
        "mov %0, sp\n\t"
        "msr cpsr_c, r3\n\t"
        : "=r"(sp_val), "=r"(lr_val)
        :
        : "r2", "r3", "cc", "memory");
    *sp_out = sp_val;
    *lr_out = lr_val;
}
