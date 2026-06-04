#include "cpu.h"
/* Establece registros SP y LR de Usuario. Afecta: SP, LR (System). Deps: cpu_read_cpsr, cpu_write_cpsr_c. */
void cpu_set_user_regs(uint32_t sp, uint32_t lr) {
    uint32_t old_cpsr = cpu_read_cpsr();
    uint32_t sys_cpsr = (old_cpsr & ~0x1Fu) | 0x1Fu | 0x80u;
    cpu_write_cpsr_c(sys_cpsr);
    __asm__ volatile ("mov sp, %0\n\tmov lr, %1" :: "r"(sp), "r"(lr) : "memory");
    cpu_write_cpsr_c(old_cpsr);
}
