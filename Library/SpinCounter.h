#ifndef SPIN_COUNTER_H
#define SPIN_COUNTER_H

#include <stdint.h>

void spin_counter_init(void);
uint32_t spin_counter_get_count(void);

#endif
