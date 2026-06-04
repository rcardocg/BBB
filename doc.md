# Preparando la fase 2
## round_robin.c
Primero había que modificar el round robin ya que en la fase una estaba corriendo en modo supervisor. El bit en modo 0x10 indica User mode, esto es para que el cpu bloquee cualquier intenti de ejecutar instrucciones sensibles. 
## root.s
Paso dos fue modificar el root. Se actualizó la tabla de vectores y ahora hace el salto al vector de svc_handler que guarda los registros antes de pasarse al código de C, al volver se restaura el estado que pudo ser modificado por el kernel. También se implementaron los abortos que apunta a la instrucción fallida y se va al manejador de errores en C. Inicialmente se queda en un bucle infinito para hacer pruebas. También se implementó el prefetch_handler y el abort_handler. Por último en esta fase se guarda y restauran registros en el stach de supervisor.
##syscall_qemu.s (NUEVO):
       * Creada la interfaz de usuario con la instrucción svc #0 para las funciones __sys_write y __sys_yield.
##print_qemu.c:
       * Refactorizada la función uart_putc para que ya no acceda a memoria (MMIO), sino que use la syscall
         __sys_write.

##os_qemu.c
Se cambió a user mode.
Se implementí el syscall_dispatcher para manejar las llamadas SYS_WRITE.
