#ifndef USER_SYSCALLS_H
#define USER_SYSCALLS_H

#include <stdint.h>

enum {
    SYS_YIELD = 0,
    SYS_EXIT = 1,
    SYS_WRITE = 2
};

int32_t __sys_write(int32_t fd, const void *buf, uint32_t len);
int32_t __sys_yield(void);
void __sys_exit(int32_t code);

static inline int32_t sys_yield(void) {
    return __sys_yield();
}

static inline void sys_exit(int32_t code) {
    __sys_exit(code);
    for (;;) {
    }
}

static inline int32_t sys_write(int32_t fd, const void *buf, uint32_t len) {
    return __sys_write(fd, buf, len);
}

#endif
