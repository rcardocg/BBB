# Modulos por inciso - 002 Execution Modes and Syscalls

Este mapa organiza la implementacion segun los incisos del PDF y las reglas de `rules.md`.

## Reglas aplicadas

- `main.c` de P1/P2 solo llama a una funcion externa (`p1_run`, `p2_run`).
- Las funciones de usuario quedaron separadas en archivos atomicos.
- Los headers `p1.h`, `p2.h`, `user_syscalls.h`, `scheduler.h` centralizan prototipos por modulo.
- Los nombres nuevos usan prefijo de modulo: `p1_`, `p2_`, `os_`, `cpu_`, `pcb_`, `scheduler_`.

## Inciso 2 - Baseline Phase 1

Responsabilidad: mantener multitarea, IRQ de timer, PCB y Round-Robin.

Archivos:

- `os/root.s`: vector table, IRQ entry, SVC entry, abort entry.
- `os/pcb.c`, `os/pcb.h`: save/restore de contexto en PCB.
- `os/scheduler/round_robin.c`, `os/scheduler/scheduler.h`: Round-Robin y estados.
- `os/qemu/timer.c`, `os/qemu/intc.c`: timer/interrupt controller para QEMU.
- `os/beagle/drivers/timer.c`: DMTimer2/INTC para Beagle.

## Inciso 3 - Kernel and User Mode

### 3.4 Initial boot transition

Responsabilidad: entrar al primer proceso en USR mode con contexto construido.

Archivos:

- `os/qemu/src/os_kmain.c`: orquesta arranque QEMU y llama `cpu_switch_to_user_mode`.
- `os/beagle/os.c`: orquesta arranque Beagle y llama `cpu_switch_to_user_mode`.
- `os/cpu/cpu_switch_to_user_mode.c`: usa `SPSR=0x10` y `movs pc, lr`.
- `os/qemu/src/os_pcb_system_init.c`: crea PCB inicial QEMU con SP/PC/SPSR.
- `os/scheduler/round_robin.c`: crea PCB inicial Beagle.

Trace esperado:

```text
MODE_SWITCH KERNEL_TO_USER pid=1 reason=initial_launch
```

### 3.5 Interrupt path

Responsabilidad: timer IRQ preempta user-space y despacha otra tarea.

Archivos:

- `os/root.s`: `irq_handler`.
- `os/qemu/src/os_timer_irq_handler.c`: IRQ path QEMU.
- `os/beagle/os.c`: `os_timer_irq_handler` para Beagle.
- `os/cpu/cpu_get_user_regs.c`: lee SP/LR de USR usando System mode.
- `os/cpu/cpu_set_user_regs.c`: restaura SP/LR de USR.

Traces esperados:

```text
MODE_SWITCH USER_TO_KERNEL pid=<n> reason=timer_irq
MODE_SWITCH KERNEL_TO_USER pid=<m> reason=dispatch
```

### 3.6 Exception path

Responsabilidad: contener abort/undefined/fault de user-space.

Archivos:

- `os/root.s`: `undefined_handler`, `prefetch_handler`, `abort_handler`, `common_abort`.
- `os/qemu/src/os_fault_handler.c`: termina proceso fallido y recupera otro.
- `os/qemu/src/os_trace_fault.c`: trace de fault QEMU.
- `os/beagle/os.c`: `os_fault_handler` para Beagle.

Traces esperados:

```text
MODE_SWITCH USER_TO_KERNEL pid=<n> reason=fault type=<type>
MODE_SWITCH KERNEL_TO_USER pid=<m> reason=fault_recovery
```

### 3.7 Syscall path

Responsabilidad: user-space entra al kernel via `svc #0`.

Archivos:

- `os/root.s`: `svc_handler`.
- `lib/syscall.s`: stubs `__sys_yield`, `__sys_exit`, `__sys_write`.
- `lib/user_syscalls.h`: wrappers `sys_yield`, `sys_exit`, `sys_write`.
- `os/qemu/src/os_syscall_dispatcher.c`: dispatcher QEMU.
- `os/beagle/os.c`: dispatcher Beagle.

Traces esperados:

```text
MODE_SWITCH USER_TO_KERNEL pid=<n> reason=syscall id=<id>
MODE_SWITCH KERNEL_TO_USER pid=<m> reason=syscall_return id=<id> rc=<rc>
```

## Inciso 4 - System Calls

ABI:

- `r0`: syscall ID al entrar, retorno al salir.
- `r1-r3`: argumentos.
- `svc #0`: entrada al kernel.

IDs:

- `SYS_YIELD = 0`
- `SYS_EXIT = 1`
- `SYS_WRITE = 2`

Archivos:

- `lib/syscall.s`
- `lib/user_syscalls.h`
- `os/qemu/src/os_syscall_dispatcher.c`
- `os/qemu/src/os_handle_sys_yield.c`
- `os/beagle/os.c`

Errores:

- `-1`: syscall desconocida.
- `-2`: descriptor/argumento invalido.
- `-3`: puntero user invalido.

## Inciso 5 - Faults, Memory Protection and Isolation

Responsabilidad: usuario que falla no tumba el kernel.

Archivos:

- `os/root.s`
- `os/qemu/src/os_fault_handler.c`
- `os/qemu/src/os_trace_fault.c`
- `os/beagle/os.c`

Politica:

- Proceso culpable pasa a `PROC_TERMINATED`.
- Scheduler busca otro runnable.
- Si no hay runnable, kernel entra a halt/idle documentado.

## Inciso 6 - PCB and Unified Context

Responsabilidad: un solo contrato de contexto para IRQ, SVC y faults.

Archivos:

- `os/pcb.h`: estructura `pcb_t`.
- `os/pcb.c`: inicializacion, save/restore.
- `os/root.s`: frame comun `r0-r12`, `lr_exception`, `spsr_exception`.

PCB contiene:

- `pid`
- `sp`
- `pc`
- `lr`
- `spsr`
- `r[0..12]`
- `state`

## User programs

P1:

- `P1/main.c`: solo llama `p1_run`.
- `P1/p1_run.c`: loop de P1.
- `P1/p1_delay.c`: delay user-space.
- `P1/p1_fault_demo.c`: demo opcional de fault.
- `P1/p1.h`: prototipos del modulo P1.

P2:

- `P2/main.c`: solo llama `p2_run`.
- `P2/p2_run.c`: loop de P2.
- `P2/p2_delay.c`: delay user-space.
- `P2/p2.h`: prototipos del modulo P2.

## Ejecucion

Compilar todo:

```sh
make -C Multiprogramming
```

QEMU:

```sh
make -C Multiprogramming qemurun
```

Salir de QEMU:

```text
Ctrl+A, luego X
```

Beagle:

```sh
make -C Multiprogramming beagle
```

Luego cargar:

```text
loady 0x82000000  # Beagle/os_beagle.bin
loady 0x82100000  # Beagle/p1_beagle.bin
loady 0x82200000  # Beagle/p2_beagle.bin
go 0x82000000
```
