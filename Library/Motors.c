#include <math.h>
#include "Motors.h"

void Motors_Init(void) {
	const uint32_t port1mask = (1<<20)|(1<<23)|(1<<24);
	const uint32_t port0mask = (1<<4)|(1<<5)|(1<<21);
	const uint32_t port5mask = (1<<2)|(1<<3);
	int i;
	
	PWM_Init();
	
	GPIO_DIR_Write(GPIO_PORT1,port1mask,OUTPUT);
	GPIO_DIR_Write(GPIO_PORT0,port0mask,OUTPUT);
	GPIO_DIR_Write(GPIO_PORT5,port5mask,OUTPUT);
	
	for (i=0; i<4; i++)
		Motors_Set_Scaled_Speed(i, 0);
}

void Motors_Set_Scaled_Speed(int motor_index, double speed) {
	uint32_t temp;
	int in1, in2;
	if (speed == 0)
		in1 = in2 = LOW; // Brake
	else if (speed > 0) {
		in1 = HIGH;
		in2 = LOW;
	}
	else {
		in1 = LOW;
		in2 = HIGH;
	}
	if (motor_index == 0) {
		GPIO_PIN_Write(GPIO_PORT1,1<<24,in1);
		GPIO_PIN_Write(GPIO_PORT5,1<<3,in2);
	}
	else if (motor_index == 1) {
		GPIO_PIN_Write(GPIO_PORT1,1<<23,in1);
		GPIO_PIN_Write(GPIO_PORT5,1<<2,in2);
	}
	else if (motor_index == 2) {
		GPIO_PIN_Write(GPIO_PORT1,1<<20,in1);
		GPIO_PIN_Write(GPIO_PORT0,1<<5,in2);
	}
	else if (motor_index == 3) {
		temp = GPIO_PORT0->PIN;
		temp &= ~((1<<21)|(1<<4));
		temp |= (in1<<21)|(in2<<4);
		GPIO_PORT0->PIN = temp;
	}
	
	PWM_Write(motor_index, fabs(speed)*0.9); // The maximum recommended duty cycle for the motors is 90%
}
