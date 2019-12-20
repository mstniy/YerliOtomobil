#ifndef ONBOARD_LED_H
#define ONBOARD_LED_H

#include "LPC407x_8x_177x_8x.h"
#include "GPIO.h"

void Onboard_LEDs_Init(void);

void Onboard_LEDs_ON(void);
void LED1_On(void);
void LED2_On(void);
void LED3_On(void);
void LED4_On(void);

void Onboard_LEDs_OFF(void);
void LED1_Off(void);
void LED2_Off(void);
void LED3_Off(void);
void LED4_Off(void);

#endif
