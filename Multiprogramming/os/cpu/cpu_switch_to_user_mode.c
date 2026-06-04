#include "cpu.h"
/* Salto a modo Usuario. Afecta: PC, SP, LR, CPSR. Deps: cpu_set_user_regs. */
void cpu_switch_to_user_mode(uint32_t pc, uint32_t sp, uint32_t lr) {
    cpu_set_user_regs(sp, lr);
    __asm__ volatile (
        "msr spsr_cxsf, %1\n\t"
        "mov lr, %0\n\t"
        "movs pc, lr\n\t"
        :: "r"(pc), "r"(0x10u)
        : "memory");
    for(;;);
}
