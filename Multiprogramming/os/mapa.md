# Mapa del Núcleo (os/os.c)

Este archivo actúa como el orquestador principal del sistema operativo. Tras la refactorización modular, su responsabilidad se limita a la inicialización del sistema y el manejo de alto nivel de las interrupciones de hardware.

## 1. Dependencias (Módulos)
El archivo `os.c` se conecta con los siguientes componentes:
*   `pcb.h`: Estructura de Control de Procesos para guardar/restaurar estados.
*   `cpu.h`: Funciones de bajo nivel para manipular registros del procesador ARM.
*   `drivers/uart.h`: Salida de texto por consola serial.
*   `drivers/wdt.h`: Control del Watchdog Timer.
*   `drivers/timer.h`: Configuración del DMTimer2 y el controlador de interrupciones (INTC).
*   `scheduler/scheduler.h`: Interfaz del planificador para decidir qué proceso sigue.

---

## 2. Variables y Tipos
*   **`entry_fn_t` (typedef)**: Puntero a función de tipo `void(void)`. Se utiliza para castear la dirección de memoria del proceso (PC) y ejecutarlo como una función de C.

---

## 3. Funciones Principales

### `timer_irq_handler(uint32_t *irq_frame)`
Es el corazón del **Multitasking Preemptivo**. Se ejecuta automáticamente cada vez que el temporizador genera una interrupción.
*   **Entrada**: `irq_frame`, un puntero al marco de registros (r0-r12, lr, spsr) guardados en el stack por `root.s`.
*   **Flujo de Conexión**:
    1.  **ACK**: Llama a `timer_ack_interrupt()` para limpiar la bandera de interrupción y permitir que ocurran futuras interrupciones.
    2.  **Contexto SVC**: Usa `read_svc_sp_lr()` para obtener los registros `sp` y `lr` del modo Supervisor (donde corre el proceso de usuario).
    3.  **Guardar**: Obtiene el proceso actual mediante `scheduler_get_current_pcb()` y guarda su estado completo usando `pcb_save_from_irq_frame()`.
    4.  **Estado**: Cambia el estado del proceso actual a `PROC_READY`.
    5.  **Decisión**: Llama a `scheduler_next()` para que el algoritmo (Round Robin) elija el siguiente proceso.
    6.  **Restaurar**: Obtiene el nuevo proceso, restaura sus registros en el `irq_frame` y actualiza los registros SVC reales con `write_svc_sp_lr()`.
    7.  **Ejecución**: Al terminar la función, `root.s` usará el `irq_frame` modificado para saltar al nuevo proceso.

### `kmain(void)`
Punto de entrada del Kernel después de que `root.s` inicializa el hardware básico.
*   **Inicialización**:
    *   `wdt_disable()`: Evita que la placa se reinicie sola.
    *   `scheduler_init()`: Configura la tabla de procesos (PCBs) y sus puntos de entrada (P1, P2).
    *   `timer_init()` y `intc_init()`: Activan el "latido" del sistema.
    *   `enable_irq()`: Habilita las interrupciones a nivel global en el CPU.
*   **Arranque del Primer Proceso**:
    *   Obtiene el PCB del primer proceso (P1) mediante el planificador.
    *   Configura su Stack Pointer (`sp`) y Link Register (`lr`) iniciales.
    *   Realiza un salto directo a su dirección de entrada (`pc`).

---

## 4. Flujo de Control
1.  **Arranque**: `root.s` -> `kmain()` -> P1 inicia ejecución.
2.  **Interrupción**: El Timer llega a cero -> El CPU salta a `root.s` (vector IRQ) -> `root.s` llama a `timer_irq_handler()`.
3.  **Cambio**: `timer_irq_handler()` decide que es turno de P2 -> Modifica el stack de retorno.
4.  **Retorno**: `root.s` restaura registros -> El CPU ahora está ejecutando P2 en lugar de P1.
