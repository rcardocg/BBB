.syntax unified
.text

.global __sys_write
__sys_write:
    @ int32_t __sys_write(int32_t fd, const void *buf, uint32_t len)
    mov r3, r2
    mov r2, r1
    mov r1, r0
    mov r0, #2      @ SYS_WRITE
    svc #0
    bx lr

.global __sys_yield
__sys_yield:
    @ int32_t __sys_yield(void)
    mov r0, #0      @ SYS_YIELD
    mov r1, #0
    mov r2, #0
    mov r3, #0
    svc #0
    bx lr

.global __sys_exit
__sys_exit:
    @ void __sys_exit(int32_t code)
    mov r1, r0
    mov r0, #1      @ SYS_EXIT
    mov r2, #0
    mov r3, #0
    svc #0
1:
    b 1b
