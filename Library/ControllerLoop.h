#ifndef CONTROLLERLOOP_H
#define CONTROLLERLOOP_H

#include "Timer.h"
#include "SystemStructures.h"

typedef void (*Controller_Callback)();

void Controller_Loop_Init(Controller_Callback cb);

#endif
