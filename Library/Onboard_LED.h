#ifndef ONBOARD_LED_H
#define ONBOARD_LED_H

#include "LPC407x_8x_177x_8x.h"
#include "GPIO.h"

void Onboard_LEDs_Init(void);

// 1: turn on, 0: turn off, other: no change
void Onboard_LEDs_Set_State(int one_on, int two_on, int three_on, int four_on);

#endif
