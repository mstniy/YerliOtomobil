#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/Joystick.h"
#include "Library/LED.h"
#include "Library/Motors.h"
#include "Library/ADC.h"
#include "Library/Timer.h"

void init() {
	//Joystick_Init();
	Motors_Init();
	LED_Init();
	LED_OFF();
	Timer0_Init();
	ADC_Init();
	ADC_Start();
}

void update_adc() {
	uint32_t data;
	if (ADC_New_Data_Available[POTENTIOMETER] == 0)
			return ;
	data = ADC_GetLastValueOfPotentiometer();
	data=data;
}
 
int main() {
    init();
 
    while(1) {
			update_adc();
		}
}
