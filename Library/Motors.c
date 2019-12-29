#include <math.h>
#include "Motors.h"
#include "ADC.h"

void Motors_Init(void) {
	const uint32_t port1mask = (1<<20)|(1<<23);
	const uint32_t port0mask = (1<<5);
	const uint32_t port5mask = (1<<2);
	
	PWM_Init();
	
	GPIO_DIR_Write(GPIO_PORT1,port1mask,OUTPUT);
	GPIO_DIR_Write(GPIO_PORT0,port0mask,OUTPUT);
	GPIO_DIR_Write(GPIO_PORT5,port5mask,OUTPUT);
	
  Motors_Set_Scaled_Speed(0, 0);
	Motors_Set_Scaled_Speed(1, 0);
}

void Motors_Set_Scaled_Speed(int motor_index, double speed) {
	const double potentiometerLimit = ADC_GetLastValueOfPotentiometer()/4095.0; // Potentiometer acts as a speed multiplicator
	int in1, in2;
	if (speed == 0)
		in1 = in2 = LOW; // Brake
	else if (speed > 0) {
		in1 = LOW;
		in2 = HIGH;
	}
	else {
		in1 = HIGH;
		in2 = LOW;
	}
	if (motor_index == 0) {
		GPIO_PIN_Write(GPIO_PORT1,1<<20,in1);
		GPIO_PIN_Write(GPIO_PORT0,1<<5,in2);
	}
	else if (motor_index == 1) {
		GPIO_PIN_Write(GPIO_PORT1,1<<23,in1);
		GPIO_PIN_Write(GPIO_PORT5,1<<2,in2);
	}
	speed = fabs(speed);
	//speed *= 0.9; // The maximum recommended duty cycle for the motors is 90%, but we use a 5V source, not 12.
	if (speed > potentiometerLimit)
		speed = potentiometerLimit;
	PWM_Write(motor_index, speed);
}
