# Syscall ABI Document

Este documento describe la Interfaz Binaria de Aplicación (ABI) para las llamadas al sistema (Syscalls) implementadas en el OS, cumpliendo con los requerimientos de la Fase 2 del proyecto.

## 1. Convención de Registros (ARM / SVC)

El sistema operativo utiliza la instrucción `svc #0` (Supervisor Call) para realizar transiciones del modo Usuario (USR) al modo Kernel (SVC). La convención de paso de parámetros es la siguiente:

*   **`r0` (Entrada):** Identificador de la Syscall (Syscall ID).
*   **`r1 - r3` (Entrada):** Argumentos de la Syscall. Si una syscall no utiliza todos los argumentos, estos se ignoran.
*   **`r0` (Salida):** Valor de retorno de la Syscall (código de éxito o error).

## 2. Tabla de Identificadores de Syscall (Syscall IDs)

| Símbolo | ID Numérico | Propósito |
| :--- | :---: | :--- |
| `SYS_YIELD` | `0` | Ceder voluntariamente el procesador (Reschedule). |
| `SYS_EXIT` | `1` | Terminar el proceso actual (No retorna). |
| `SYS_WRITE` | `2` | Escribir bytes desde un buffer de usuario (ej. a UART). |

## 3. Comportamiento y Códigos de Retorno

### Retornos Generales
*   `>= 0`: Éxito. En el caso de `SYS_WRITE`, representa la cantidad de bytes escritos.
*   `< 0`: Códigos de error (ver tabla de errores).

### Tabla de Errores
| Código de Error | Significado |
| :---: | :--- |
| `-1` | **Syscall ID Inválido:** El ID solicitado en `r0` no existe en la tabla de Syscalls. |
| `-2` | **Descriptor o Argumento Inválido:** (ej. `fd` distinto de 1, o longitud mayor a la permitida). |
| `-3` | **Violación de Memoria/Puntero:** El puntero de usuario apunta fuera del rango de memoria mapeado para el proceso. |

## 4. Detalles de Implementación de Syscalls

### 4.1 SYS_YIELD (ID 0)
*   **Descripción:** Guarda el contexto actual del proceso, cambia su estado a `PROC_READY` (si no ha sido terminado), e invoca al planificador (Round Robin) para ejecutar el siguiente proceso.
*   **Retorno:** Retorna `0` de manera determinista al proceso cuando este vuelve a ser calendarizado.

### 4.2 SYS_EXIT (ID 1)
*   **Descripción:** Marca el proceso actual como `PROC_TERMINATED`, lo elimina de la cola de procesos ejecutables y llama al planificador.
*   **Retorno:** No retorna al proceso que la llama. Si todos los procesos terminan, el kernel entra en estado de inactividad segura (Halt/Idle) con un mensaje por consola.

### 4.3 SYS_WRITE (ID 2)
*   **Descripción:** Escribe a la consola (UART). Valida que el descriptor de archivo (`fd`) sea 1 y que el buffer de memoria origen pertenezca estrictamente al rango de direcciones de memoria del proceso llamador.
*   **Comportamiento ante errores:** Si el `fd` es inválido o el tamaño es demasiado grande, retorna `-2`. Si el puntero de memoria sale de los límites del proceso (ej. `0x82100000` a `0x82112000` en BeagleBone para P1), retorna `-3` para evitar corrupción del kernel.

### 4.4 Syscalls Desconocidas
Si un proceso de usuario invoca `svc #0` con un ID en `r0` que no está en la tabla (ej. ID `5`), el Kernel:
1. Intercepta la llamada.
2. Registra el intento en consola (`[KERNEL] Unknown syscall`).
3. Retorna inmediatamente el código de error `-1` en `r0` al proceso usuario.
4. Mantiene la estabilidad del sistema sin colapsar.
