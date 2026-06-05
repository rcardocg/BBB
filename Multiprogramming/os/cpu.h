#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/**
 * @brief Lee el registro de estado actual (CPSR).
 */
static inline uint32_t read_cpsr(void) {
    uint32_t v;
    __asm__ volatile ("mrs %0, cpsr" : "=r"(v));
    return v;
}

/**
 * @brief Escribe en los bits de control del CPSR.
 */
static inline void write_cpsr_c(uint32_t v) {
    __asm__ volatile ("msr cpsr_c, %0" :: "r"(v) : "cc", "memory");
}

/**
 * @brief Habilita las interrupciones IRQ globalmente.
 */
static inline void enable_irq(void) {
    __asm__ volatile ("cpsie i" ::: "memory");
}

static inline uint32_t cpu_read_dfsr(void) {
    uint32_t v;
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r"(v));
    return v;
}

static inline uint32_t cpu_read_ifsr(void) {
    uint32_t v;
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 1" : "=r"(v));
    return v;
}

/**
 * @brief Obtiene los registros SP y LR del modo Usuario.
 * 
 * Utiliza el modo System (0x1F) para acceder a los registros mapeados
 * del usuario desde un modo privilegiado.
 */
static inline void cpu_get_user_regs(uint32_t *sp_out, uint32_t *lr_out) {
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

/**
 * @brief Establece los registros SP y LR del modo Usuario.
 */
static inline void cpu_set_user_regs(uint32_t sp, uint32_t lr) {
    uint32_t old_cpsr = read_cpsr();
    uint32_t sys_cpsr = (old_cpsr & ~0x1Fu) | 0x1Fu | 0x80u;

    write_cpsr_c(sys_cpsr);
    __asm__ volatile (
        "mov sp, %0\n\t"
        "mov lr, %1\n\t"
        :: "r"(sp), "r"(lr)
        : "memory");
    write_cpsr_c(old_cpsr);
}

/**
 * @brief Realiza el salto inicial al primer proceso en Modo Usuario.
 * 
 * Esta función prepara el SPSR para que al ejecutar 'movs pc, lr' el CPU 
 * baje automáticamente a Modo Usuario (0x10).
 */
static inline __attribute__((noreturn)) void cpu_switch_to_user_mode(uint32_t pc,
                                                                    uint32_t sp,
                                                                    uint32_t lr) {
    cpu_set_user_regs(sp, lr);
    
    __asm__ volatile (
        "msr spsr_cxsf, %1\n\t" // Preparamos modo Usuario (0x10)
        "mov lr, %0\n\t"        // Cargamos PC de entrada
        "movs pc, lr\n\t"       // Salto con cambio de modo
        :: "r"(pc), "r"(0x10u)
        : "memory");

    __builtin_unreachable();
}

#endif
