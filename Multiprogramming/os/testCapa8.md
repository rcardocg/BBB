# Examen de Diagnostico y Escenarios: Capa 8

Este examen evalua el conocimiento tecnico sobre la arquitectura del sistema, la gestion de procesos y la interaccion con el hardware AM335x (BeagleBone Black).

## Parte 1: Arquitectura y Ubicacion

1. **Abstraccion del Hardware**: Si necesitas agregar soporte para un nuevo periferico (por ejemplo, un LED en el GPIO1), ¿en que directorio deberias crear los archivos y que archivo de cabecera deberias incluir para realizar accesos a memoria?
2. **Logica de Planificacion**: El sistema actual usa Round Robin. ¿Que archivos deberias modificar o reemplazar para implementar un planificador basado en prioridades sin afectar los controladores de hardware?
3. **Punto de Entrada**: ¿Cual es el primer archivo que se ejecuta al arrancar el kernel y cual es su funcion principal antes de saltar a `kmain`?
4. **Definicion de Memoria**: ¿En que archivo se define la organizacion de las secciones `.text`, `.data` y `.bss`, y que pasaria si las direcciones de carga chocan con las direcciones de los procesos P1 o P2?

## Parte 2: Teoria de Multiprogramacion

5. **El Marco de Interrupcion (IRQ Frame)**: En `os.c`, la funcion `timer_irq_handler` recibe un `uint32_t *irq_frame`. ¿Que registros contiene exactamente este puntero y quien los puso ahi?
6. **Cambio de Contexto**: ¿Por que es necesario cambiar temporalmente a modo SVC (Supervisor) dentro del manejador de interrupciones para salvar el estado de un proceso?
7. **Estado de los Procesos**: Describe la diferencia entre `PROC_READY` y `PROC_RUNNING` en el contexto de la funcion `scheduler_next()`.

## Parte 3: Escenarios "Que pasa si..."

8. **QEMU vs BeagleBone Black**: Si intentas ejecutar este binario en QEMU usando una maquina generica (como `-M virt`) en lugar de emular especificamente la arquitectura AM335x, el sistema fallara inmediatamente. ¿Cual es la razon tecnica principal relacionada con los registros MMIO?
9. **Modificacion de Hexadecimales**: En `os/drivers/timer.c`, el valor `TIMER2_RELOAD` esta configurado para disparar cada 100ms aproximadamente. ¿Que sucederia si cambias ese valor a `0xFFFFFFFE`?
10. **SPSR e Integridad**: En `scheduler/round_robin.c`, el valor `INITIAL_PROC_SPSR` es `0x13u`. ¿Que significa este valor y que pasaria si lo cambias a `0x10u` (User mode) antes de que el proceso realice su primera llamada al sistema o interrupcion?
11. **Stack Overflow**: Si el proceso P1 excede el tamaño de stack definido en `0x82112000u`, ¿que componente del sistema (si es que existe alguno actualmente) detectaria el error?
12. **Corrupcion de Registro**: En `cpu.h`, si la funcion `write_svc_sp_lr` no tuviera el calificador `volatile` o las restricciones de memoria (`memory barrier`) en el ensamblador inline, ¿que tipo de optimizacion podria realizar el compilador que romperia el cambio de contexto?

## Parte 4: Desafio de Depuracion

13. **Escenario**: El sistema arranca, imprime `[OS] jumping to P1`, pero nunca ocurre la primera interrupcion del reloj (el sistema se queda "colgado" en P1). Enumera 3 posibles causas en orden de probabilidad (Hardware, INTC, CPU).
