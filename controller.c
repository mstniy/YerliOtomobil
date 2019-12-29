#include <stdlib.h>

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
#include "Library/SpinCounter.h"

volatile Controller_Test_State controller_test_state = Stop;
volatile Controller_Auto_State controller_auto_state;
volatile int controller_in_test = 1; // Start in test mode

static const uint32_t TEST_LEFT_RIGHT_SPIN_COUNT = 6;
static const uint32_t TEST_LEFT_RIGHT_LED_BLINK_MS = 500;
static uint32_t test_left_right_start_spin_count;
static uint32_t test_left_right_start_controller_loop_counter;

static uint32_t controller_loop_counter=0;

void motors_stop() {
	Motors_Set_Scaled_Speed(0, 0);
	Motors_Set_Scaled_Speed(1, 0);
	Offboard_LEDs_Set_State(0,0,0,0);
}

void motors_left() {
	Motors_Set_Scaled_Speed(0, -1);
	Motors_Set_Scaled_Speed(1, 1);
}

void motors_right() {
	Motors_Set_Scaled_Speed(0, 1);
	Motors_Set_Scaled_Speed(1, -1);
}

void motors_forward() {
	Motors_Set_Scaled_Speed(0, 1);
	Motors_Set_Scaled_Speed(1, 1);
	Offboard_LEDs_Set_State(1,1,0,0);
}

void motors_backward() {
	Motors_Set_Scaled_Speed(0, -1);
	Motors_Set_Scaled_Speed(1, -1);
	Offboard_LEDs_Set_State(0,0,1,1);
}

int check_bright_light() {
	return ADC_GetLastValueOfLeftLDR() >= 800 || ADC_GetLastValueOfRightLDR() >= 800;
}

void Controller_Test_Update() {	
	if (check_bright_light()) {
		motors_stop();
		return;
	}
	if (controller_test_state == LeftNew) {
		test_left_right_start_spin_count = spin_counter_get_count();
		test_left_right_start_controller_loop_counter = controller_loop_counter;
		motors_left();
		Offboard_LEDs_Set_State(1,0,1,0);
		controller_test_state = LeftOngoing;
	}
	else if (controller_test_state == LeftOngoing) {
		if (spin_counter_get_count() - test_left_right_start_spin_count >= TEST_LEFT_RIGHT_SPIN_COUNT) {
			motors_stop();
			controller_test_state = Stop;
		}
		else {
			if ((controller_loop_counter - test_left_right_start_controller_loop_counter) * CONTROLLER_LOOP_PERIOD_MS % TEST_LEFT_RIGHT_LED_BLINK_MS < TEST_LEFT_RIGHT_LED_BLINK_MS/2)
				Offboard_LEDs_Set_State(1,0,1,0);
			else
				Offboard_LEDs_Set_State(0,0,0,0);
			motors_left();
		}
	}
	else if (controller_test_state == RightNew) {
		test_left_right_start_spin_count = spin_counter_get_count();
		test_left_right_start_controller_loop_counter = controller_loop_counter;
		motors_right();
		Offboard_LEDs_Set_State(0,1,0,1);
		controller_test_state = RightOngoing;
	}
	else if (controller_test_state == RightOngoing) {
		if (spin_counter_get_count() - test_left_right_start_spin_count >= TEST_LEFT_RIGHT_SPIN_COUNT) {
			motors_stop();
			controller_test_state = Stop;
		}
		else {
			if ((controller_loop_counter - test_left_right_start_controller_loop_counter) * CONTROLLER_LOOP_PERIOD_MS % TEST_LEFT_RIGHT_LED_BLINK_MS < TEST_LEFT_RIGHT_LED_BLINK_MS/2)
				Offboard_LEDs_Set_State(0,1,0,1);
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
	int diff; 
	
	if (controller_auto_state == Wait || controller_auto_state == StoppedNew) {
		motors_stop();
		return;
	}
	if (controller_auto_state == Started) {
		if (check_bright_light()) {
			controller_auto_state = StoppedNew;
			motors_stop();
		}
		else {
			// TODO: Probably use a PID controller
			diff = abs(ultrasonicSensorLastMeasurementCM - 25);
			if (diff > 25) {
					diff = 25;
			}
			if (ultrasonicSensorLastMeasurementCM > 25) {
				Motors_Set_Scaled_Speed(0, 1 - (diff/25.0) * 0.99);
				Motors_Set_Scaled_Speed(1, 1);
			}
			else {
				Motors_Set_Scaled_Speed(0, 1);
				Motors_Set_Scaled_Speed(1, 1 - (diff/25.0) * 0.8);
			}
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
