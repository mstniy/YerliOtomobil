#ifndef MOTORS_H
#define MOTORS_H

#include "PWM.h"
#include "GPIO.h"

void Motors_Init(void);
void Motors_Set_Scaled_Speed(int motor_index, double speed);

#endif
