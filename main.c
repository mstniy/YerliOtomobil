#include "LPC407x_8x_177x_8x.h"

#include "Library/GPIO.h"
#include "Library/Joystick.h"
#include "Library/Offboard_LED.h"
#include "Library/Onboard_LED.h"
#include "Library/Motors.h"
#include "Library/ADC.h"
#include "Library/Timer.h"
#include "Library/Ultrasonic.h"
#include "Library/UART.h"
#include "Library/HM10.h"

void init_hm10();
void serial_recv_callback(volatile char* buffer, int old_size, int new_size);
void hm10_recv_callback(volatile char* buffer, int old_size, int new_size);

void init() {
	Motors_Init();
	//Controller_Loop_Init();
	ADC_Init();
	ADC_Start();
  Ultrasonic_Init();
	Offboard_LEDs_Init();
	Onboard_LEDs_Init();
	uart_init(0, 115200);
	uart_attach_recv_callback(0, serial_recv_callback);
	uart_attach_recv_callback(3, hm10_recv_callback);
	uart_write(0, "Hello!\r\n");
	hm10_init("YerliOtomobil");
	
	Offboard_LEDs_Set_State(0, 0, 0, 0);
	Onboard_LEDs_Set_State(0, 0, 0, 0);
}

void update_adc_test() {
	uint32_t data;
	if (ADC_New_Data_Available[POTENTIOMETER] == 0)
			return ;
	data = ADC_GetLastValueOfPotentiometer();
	data=data;
}

void update_motor_test() {
	int i;
	Motors_Set_Scaled_Speed(3, 1);
	for (i=0; i<30000000;i++)
		;
	Motors_Set_Scaled_Speed(3, -1);
	for (i=0; i<30000000;i++)
		;
	Motors_Set_Scaled_Speed(3, 0);
	for (i=0; i<30000000;i++)
		;
}

void update_led_test() {
	int i;
	Offboard_LEDs_Set_State(0,0,0,0);
	Onboard_LEDs_Set_State(0,0,0,0);
	for (i=0;i<4000000;i++)
		;
	Offboard_LEDs_Set_State(1,1,1,1);
	Onboard_LEDs_Set_State(1,1,1,1);
	for (i=0;i<4000000;i++)
		;
}

void update_ultrasonic_test() {
	double dist_cm, scaled_speed;
	if (ultrasonicSensorNewDataAvailable == 0)
		return ;
	ultrasonicSensorNewDataAvailable = 0;
	dist_cm = (ultrasonicSensorFallingCaptureTime-ultrasonicSensorRisingCaptureTime)/58.0;
	if (dist_cm < 5)
		Onboard_LEDs_Set_State(1,1,1,1);
	else if (dist_cm < 10)
		Onboard_LEDs_Set_State(1,1,1,0);
	else if (dist_cm < 15)
		Onboard_LEDs_Set_State(1,1,0,0);
	else if (dist_cm < 25)
		Onboard_LEDs_Set_State(1,0,0,0);
	else
		Onboard_LEDs_Set_State(0,0,0,0);
	scaled_speed = dist_cm/50;
	if (scaled_speed<0)
		scaled_speed = 0;
	if (scaled_speed > 1)
		scaled_speed = 1;
	Motors_Set_Scaled_Speed(3, scaled_speed);
}

void update_bluetooth_test() {
	static char buf[128]; // Do not use the stack
	uint32_t n_read;
	
	n_read = uart_readline(0, "\r", buf);
	buf[n_read++] = '\n';
	buf[n_read] = '\0';
	uart_write(3, buf); // Send to bluetooth
}

void serial_recv_callback(volatile char* buffer, int old_size, int new_size) {
	volatile char* cur;
	for (cur = buffer+old_size; cur<buffer+new_size; cur++) {
		uart_write_n(0, (const char*)cur, 1);
		if (*cur == '\r')
			uart_write(0, "\n");
	}
}

void hm10_recv_callback(volatile char* buffer, int old_size, int new_size) {
	uart_write_n(0, (const char *)(buffer+old_size), new_size-old_size);
	uart_clear_buffer(3); // We don't explicitly read from hm10, so to keep the bufer from filling up, we clean it here.
												// Note that once we implement the logic that reads commands from HM10, we will need to remove this line.
}
 
int main() {
	init();

	while (1) {
		update_bluetooth_test();
	}
}
