#include "LPC407x_8x_177x_8x.h"

#include "Library/Offboard_LED.h"
#include "Library/Onboard_LED.h"
#include "Library/Motors.h"
#include "Library/ADC.h"
#include "Library/Timer.h"
#include "Library/Ultrasonic.h"
#include "Library/UART.h"
#include "Library/HM10.h"
#include "Library/ControllerLoop.h"
#include "testers.h"
#include "controller.h"

void init_hm10(void);
void serial_recv_callback(volatile char* buffer, int old_size, int new_size);
void hm10_recv_callback(volatile char* buffer, int old_size, int new_size);

void init() {
	Motors_Init();
	Controller_Loop_Init(Controller_Update);
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
