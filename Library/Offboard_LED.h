#ifndef OFFBOARD_LED_H
#define OFFBOARD_LED_H

#include "GPIO.h"

void Offboard_LEDs_Init(void);

// 1: turn on, 0: turn off, other: no change
void Offboard_LEDs_Set_State(int left_on, int right_on, int front_on, int back_on);
#endif
