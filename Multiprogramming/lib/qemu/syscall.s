.syntax unified
.text

.global __sys_write
__sys_write:
    push {r7, lr}
    mov r7, #1      @ SYS_WRITE
    svc #0
    pop {r7, pc}

.global __sys_yield
__sys_yield:
    push {r7, lr}
    mov r7, #2      @ SYS_YIELD
    svc #0
    pop {r7, pc}
