# Preparando la fase 2

## 1. round_robin.c — Modo usuario en procesos
`os/scheduler/round_robin.c`:
- `INITIAL_PROC_SPSR` cambiado de `0x13` (Supervisor) a `0x10` (User) para que el CPU bloquee instrucciones sensibles desde los procesos.

## 2. root.s — Vector table + SVC + Aborts
`os/root.s`:
- Vector table actualizada: `svc_handler`, `prefetch_handler`, `abort_handler` apuntan a manejadores reales.
- `svc_handler`: guarda r0-r12, lr, spsr en el stack SVC, ajusta LR (+4) para convención IRQ-frame de `pcb_save/restore`, llama `os_syscall_dispatcher`, restaura LR (-4), restaura spsr y retorna con `movs pc, lr`.
- `prefetch_handler`: ajusta LR en -4, salta a `common_abort`.
- `abort_handler`: ajusta LR en -8, salta a `common_abort`.
- `common_abort`: guarda frame, llama `os_fault_handler`, hace bucle infinito.
- `irq_handler`: inalterado (ya existente).

## 3. syscall.s (NUEVO) — Interfaz usuario-syscall
`lib/qemu/syscall.s`:
- `__sys_write`: `mov r7, #1` + `svc #0`
- `__sys_yield`: `mov r7, #2` + `svc #0`

## 4. print.c — UART vía syscall
`lib/qemu/print.c`:
- `uart_putc` ya no escribe MMIO directo, ahora llama `__sys_write(c)` para cada carácter (y `\r` antes de `\n`).

## 5. Modularización: os.c → src/ (archivos atómicos)
El monolítico `os/qemu/os.c` se partió en archivos 1-función según `rules.md`:

### 5a. os_kmain.c — Punto de entrada
`os/qemu/src/os_kmain.c`:
- Ya no llama `enable_irq()` (Inciso 3.4: las IRQ se habilitan al entrar a USR vía SPSR=0x10).
- `os_trace_mode_switch` ahora usa 4 parámetros: direction, reason, extra.

### 5b. os_syscall_dispatcher.c — Despachador de syscalls
`os/qemu/src/os_syscall_dispatcher.c`:
- Valida que `spsr & 0x1F == 0x10` (solo desde USR mode).
- Rechaza con `0xFFFFFFFF` si no viene de USR.
- Traza `USER_TO_KERNEL` con `id=N` y `KERNEL_TO_USER` con `rc=0`.
- Syscall 1 (SYS_WRITE): escribe `frame[0]` vía `os_uart_putc`, retorna 0.
- Syscall 2 (SYS_YIELD): delega a `os_handle_sys_yield`.

### 5c. os_handle_sys_yield.c (NUEVO) — SYS_YIELD
`os/qemu/src/os_handle_sys_yield.c`:
- Obtiene SP/LR de usuario, guarda contexto en PCB, pasa al siguiente proceso, restaura contexto, actualiza estado.

### 5d. os_fault_handler.c — Manejador de fallos con recuperación
`os/qemu/src/os_fault_handler.c`:
- Lee `DFSR` para clasificar `DATA_ABORT` o `PREFETCH_ABORT`.
- Marca proceso actual como `PROC_TERMINATED`.
- Busca el siguiente proceso `PROC_READY` y salta a él (fault_recovery).
- Si no hay proceso listo, se cuelga (`for(;;)`).
- Antes solo imprimía y se colgaba.

### 5e. os_timer_irq_handler.c — Timer con traces
`os/qemu/src/os_timer_irq_handler.c`:
- Traza `USER_TO_KERNEL` (timer_irq) y `KERNEL_TO_USER` (dispatch) con la nueva firma de 4 parámetros.

### 5f. os_trace_mode_switch.c — Traces unificados
`os/qemu/src/os_trace_mode_switch.c`:
- Nueva firma: `(pid, direction, reason, extra)`.
- Imprime `MODE_SWITCH <direction> pid=<N> reason=<reason> <extra>`.

### 5g. os_trace_fault.c — Trace de fallos
`os/qemu/src/os_trace_fault.c`:
- Construye string extra `type=<DATA_ABORT|PREFETCH_ABORT>` y llama `os_trace_mode_switch`.

### 5h. Archivos auxiliares (sin cambios funcionales)
- `os_uart_putc.c` — MMIO directo para UART (kernel-side).
- `os_uart_puts.c` — wrapper de putc para cadenas.
- `os_wdt_disable.c` — deshabilita watchdog.
- `os_pcb_system_init.c` — inicializa PCBs con direcciones QEMU (0x00020000, 0x00030000) y SPSR=0x10.

## 6. cpu_get_user_regs / cpu_set_user_regs — Inline asm
`os/cpu/cpu_get_user_regs.c` y `os/cpu/cpu_set_user_regs.c`:
- Cambiaron de llamar `cpu_read_cpsr()`/`cpu_write_cpsr_c()` a usar inline asm con `mrs/msr` directo (más eficiente, evita dependencias).

## 7. pcb.h — Nuevo estado PROC_TERMINATED
`os/pcb.h`:
- Añadido `PROC_TERMINATED = 2` al enum `proc_state_t`.

## 8. timer.c — Timer más lento
`os/qemu/timer.c`:
- `TIMER_RELOAD` cambiado de `10000` a `12000000` (~500ms a 24MHz).

## 9. os_qemu.h — Firma actualizada
`os/qemu/os_qemu.h`:
- `os_trace_mode_switch` ahora: `(pid, direction, reason, extra)`.
- Añadido prototipo `os_handle_sys_yield`.

## 10. Makefile — Nuevo objeto
`qemu/Makefile`:
- Añadido `os_handle_sys_yield.o` a `OS_OBJS`.

## Incisos del documento 002Execution_modes_and_Syscalls

| Inciso | Descripción | Archivos |
|--------|-------------|----------|
| 3.1 | Modo usuario en procesos | `scheduler/round_robin.c` |
| 3.2 | SVC handler en root.s | `root.s` |
| 3.3 | Syscall interface (asm) | `lib/qemu/syscall.s` |
| 3.4 | User mode launch + sin enable_irq manual | `src/os_kmain.c`, `cpu/cpu_switch_to_user_mode.c` |
| 3.5 | Timer IRQ handler con traces | `src/os_timer_irq_handler.c` |
| 3.6 | Fault handler con recovery | `src/os_fault_handler.c`, `cpu/cpu_read_dfsr.c` |
| 3.7 | Syscall dispatcher + SYS_YIELD | `src/os_syscall_dispatcher.c`, `src/os_handle_sys_yield.c` |
| 3.8 | Traces unificados de modo | `src/os_trace_mode_switch.c`, `src/os_trace_fault.c` |
| 3.9 | PCB con PROC_TERMINATED | `pcb.h`, `pcb.c` |

## 11. Implementación por inciso

### Inciso 3.3 — Interfaz de syscalls en ASM
- Archivos: `lib/qemu/syscall.s`, `lib/user_syscalls.h`
- Lógica: el usuario no toca UART directo; todos los servicios pasan por `svc #0` con `r0`=ID. Esto obliga al paso por el kernel y protege la entrada de datos.

### Inciso 3.4 — Lanzamiento a modo usuario sin `enable_irq()` manual
- Archivos: `os/qemu/src/os_kmain.c`, `os/cpu/cpu_switch_to_user_mode.c`, `os/root.s`
- Lógica: el kernel prepara el contexto inicial con `SPSR=0x10` y salta al proceso en modo usuario. Las IRQ se habilitan implícitamente al entrar a USR.

### Inciso 3.5 — Manejador de timer con traces
- Archivos: `os/qemu/src/os_timer_irq_handler.c`, `os/root.s`, `os/qemu/timer.c`
- Lógica: la IRQ de timer salva el estado de usuario, selecciona el siguiente proceso runnable y lo restaura desde el PCB. Las trazas permiten ver preempción real.

### Inciso 3.6 — Manejador de fallos con recuperación
- Archivos: `os/qemu/src/os_fault_handler.c`, `os/qemu/src/os_trace_fault.c`, `os/cpu/cpu_read_dfsr.c`, `os/root.s`
- Lógica: al ocurrir un abort el kernel clasifica el fallo, termina el proceso culpable y intenta reanudar otro proceso ready.

### Inciso 3.7 — Despachador de syscalls y `SYS_YIELD`
- Archivos: `os/qemu/src/os_syscall_dispatcher.c`, `os/qemu/src/os_handle_sys_yield.c`, `os/pcb.c`, `os/scheduler/round_robin.c`
- Lógica: `SYS_YIELD` guarda el contexto, pone el proceso en `READY`, elige uno nuevo y restablece su contexto. `SYS_EXIT` termina el proceso actual.

### Inciso 3.8 — Trazas unificadas de modo
- Archivos: `os/qemu/src/os_trace_mode_switch.c`, `os/qemu/src/os_trace_fault.c`
- Lógica: todas las transiciones entre usuario y kernel y los fallos se imprimen con formato uniforme para auditoría.

### Inciso 3.9 — PCB con `PROC_TERMINATED`
- Archivos: `os/pcb.h`, `os/pcb.c`
- Lógica: el scheduler ignora procesos terminados y mantiene estados claros para evitar reanudar tareas muertas.

