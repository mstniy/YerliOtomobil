#ifndef MOTORS_H
#define MOTORS_H

#include "PWM.h"
#include "GPIO.h"

void Motors_Init(void);
double Motors_Get_Last_Scaled_Speed(int motor_index);
void Motors_Set_Scaled_Speed(int motor_index, double speed);

#endif
