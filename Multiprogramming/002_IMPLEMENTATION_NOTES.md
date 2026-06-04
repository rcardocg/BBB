# Phase 002 - Execution Modes and Syscalls

Este documento resume lo que se agrego sobre la base que ya existia de Phase 1 para cubrir `002Execution_modes_and_Syscalls.pdf`.

## Objetivo cubierto

La fase ahora separa kernel/user de forma explicita:

- El kernel arranca en modo privilegiado, configura stacks por modo y transfiere el primer proceso a USR.
- P1/P2 corren con SPSR inicial `0x10` (USR mode).
- Las salidas de usuario pasan por `svc #0`; user-space ya no debe escribir UART directamente.
- IRQ, SVC y faults usan frames compatibles para guardar/restaurar contexto.
- El scheduler evita reanudar procesos terminados.

## Cambios principales

### 1. Kernel -> User

Archivos:

- `os/root.s`
- `os/cpu.h`
- `os/cpu/cpu_switch_to_user_mode.c`
- `os/cpu/cpu_get_user_regs.c`
- `os/cpu/cpu_set_user_regs.c`

Se agrego/restauro el camino de entrada a USR usando `SPSR=0x10` y `movs pc, lr`, en vez de saltar como llamada normal de C. Tambien se inicializan stacks para IRQ, SVC, ABT y UND:

- QEMU: `__irq_stack_top`, `__svc_stack_top`, `__abt_stack_top`, `__und_stack_top`
- Beagle: equivalentes en `os/beagle/linker.ld`

Bug corregido: `cpu_get_user_regs()` podia corromper `LR_user` porque el compilador asignaba un output al registro `lr` mientras el CPU estaba temporalmente en System mode. Ahora usa temporales explicitos (`r2`, `r3`, `ip`) y escribe directo a memoria.

### 2. Syscall ABI

Archivos:

- `lib/syscall.s`
- `lib/user_syscalls.h`
- `os/qemu/src/os_syscall_dispatcher.c`
- `os/beagle/os.c`

ABI implementado segun el PDF:

- `r0`: syscall ID al entrar, return value al salir.
- `r1-r3`: argumentos.
- `svc #0`: trap canonico.

IDs:

- `SYS_YIELD = 0`
- `SYS_EXIT = 1`
- `SYS_WRITE = 2`

Errores:

- `-1`: syscall desconocida.
- `-2`: descriptor o argumento invalido.
- `-3`: puntero user invalido.

### 3. SYS_WRITE

Archivos:

- `lib/qemu/print.c`
- `lib/beagle/print.c`
- `os/qemu/src/os_syscall_dispatcher.c`
- `os/beagle/os.c`

`PRINT()` ahora arma un buffer y llama `SYS_WRITE`; ya no hace MMIO desde user-space. El kernel valida:

- `fd == 1`
- `len <= 256`
- `buf` dentro del rango user del proceso actual

Rangos usados:

- QEMU P1: `0x00020000..0x00030000`
- QEMU P2: `0x00030000..0x00040000`
- Beagle P1: `0x82100000..0x82112000`
- Beagle P2: `0x82200000..0x82212000`

### 4. SYS_YIELD

Archivos:

- `os/qemu/src/os_handle_sys_yield.c`
- `os/beagle/os.c`
- `os/scheduler/round_robin.c`

`yield` guarda el contexto USR actual, marca el proceso como listo si no esta terminado, selecciona el siguiente runnable y restaura su contexto. El retorno puede ser hacia otro proceso, como pide el PDF.

### 5. SYS_EXIT

Archivos:

- `lib/syscall.s`
- `lib/user_syscalls.h`
- `os/qemu/src/os_syscall_dispatcher.c`
- `os/beagle/os.c`
- `os/scheduler/round_robin.c`

`exit` marca el proceso como `PROC_TERMINATED`, lo saca del scheduling y despacha otro runnable. Si no queda ninguno, el kernel entra a una politica de idle/halt documentada con mensaje serial.

### 6. Fault handling

Archivos:

- `os/root.s`
- `os/qemu/src/os_fault_handler.c`
- `os/beagle/os.c`
- `os/qemu/linker.ld`
- `os/beagle/linker.ld`

Se conectaron:

- Undefined instruction
- Prefetch abort
- Data abort

Todos entran por `common_abort`, guardan frame, llaman `os_fault_handler()` y terminan/quarantinan el proceso actual. Si existe otro runnable, se restaura y se vuelve a USR con `fault_recovery`.

### 7. PCB y scheduler

Archivos:

- `os/pcb.h`
- `os/pcb.c`
- `os/scheduler/scheduler.h`
- `os/scheduler/round_robin.c`

El PCB conserva:

- `pid`
- `sp`
- `pc`
- `lr`
- `spsr`
- `r0-r12`
- `state`

Estados:

- `PROC_READY`
- `PROC_RUNNING`
- `PROC_TERMINATED`

El scheduler ahora salta procesos terminados y expone helpers para terminar el actual y saber si queda alguno runnable.

## Trazas

QEMU emite las trazas requeridas:

- `MODE_SWITCH KERNEL_TO_USER pid=<n> reason=initial_launch`
- `MODE_SWITCH USER_TO_KERNEL pid=<n> reason=syscall id=<id>`
- `MODE_SWITCH KERNEL_TO_USER pid=<n> reason=syscall_return id=<id> rc=<rc>`
- `MODE_SWITCH USER_TO_KERNEL pid=<n> reason=timer_irq`
- `MODE_SWITCH KERNEL_TO_USER pid=<n> reason=dispatch`
- `MODE_SWITCH USER_TO_KERNEL pid=<n> reason=fault type=<type>`
- `MODE_SWITCH KERNEL_TO_USER pid=<n> reason=fault_recovery`

Beagle tiene los paths funcionales equivalentes; las trazas son mas minimas para no saturar UART en placa.

## Verificacion realizada

Comandos ejecutados:

```sh
make -C Multiprogramming/qemu clean
make -C Multiprogramming/Beagle clean
make -C Multiprogramming/qemu
make -C Multiprogramming/Beagle
make -C Multiprogramming
timeout 6s make -C Multiprogramming runqemu
```

Resultado observado en QEMU:

```text
MODE_SWITCH KERNEL_TO_USER pid=1 reason=initial_launch
MODE_SWITCH USER_TO_KERNEL pid=1 reason=syscall id=2
P1:0
MODE_SWITCH KERNEL_TO_USER pid=1 reason=syscall_return id=2 rc=6
MODE_SWITCH USER_TO_KERNEL pid=1 reason=syscall id=0
MODE_SWITCH KERNEL_TO_USER pid=2 reason=syscall_return id=0 rc=0
MODE_SWITCH USER_TO_KERNEL pid=2 reason=syscall id=2
P2:a
...
P1:9
P2:z
P1:0
P2:a
```

La prueba valida que:

- P1/P2 corren en USR.
- `PRINT` usa `SYS_WRITE`.
- `yield` alterna procesos.
- El contexto de usuario se conserva en ciclos largos.
- QEMU ya no se reinicia por corrupcion de `LR_user`.
- Beagle compila con la misma separacion de user/kernel y syscalls.

## Notas de alcance

- No se habilito MMU/MPU; la proteccion de `SYS_WRITE` se implementa por validacion explicita de rangos user antes de dereferenciar buffers.
- La politica de “sin procesos runnable” es halt/idle con mensaje serial.
- El handler de fault termina el proceso culpable y recupera otro runnable si existe.
