#include "LPC407x_8x_177x_8x.h"
#include <string.h>
#include <stdio.h>

#include "Library/Offboard_LED.h"
#include "Library/Onboard_LED.h"
#include "Library/Motors.h"
#include "Library/ADC.h"
#include "Library/Timer.h"
#include "Library/Ultrasonic.h"
#include "Library/UART.h"
#include "Library/HM10.h"
#include "Library/ControllerLoop.h"
#include "Library/SpinCounter.h"
#include "testers.h"
#include "controller.h"

static void serial_recv_callback(volatile char* buffer, int old_size, int new_size);
static void hm10_recv_callback(volatile char* buffer, int old_size, int new_size);

static void init() {
	Motors_Init();
	Controller_Loop_Init(Controller_Update);
	ADC_Init();
	ADC_Start();
	Ultrasonic_Init();
	spin_counter_init();
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

static void serial_recv_callback(volatile char* buffer, int old_size, int new_size) {
	volatile char* cur;
	for (cur = buffer+old_size; cur<buffer+new_size; cur++) {
		uart_write_n(0, (const char*)cur, 1);
		if (*cur == '\r')
			uart_write(0, "\n");
	}
	uart_clear_buffer(0); // We never explicitly read from uart0 (uart_readline). To keep the buffer from filling up, we explicitly clean it here.
}

static void hm10_recv_callback(volatile char* buffer, int old_size, int new_size) {
	uart_write_n(0, (const char *)(buffer+old_size), new_size-old_size);
}

static void create_status_information(char* buf) {
	sprintf(buf, "{\"count\":%d, \"distance\":%d,\"light_level_left\":%d,\"light_level_right\":%d,\"op_mode\":\"%s\"}\r\n",
		spin_counter_get_count(),
		ultrasonicSensorLastMeasurementCM,
		ADC_GetLastValueOfLeftLDR(),
		ADC_GetLastValueOfRightLDR(),
		controller_in_test?"TEST":"AUTO"
	);
}

static void update() {
	static char line[128];
	static char status_buf[256];
	
	uart_readline(3, "\r\n", line);
	if (strcmp(line, "STATUS\r\n")==0) {
		create_status_information(status_buf);
		uart_write(3, status_buf);
		return;
	}
	if (controller_in_test) {
		if (strcmp(line, "LEFT\r\n")==0)
			controller_test_state = LeftNew;
		else if (strcmp(line, "RIGHT\r\n")==0)
			controller_test_state = RightNew;
		else if (strcmp(line, "FORWARD\r\n")==0)
			controller_test_state = Forward;
		else if (strcmp(line, "BACK\r\n")==0)
			controller_test_state = Back;
		else if (strcmp(line, "STOP\r\n")==0)
			controller_test_state = Stop;
		else if (strcmp(line, "AUTO\r\n")==0) {
			controller_auto_state = Wait;
			controller_in_test=0;
		}
		uart_write(3, line);
		if (strcmp(line, "AUTO\r\n")==0)
			uart_write(3, "AUTONOMOUS\r\n");
	}
	else { // Auto mode
		if (strcmp(line, "TEST\r\n")==0) {
			controller_test_state = Stop;
			controller_in_test=1;
			return;
		}
		if (controller_auto_state == Wait) {
			if (strcmp(line, "START\r\n")==0)
				controller_auto_state = Started;
		}
		if (controller_auto_state == StoppedNew) {
			uart_write(3, "FINISH\r\n");
			controller_auto_state = StoppedStale;
		}
	}
}

int main() {
	init();

	while (1) {
		update();
	}
}
