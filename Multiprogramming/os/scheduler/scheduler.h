#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../pcb.h"

void scheduler_init(void);
pcb_t* scheduler_get_current_pcb(void);
void scheduler_next(void);

#endif
