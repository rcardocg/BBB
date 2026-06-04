#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/* Lee el registro de estado actual (CPSR). */
uint32_t cpu_read_cpsr(void);

/* Escribe en los bits de control del CPSR. */
void cpu_write_cpsr_c(uint32_t v);

/* Habilita las interrupciones IRQ globalmente. */
void cpu_enable_irq(void);

/* Lee el Data Fault Status Register (DFSR). */
uint32_t cpu_read_dfsr(void);

/* Lee el Instruction Fault Status Register (IFSR). */
uint32_t cpu_read_ifsr(void);

/* Obtiene los registros SP y LR del modo Usuario. */
void cpu_get_user_regs(uint32_t *sp_out, uint32_t *lr_out);

/* Establece los registros SP y LR del modo Usuario. */
void cpu_set_user_regs(uint32_t sp, uint32_t lr);

/* Realiza el salto inicial al primer proceso en Modo Usuario. */
void cpu_switch_to_user_mode(uint32_t pc, uint32_t sp, uint32_t lr);

#endif
