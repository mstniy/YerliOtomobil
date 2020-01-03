#ifndef OFFBOARD_LED_H
#define OFFBOARD_LED_H

#include "GPIO.h"

void Offboard_LEDs_Init(void);

// 1: turn on, 0: turn off, other: no change
void Offboard_LEDs_Set_State(int fl_on, int fr_on, int bl_on, int br_on);

void Offboard_LEDs_Forward(void);
void Offboard_LEDs_Backward(void);
void Offboard_LEDs_Left(void);
void Offboard_LEDs_Right(void);

#endif
