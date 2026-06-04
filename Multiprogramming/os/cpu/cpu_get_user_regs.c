#include "cpu.h"
void cpu_get_user_regs(uint32_t *sp_out, uint32_t *lr_out) {
    __asm__ volatile (
        "mrs r2, cpsr\n\t"
        "bic r3, r2, #0x1F\n\t"
        "orr r3, r3, #0x1F\n\t"
        "orr r3, r3, #0x80\n\t"
        "msr cpsr_c, r3\n\t"
        "mov r3, sp\n\t"
        "mov ip, lr\n\t"
        "msr cpsr_c, r2\n\t"
        "str r3, [%0]\n\t"
        "str ip, [%1]\n\t"
        :
        : "r"(sp_out), "r"(lr_out)
        : "r2", "r3", "ip", "cc", "memory");
}
