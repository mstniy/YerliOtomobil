#ifndef CONTROLLERLOOP_H
#define CONTROLLERLOOP_H

#include "Timer.h"
#include "SystemStructures.h"

#define CONTROLLER_LOOP_PERIOD_MS 50

typedef void (*Controller_Callback)();

void Controller_Loop_Init(Controller_Callback cb);
uint32_t get_ms(void);

#endif
