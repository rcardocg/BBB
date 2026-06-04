# Mapa del Arquitectura (Fase 2)

Este documento describe la arquitectura modular del sistema operativo tras la implementación de la Fase 2 (Aislamiento Kernel/Usuario).

## 1. Capas del Sistema

### Capa de Abstracción de CPU (`os/cpu.h`)
Responsable de todas las operaciones que dependen directamente de los modos de ejecución del ARMv7-A.
*   `cpu_get_user_regs()` / `cpu_set_user_regs()`: Permiten al Kernel manipular los registros `sp` y `lr` del modo Usuario utilizando el modo **System (0x1F)**.
*   `cpu_switch_to_user_mode()`: Realiza la transición protegida desde el Kernel (Supervisor) al primer proceso de Usuario (User).

### Capa de Gestión de Procesos (`os/qemu/os.c` y `os/pcb.c`)
Orquestador del multitasking y los servicios del sistema.
*   **Syscall Dispatcher**: Punto de entrada único para solicitudes del usuario (`svc #0`).
    *   `SYS_WRITE` (R7=1): Servicio de impresión segura.
    *   `SYS_YIELD` (R7=2): Cede voluntariamente el CPU.
*   **Fault Handler**: Captura y reporta errores de ejecución (Abortos) para evitar el colapso del sistema.

### Capa de Usuario (`P1/`, `P2/`, `lib/qemu/`)
Código que se ejecuta sin privilegios.
*   **Aislamiento**: No tiene acceso a MMIO ni a instrucciones privilegiadas.
*   **Interrupción de Sistema**: Usa `lib/qemu/syscall.s` para solicitar servicios al Kernel.

---

## 2. Flujo de una Syscall (Ejemplo: PRINT)
1.  **Usuario**: Llama a `PRINT()`.
2.  **Librería**: `uart_putc()` llama a `__sys_write()`.
3.  **Trampa**: `svc #0` (con R7=1) causa una excepción de CPU.
4.  **Vector**: El hardware salta a `svc_handler` en `root.s`.
5.  **Kernel**: `syscall_dispatcher()` verifica R7 y escribe en la UART real.
6.  **Retorno**: El Kernel restaura el estado y vuelve al usuario en modo no privilegiado.

---

## 3. Organización de Archivos
*   `os/common/`: Lógica compartida (PCB, Scheduler).
*   `os/qemu/`: Implementación específica para la plataforma virtual.
*   `os/beagle/`: Implementación específica para hardware real (BBB).
