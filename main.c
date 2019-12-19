#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/Joystick.h"
#include "Library/LED.h"
#include "Library/Motors.h"

void init() {
	//Joystick_Init();
	Motors_Init();
	LED_Init();
	
	LED_OFF();
	
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
		int i;
    init();
 
    while(1) {
				Motors_Set_Scaled_Speed(3, 1);
				for (i=0; i<15;i++)
					update();
				Motors_Set_Scaled_Speed(3, -1);
				for (i=0; i<15;i++)
					update();
				Motors_Set_Scaled_Speed(3, 0);
				for (i=0; i<15;i++)
					update();
    }
}
