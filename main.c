#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/PWM.h"
#include "Library/Joystick.h"
#include "Library/LED.h"

void EnableMotors() {
	const uint32_t port1mask = (1<<20)|(1<<23)|(1<<24);
	GPIO_DIR_Write(GPIO_PORT1,port1mask,OUTPUT);
	GPIO_DIR_Write(GPIO_PORT0,1<<21,OUTPUT);
	GPIO_PIN_Write(GPIO_PORT1,port1mask,HIGH);
	GPIO_PIN_Write(GPIO_PORT0,1<<21,HIGH);
}

void init() {
	//Joystick_Init();
	EnableMotors();
	PWM_Init();
	LED_Init();
	LED_OFF();
	
	PWM_Write(0, 0.75);
	
	//Initialize GPIO pins
}

void update() {
	volatile int i;
	LED1_On();
	for (i=0; i<1000000; i++)
		;
	LED1_Off();
	for (i=0; i<1000000; i++)
		;
}
 
int main() {
    init();
 
    while(1) {
        update();
    }
}
