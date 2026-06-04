#include "cpu.h"
/* Obtiene registros SP y LR de Usuario. Afecta: SP, LR (System). Deps: cpu_read_cpsr, cpu_write_cpsr_c. */
void cpu_get_user_regs(uint32_t *sp_out, uint32_t *lr_out) {
    uint32_t old_cpsr = cpu_read_cpsr();
    uint32_t sys_cpsr = (old_cpsr & ~0x1Fu) | 0x1Fu | 0x80u;
    uint32_t sp_val, lr_val;
    cpu_write_cpsr_c(sys_cpsr);
    __asm__ volatile ("mov %0, sp\n\tmov %1, lr" : "=r"(sp_val), "=r"(lr_val) :: "memory");
    cpu_write_cpsr_c(old_cpsr);
    *sp_out = sp_val;
    *lr_out = lr_val;
}
