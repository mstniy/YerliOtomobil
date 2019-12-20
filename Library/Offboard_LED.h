#ifndef OFFBOARD_LED_H
#define OFFBOARD_LED_H

#include "GPIO.h"

void Offboard_LEDs_Init(void);

void Offboard_LEDs_ON(void);
void LEFT_LED_On(void);
void RIGHT_LED_On(void);
void FRONT_LED_On(void);
void BACK_LED_On(void);

void Offboard_LEDs_OFF(void);
void LEFT_LED_Off(void);
void RIGHT_LED_Off(void);
void FRONT_LED_Off(void);
void BACK_LED_Off(void);

#endif
