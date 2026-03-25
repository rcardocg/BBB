# Multiprogramming (BeagleBone Black / AM335x)

Proyecto bare-metal que implementa multiprogramacion (multitarea) en un AM335x (ARM Cortex-A8) usando DMTimer2 para interrupciones periodicas y un scheduler Round-Robin que alterna entre 2 procesos de usuario.

## Estructura

- `root.s`: vector table, VBAR, stacks (SVC/IRQ), handler IRQ, y salto inicial a procesos.
- `linker.ld`: linker script del OS (base `0x82000000`).
- `os.c`: init de WDT, UART0, DMTimer2, INTC; PCBs; scheduler en `timer_irq_handler`.
- `pcb.h`: estructura PCB (regs + sp/lr/pc/spsr).
- `lib/print.c`, `lib/print.h`: PRINT minimo para UART0 (usado por P1/P2).
- `P1/`: proceso 1 (imprime `0..9`).
- `P2/`: proceso 2 (imprime `a..z`).
- `Loads.txt`: direcciones y comandos U-Boot.
- `PLAN_IMPLEMENTACION.md`: plan por fases (lo que se implemento y donde).
- `FALTANTES_Y_MEJORAS.md`: limites actuales y mejoras sugeridas.

## Mapa de memoria (fijo)

- OS: `0x82000000` (64 KB)
- OS stacks: `0x82010000`..`0x82011FFF` (IRQ/SVC)
- P1 code/data: `0x82100000` (64 KB)
- P1 stack: `0x82110000`..`0x82111FFF` (8 KB)
- P2 code/data: `0x82200000` (64 KB)
- P2 stack: `0x82210000`..`0x82211FFF` (8 KB)

## Compilar

Requiere toolchain `arm-none-eabi-*`.

```sh
cd /home/toto/toto/CODE/proyecto_Lester
make clean && make
```

Outputs:
- `os.bin`
- `P1/p1.bin`
- `P2/p2.bin`

## Cargar y ejecutar (U-Boot)

Ver `Loads.txt`. Ejemplo usando `loady` (YMODEM):

```text
loady 0x82000000   # enviar os.bin
loady 0x82100000   # enviar P1/p1.bin
loady 0x82200000   # enviar P2/p2.bin
go 0x82000000
```

## Salida esperada

En la consola serial deberias ver salida intercalada con el tiempo:

```text
----From P1: 0
----From P2: a
----From P1: 1
----From P2: b
...
```

## Notas importantes

- El watchdog (WDT1) se desactiva al inicio del OS.
- El cambio de contexto se hace en la IRQ:
  - se guardan `r0-r12`, `SP_svc`, `LR_svc`, `PC` (derivado de `LR_irq-4`) y `SPSR`.
  - se restaura el PCB del siguiente proceso y se retorna con `subs pc, lr, #4`.

## Problemas comunes

- No hay salida serial: revisar UART0/baud (normalmente 115200 8N1) y que estes en el puerto correcto.
- Se reinicia a ~60s: watchdog no se desactivo (revisar `disable_wdt1()` en `os.c`).
- No alterna P1/P2: revisar que DMTimer2 este generando IRQ y que IRQ 68 este unmasked en INTC.
