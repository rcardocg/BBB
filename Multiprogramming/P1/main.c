#include <stdint.h>

#include "../lib/print.h"
#include "../lib/user_syscalls.h"

int main(void) {
    uint32_t n = 0;

    for (;;) {
        PRINT("P1:%d\n", (int)n);
        n = (n + 1u) % 10u;
        sys_yield();
    }
}
