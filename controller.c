#include "controller.h"
#include "LPC407x_8x_177x_8x.h"

#include "Library/GPIO.h"
#include "Library/Offboard_LED.h"
#include "Library/Onboard_LED.h"
#include "Library/Motors.h"
#include "Library/ADC.h"
#include "Library/Timer.h"
#include "Library/Ultrasonic.h"
#include "Library/UART.h"
#include "Library/HM10.h"
#include "Library/ControllerLoop.h"

volatile Controller_Test_State controller_test_state = Stop;
volatile Controller_Auto_State controller_auto_state;
volatile int controller_in_test = 1; // Start in test mode

static const uint32_t TEST_LEFT_DURATION_MS = 1000;
static const uint32_t TEST_RIGHT_DURATION_MS = TEST_LEFT_DURATION_MS;
static const uint32_t TEST_LEFT_RIGHT_LED_BLINK_MS = 500;
static uint32_t test_left_start_time, test_right_start_time;

static uint32_t controller_loop_counter=0;

void motors_stop() {
	int i;
	for (i=0; i<4; i++)
		Motors_Set_Scaled_Speed(i, 0);
	Offboard_LEDs_Set_State(0,0,0,0);
}

void motors_left() {
	Motors_Set_Scaled_Speed(0, 0.8);
	Motors_Set_Scaled_Speed(1, 0.9);
	Motors_Set_Scaled_Speed(2, 0);
	Motors_Set_Scaled_Speed(3, 0.6);
}

void motors_right() {
	Motors_Set_Scaled_Speed(0, 0.9);
	Motors_Set_Scaled_Speed(1, 0.8);
	Motors_Set_Scaled_Speed(2, 0.6);
	Motors_Set_Scaled_Speed(3, 0);
}

void motors_forward() {
	Motors_Set_Scaled_Speed(0, 1);
	Motors_Set_Scaled_Speed(1, 1);
	Motors_Set_Scaled_Speed(2, 1);
	Motors_Set_Scaled_Speed(3, 1);
	Offboard_LEDs_Set_State(0,0,1,0);
}

void motors_backward() {
	Motors_Set_Scaled_Speed(0, -1);
	Motors_Set_Scaled_Speed(1, -1);
	Motors_Set_Scaled_Speed(2, -1);
	Motors_Set_Scaled_Speed(3, -1);
	Offboard_LEDs_Set_State(0,0,0,1);
}

int check_bright_light() {
	//return 0;
	return ADC_GetLastValueOfLeftLDR() >= 500 || ADC_GetLastValueOfRightLDR() >= 500;
}

void Controller_Test_Update() {
	if (check_bright_light()) {
		motors_stop();
		return;
	}
	if (controller_test_state == LeftNew) {
		test_left_start_time = controller_loop_counter;
		motors_left();
		Offboard_LEDs_Set_State(1,0,0,0);
		controller_test_state = LeftOngoing;
	}
	else if (controller_test_state == LeftOngoing) {
		if (controller_loop_counter - test_left_start_time >= TEST_LEFT_DURATION_MS/CONTROLLER_LOOP_PERIOD_MS) { // TODO: Time keeps passing even when we are paused
			motors_stop();
			controller_test_state = Stop;
		}
		else {
			if ((controller_loop_counter - test_left_start_time) * CONTROLLER_LOOP_PERIOD_MS % TEST_LEFT_RIGHT_LED_BLINK_MS < TEST_LEFT_RIGHT_LED_BLINK_MS/2)
				Offboard_LEDs_Set_State(1,0,0,0);
			else
				Offboard_LEDs_Set_State(0,0,0,0);
			motors_left();
		}
	}
	else if (controller_test_state == RightNew) {
		test_right_start_time = controller_loop_counter;
		motors_right();
		Offboard_LEDs_Set_State(0,1,0,0);
		controller_test_state = RightOngoing;
	}
	else if (controller_test_state == RightOngoing) {
		if (controller_loop_counter - test_right_start_time >= TEST_RIGHT_DURATION_MS/CONTROLLER_LOOP_PERIOD_MS) { // TODO: Time keeps passing even when we are paused
			motors_stop();
			controller_test_state = Stop;
		}
		else {
			if ((controller_loop_counter - test_right_start_time) * CONTROLLER_LOOP_PERIOD_MS % TEST_LEFT_RIGHT_LED_BLINK_MS < TEST_LEFT_RIGHT_LED_BLINK_MS/2)
				Offboard_LEDs_Set_State(0,1,0,0);
			else
				Offboard_LEDs_Set_State(0,0,0,0);
			motors_right();
		}
	}
	else if (controller_test_state == Forward)
		motors_forward();
	else if (controller_test_state == Back)
		motors_backward();
	else if (controller_test_state == Stop)
		motors_stop();
}

void Controller_Auto_Update() {
	if (controller_auto_state == Wait || controller_auto_state == StoppedNew || controller_auto_state == StoppedStale) {
		motors_stop();
		return;
	}
	if (controller_auto_state == Started) {
		if (check_bright_light()) {
			controller_auto_state = StoppedNew;
			motors_stop();
		}
		else {
			Motors_Set_Scaled_Speed(0, 0.9); // TODO: Actually follow the wall.
			Motors_Set_Scaled_Speed(1, 0.9); // Probably use a PID-controller
			Motors_Set_Scaled_Speed(2, 0.9); // Factor in the value of potentiometer
			Motors_Set_Scaled_Speed(3, 0.9);
		}
	}
}

void Controller_Update() {
	controller_loop_counter++;
	if (controller_in_test) {
		Controller_Test_Update();
	}
	else {
		Controller_Auto_Update();
	}
}
