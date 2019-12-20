#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/Joystick.h"
#include "Library/LED.h"
#include "Library/Motors.h"
#include "Library/Ultrasonic.h"

void init() {
	Motors_Init();
	Ultrasonic_Init();
	LED_Init();
}

void led_blink() {
	volatile int i;
	LED1_On();
	for (i=0; i<1000000; i++)
		;
	LED1_Off();
	for (i=0; i<1000000; i++)
		;
}

void update_motor_test() {
	int i;
	Motors_Set_Scaled_Speed(3, 1);
	for (i=0; i<15;i++)
		led_blink();
	Motors_Set_Scaled_Speed(3, -1);
	for (i=0; i<15;i++)
		led_blink();
	Motors_Set_Scaled_Speed(3, 0);
	for (i=0; i<15;i++)
		led_blink();
}

void update_ultrasonic_test() {
	double dist_cm, scaled_speed;
	if (ultrasonicSensorNewDataAvailable == 0)
		return ;
	ultrasonicSensorNewDataAvailable = 0;
	dist_cm = (ultrasonicSensorFallingCaptureTime-ultrasonicSensorRisingCaptureTime)/58.0;
	if (dist_cm < 5) {
		LED_ON();
	}
	else if (dist_cm < 10) {
		LED1_On();
		LED2_On();
		LED3_On();
		LED4_Off();
	}
	else if (dist_cm < 15) {
		LED1_On();
		LED2_On();
		LED3_Off();
		LED4_Off();
	}
	else if (dist_cm < 25) {
		LED1_On();
		LED2_Off();
		LED3_Off();
		LED4_Off();
	}
	else {
		LED_OFF();
	}
	scaled_speed = dist_cm/50;
	if (scaled_speed<0)
		scaled_speed = 0;
	if (scaled_speed > 1)
		scaled_speed = 1;
	Motors_Set_Scaled_Speed(3, scaled_speed);
}
 
int main() {
    init();
 
    while(1) {
				update_ultrasonic_test();
    }
}
