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

#define LOG_PERIOD_MS 100

//static void serial_recv_callback(volatile char* buffer, int old_size, int new_size);
//static void hm10_recv_callback(volatile char* buffer, int old_size, int new_size);

static int log_on=0;
static uint32_t log_last_send_ms = 0;

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
	//uart_attach_recv_callback(0, serial_recv_callback);
	//uart_attach_recv_callback(3, hm10_recv_callback);
	uart_write(0, "Hello!\r\n", 1);
	hm10_init("YerliOtomobil");
	uart_write(3, "TESTING\r\n", 1);
	
	Offboard_LEDs_Set_State(0, 0, 0, 0);
	Onboard_LEDs_Set_State(0, 0, 0, 0);
}

/*static void serial_recv_callback(volatile char* buffer, int old_size, int new_size) {
	volatile char* cur;
	for (cur = buffer+old_size; cur<buffer+new_size; cur++) {
		uart_write_n(0, (const char*)cur, 1);
		if (*cur == '\r')
			uart_write(0, "\n");
	}
	uart_clear_recv_buffer(0); // We never explicitly read from uart0 (uart_readline). To keep the buffer from filling up, we explicitly clean it here.
}

static void hm10_recv_callback(volatile char* buffer, int old_size, int new_size) {
	uart_write_n(0, (const char *)(buffer+old_size), new_size-old_size);
}*/

static const char* get_current_controller_mode_name() {
	if (controller_mode == ModeAuto)
		return "AUTO";
	if (controller_mode == ModeTest)
		return "TEST";
	if (controller_mode == ModeManual)
		return "MANUAL";
	return "BUG!";
}

static void create_status_information(char* buf) {
	sprintf(buf, "{\"ms\":%u, \"speeds\": [%.02lf, %.02lf], \"spin\":%u, \"pmeter\":%0.2lf, \"distance\":%0.1lf,\"light_level_left\":%u,\"light_level_right\":%u,\"op_mode\":\"%s\"}\r\n",
		get_ms(),
		Motors_Get_Last_Scaled_Speed(0),
		Motors_Get_Last_Scaled_Speed(1),
		spin_counter_get_count(),
		ADC_GetLastValueOfPotentiometer()/4095.0,
		ultrasonicSensorLastMeasurementCM,
		ADC_GetLastValueOfLeftLDR(),
		ADC_GetLastValueOfRightLDR(),
		get_current_controller_mode_name()
	);
}

static void update() {
	static char line[128];
	static char status_buf[256];
	
	if (controller_mode == ModeAuto && controller_auto_state==StoppedNew) {
		uart_write(3, "FINISH\r\n", 1);
		controller_auto_state = Wait;
	}
	
	if (log_on && get_ms() - log_last_send_ms >= LOG_PERIOD_MS) {
		log_last_send_ms = get_ms();
		create_status_information(status_buf);
		uart_write(3, status_buf, 1);
	}
		
	if(uart_readline(3, "\r\n", line) == -1) {
		return;
	}
	
	if (strcmp(line, "STATUS")==0) {
		uart_write(3, "STATUS\r\n", 1);
		create_status_information(status_buf);
		uart_write(3, status_buf, 1);
		return;
	}
	if (strcmp(line, "LOG ON") == 0) {
		log_on = 1;
		return ;
	}
	if (strcmp(line, "LOG OFF") == 0) {
		log_on = 0;
		return ;
	}
	if (strcmp(line, "AUTO") == 0) {
		controller_auto_state = Wait;
		controller_mode = ModeAuto;
		uart_write(3, "AUTO\r\n", 1);
		uart_write(3, "AUTONOMOUS\r\n", 1);
		return ;
	}
	if (strcmp(line, "TEST") == 0) {
		controller_test_state = Stop;
		controller_mode = ModeTest;
		uart_write(3, "TEST\r\n", 1);
		uart_write(3, "TESTING\r\n", 1);
		return ;
	}
	if (strcmp(line, "MANUAL") == 0) {
		controller_manual_left = controller_manual_right = 0.0;
		controller_mode = ModeManual;
		uart_write(3, "MANUAL\r\n", 1);
		return ;
	}
	if (controller_mode == ModeTest) {
		if (strcmp(line, "LEFT")==0)
			controller_test_state = LeftNew;
		else if (strcmp(line, "RIGHT")==0)
			controller_test_state = RightNew;
		else if (strcmp(line, "FORWARD")==0)
			controller_test_state = Forward;
		else if (strcmp(line, "BACK")==0)
			controller_test_state = Back;
		else if (strcmp(line, "STOP")==0) {
			if(controller_test_state != LeftOngoing &&
				 controller_test_state != RightOngoing) // ignore STOP command while turning
				controller_test_state = Stop;
		}
		uart_write(3, line, 1);
		uart_write(3, "\r\n", 1);
	}
	else if (controller_mode == ModeAuto) {
		if (strcmp(line, "STOP")==0) {
				controller_auto_state = Wait;
		}
		if (controller_auto_state == Wait) {
			if (strcmp(line, "START")==0) {
				controller_auto_state = StartedNew;
			}
		}
		uart_write(3, line, 1);
		uart_write(3, "\r\n", 1);
	}
	else if (controller_mode == ModeManual) {
		for (char* cp=line; *cp; cp++)
			if (*cp == ',')
				*cp = '.';
		sscanf(line, "DC %lf %lf", &controller_manual_left, &controller_manual_right);
	}
}

int main() {
	init();

	while (1) {
		update();
	}
}
