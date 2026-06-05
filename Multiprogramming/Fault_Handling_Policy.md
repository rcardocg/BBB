# Fault-Handling Policy Document

Este documento establece la política de clasificación y contención de fallos (Fault Containment) implementada en el sistema operativo, garantizando que el kernel y los procesos sanos continúen su ejecución de manera ininterrumpida frente a comportamientos anómalos de tareas de usuario individuales.

## 1. Arquitectura de Aislamiento

El sistema operativo maneja las excepciones provenientes de tareas de usuario asegurando una completa separación de privilegios:
*   **Modo Usuario (USR):** Los procesos de usuario (`P1` y `P2`) corren exclusivamente en este modo (CPSR `0x10`). No tienen acceso a instrucciones privilegiadas ni al hardware de manera directa.
*   **Interceptación Centralizada:** Toda excepción (Instrucción Indefinida, Aborto de Datos, Aborto de Prefetch) transiciona automáticamente al Kernel. La rutina ensambladora (`os/root.s`) guarda el contexto completo del proceso infractor en la pila correspondiente y delega el análisis a `os_fault_handler()`.

## 2. Clasificación de Fallos

Dado que en esta fase **no se encuentra habilitada la Unidad de Manejo de Memoria (MMU/MPU)**, los errores de traducción de páginas o permisos de MMU no se generan de forma nativa por hardware. Por lo tanto, el sistema captura e identifica los fallos basándose en las excepciones arquitectónicas del procesador ARM Cortex-A8:

| Fuente del Fallo | Identificador (fault_type) | Causa Típica en Modo Usuario |
| :--- | :---: | :--- |
| **Undefined Instruction** | `0` | El proceso intentó ejecutar un código de operación (OpCode) que no pertenece al set de instrucciones de ARM, o intentó ejecutar una instrucción privilegiada desde el modo USR. |
| **Prefetch Abort** | `1` | El procesador intentó realizar un *fetch* de instrucción desde una dirección de memoria inválida o no respaldada por el bus físico. |
| **Data Abort** | `2` | El procesador intentó leer/escribir datos en una dirección inválida (ej. *Alignment Fault* si la alineación de memoria es incorrecta y SCTLR.A está activo, o un External Abort en el bus). |

### Decodificador FSR (Preparación Futura)
Aunque la MMU esté deshabilitada, el Kernel cuenta con el decodificador `decode_fsr()` preparado para analizar el *Fault Status Register* (DFSR/IFSR) identificando `alignment`, `translation_section`, `translation_page`, `permission_section` y `permission_page` para fases posteriores. En esta fase, los errores comunes sin MMU recaen en la categoría `type=undefined` u ocurren silenciosamente si el bus físico no levanta una excepción.

## 3. Matriz de Clasificación -> Resultados (Outcome Matrix)

La política estricta del Kernel ante **cualquier fallo** provocado por un proceso de usuario es la cuarentena y eliminación permanente del proceso culpable, salvaguardando la integridad del Kernel.

| Clasificación de Fallo Detectado | Decisión del Kernel | Impacto en el Proceso Culpable | Impacto en Procesos Sanos (Peers) | Impacto en el Sistema |
| :--- | :--- | :--- | :--- | :--- |
| **Undefined Instruction** | **Terminación Inmediata** | Es sacado de ejecución y su PCB se marca como `PROC_TERMINATED`. Nunca vuelve a ser planificado. | Ninguno. Su ejecución continúa con normalidad tras la recuperación. | El Kernel realiza un Context Switch seguro (`fault_recovery`) y continúa vivo. |
| **Data / Prefetch Abort** | **Terminación Inmediata** | Mismo resultado (`PROC_TERMINATED`). | Ninguno. | Mismo resultado. |
| **Violación de Memoria por Syscall** | **Rechazo (Error `-3`)** | Recibe un código de retorno `-3` (No es terminado forzosamente, pero la escritura es bloqueada). | Ninguno. | El Kernel rechaza el intento y evita la dereferenciación de punteros inválidos. |

## 4. Recuperación de Contexto (Fault Recovery)

La recuperación de fallos es totalmente transparente para el resto del sistema:
1. El manejador registra la falla con la traza: `MODE_SWITCH USER_TO_KERNEL pid=<n> reason=fault type=<type>`.
2. Se destruye el PCB del proceso infractor.
3. El planificador Round Robin selecciona al siguiente proceso en estado `PROC_READY`.
4. El Kernel emite la traza: `MODE_SWITCH KERNEL_TO_USER pid=<n> reason=fault_recovery`.
5. **Restauración Segura:** Se restauran los 13 registros generales (`r0-r12`), `LR` y `SPSR` del nuevo proceso utilizando `pcb_restore_to_irq_frame`, evitando cualquier tipo de corrupción (fuga de registros) entre el proceso muerto y el proceso sano.
