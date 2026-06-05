// Declara el modulo P1; afecta prototipos user-space; no tiene dependencias externas.
#ifndef P1_H
#define P1_H

#include <stdint.h>

#define P1_FORCE_FAULT 0

void p1_delay(void);
void p1_fault_demo(uint32_t n);
void p1_run(void);

#endif
