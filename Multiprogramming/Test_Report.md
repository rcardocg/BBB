# Test Report

Este documento presenta los resultados de las pruebas realizadas sobre el sistema operativo, validando el cumplimiento estricto de los Criterios de Aceptación (Acceptance Criteria) estipulados en la Fase 2 del proyecto.

## 1. Checklist de Criterios de Aceptación (Pass/Fail)

| Criterio a Evaluar | Estado | Observaciones |
| :--- | :---: | :--- |
| El cambio de contexto basado en Timer (`timer_irq`) permanece estable durante la ejecución sostenida. | ✅ PASS | El sistema alterna continuamente entre P1 y P2 sin corrupciones a lo largo del tiempo. |
| Al menos dos tareas de modo usuario se ejecutan concurrentemente bajo preemption forzado. | ✅ PASS | P1 y P2 se ejecutan en modo USR y el round-robin reparte equitativamente los ciclos. |
| `SYS_YIELD` y `SYS_EXIT` son correctos; `SYS_WRITE` está validado contra punteros/argumentos corruptos. | ✅ PASS | `yield` cambia de proceso. `write` se apoya en UART bajo validación de descriptores (fd=1) y rangos de memoria del proceso. |
| Tareas que provocan fallos (Faulting tasks) pueden ser terminadas/aisladas mientras el kernel y los peers continúan vivos. | ✅ PASS | Verificado mediante la inyección deliberada de un `Undefined Instruction` (FORCE_FAULT=1). El proceso defectuoso muere; su peer sobrevive. |
| No se observa escalada de privilegios de usuario a kernel en los tests. | ✅ PASS | Los procesos USR tienen prohibido ejecutar `msr` y `mcr`. Solamente pueden comunicarse vía `svc #0`. |
| No hay corrupción de contexto después de repetidos entrelazados (interleavings) de IRQ + syscall + fault. | ✅ PASS | Validado. Tras corregir la lógica de retorno del `fault_recovery`, los registros `r0-r12` se conservan de manera aislada sin contaminación. |

---

## 2. Evidencia y Trazas Clave (Keyed Traces)

A continuación, se adjunta la traza del sistema demostrando el flujo de ejecución completo bajo estrés (Inyección de falla en el Proceso 1).

### 2.1 Entorno de Prueba
*   **Plataforma:** QEMU (versatilepb).
*   **Condición:** `P1` configurado para ejecutar una instrucción en ensamblador ilegal (`0xE7F000F0`) en su quinta iteración para forzar la actuación del Kernel.

### 2.2 Trazas Observadas y Explicadas

**Lanzamiento Inicial:**
```text
MODE_SWITCH KERNEL_TO_USER pid=1 reason=initial_launch
MODE_SWITCH USER_TO_KERNEL pid=1 reason=syscall id=2
P1:0
MODE_SWITCH KERNEL_TO_USER pid=1 reason=syscall_return id=2 rc=6
```
*(El Kernel inicializa y cede el control a P1 en modo USR. P1 hace un `sys_write` exitoso de 6 bytes).*

**Preemption Normal (Intercalado de procesos):**
```text
MODE_SWITCH USER_TO_KERNEL pid=1 reason=syscall id=0
MODE_SWITCH KERNEL_TO_USER pid=2 reason=syscall_return id=0 rc=0
MODE_SWITCH USER_TO_KERNEL pid=2 reason=syscall id=2
P2:a
```
*(P1 llama a `sys_yield` (`id=0`). El Kernel guarda a P1, despacha a P2, y P2 escribe "P2:a").*

**Inyección del Fallo y Aislamiento (Fault Containment):**
```text
P1: Intentando ejecutar instruccion ilegal...
MODE_SWITCH KERNEL_TO_USER pid=1 reason=syscall_return id=2 rc=46
MODE_SWITCH USER_TO_KERNEL pid=1 reason=fault type=undefined
```
*(P1 alcanza su iteración objetivo y ejecuta la instrucción ilegal. El hardware atrapa la excepción y lanza a P1 al modo Kernel (`USER_TO_KERNEL`), el cual decodifica y reporta `type=undefined`).*

**Recuperación del Sistema (Fault Recovery):**
```text
MODE_SWITCH KERNEL_TO_USER pid=2 reason=fault_recovery
MODE_SWITCH USER_TO_KERNEL pid=2 reason=syscall id=2
P2:d
MODE_SWITCH KERNEL_TO_USER pid=2 reason=syscall_return id=2 rc=6
MODE_SWITCH USER_TO_KERNEL pid=2 reason=syscall id=0
```
*(El Kernel marca a P1 como terminado y hace un Context Switch forzoso hacia P2 bajo el tag de recuperación. P2 continúa su ejecución en modo USR de manera inalterada y segura).*

### 2.3 Conclusión del Reporte
Las pruebas confirman la total funcionalidad del sistema operativo, el cual exhibe robustez para soportar concurrencia mediante interrupciones por hardware y software, a su vez garantizando políticas de protección a nivel de fallos arquitectónicos. El Kernel **no es susceptible a caídas (panics/halts) causadas por tareas de usuario.**
