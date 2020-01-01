#include <stdlib.h>
#include <string.h>

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

#include "math.h"

volatile Controller_Test_State controller_test_state = Stop;
volatile Controller_Auto_State controller_auto_state;
volatile double controller_manual_left, controller_manual_right;
volatile Controller_Mode controller_mode = ModeTest; // Start in test mode

static const uint32_t TEST_LEFT_RIGHT_SPIN_COUNT = 6;
static const uint32_t TEST_LEFT_RIGHT_LED_BLINK_MS = 500;
static uint32_t test_left_right_start_spin_count;
static uint32_t test_left_right_start_ms;

double cumulativeError, lastError, lastErrorDerivative;
const double dt = 0.05;

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
		test_left_right_start_ms = get_ms();
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
			if ((get_ms() - test_left_right_start_ms) % TEST_LEFT_RIGHT_LED_BLINK_MS < TEST_LEFT_RIGHT_LED_BLINK_MS/2)
				Offboard_LEDs_Set_State(1,0,1,0);
			else
				Offboard_LEDs_Set_State(0,0,0,0);
			motors_left();
		}
	}
	else if (controller_test_state == RightNew) {
		test_left_right_start_spin_count = spin_counter_get_count();
		test_left_right_start_ms = get_ms();
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
			if ((get_ms() - test_left_right_start_ms) % TEST_LEFT_RIGHT_LED_BLINK_MS < TEST_LEFT_RIGHT_LED_BLINK_MS/2)
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

double getMedian(double a, double b, double c) {
	double tmp;
	if (a>b) {
		tmp = a;
		a = b;
		b = tmp;
	}
	if (b>c) {
		tmp = b;
		b = c;
		c = tmp;
	}
	if (a>b) {
		tmp = a;
		a = b;
		b = tmp;
	}
	return b;
}

void Controller_Auto_Update() {
	static double lasts[3] = {0};
	double medianCM=0;
	
	if (controller_auto_state == Wait || controller_auto_state == StoppedNew) {
		motors_stop();
		return;
	}
	if (controller_auto_state == Started) {
		if (check_bright_light()) {
			controller_auto_state = StoppedNew;
			motors_stop();
			return ;
		}
		
		memmove(lasts, lasts+1, 2*sizeof(double));
		lasts[2] = ultrasonicSensorLastMeasurementCM;
		medianCM = getMedian(lasts[0], lasts[1], lasts[2]);
		
		if (medianCM > 50) { // Wrong sensor measurement. Probably the wall turning sharply, thus the echo not making it to the sensor.
			Motors_Set_Scaled_Speed(0, 0);
			Motors_Set_Scaled_Speed(1, 0);
		}
		else {
			if (medianCM < 15) {
				Motors_Set_Scaled_Speed(0, 0.5);
				Motors_Set_Scaled_Speed(1, 0.2);
			}
			else if (medianCM > 30) {
				Motors_Set_Scaled_Speed(0, 0.2);
				Motors_Set_Scaled_Speed(1, 0.5);
			}
			else {
				Motors_Set_Scaled_Speed(0, 0.35);
				Motors_Set_Scaled_Speed(1, 0.35);
			}
		}
	}
}

void Controller_Manual_Update() {
	Motors_Set_Scaled_Speed(0, controller_manual_left);
	Motors_Set_Scaled_Speed(1, controller_manual_right);
}

void Controller_Update() {
	if (controller_mode == ModeTest) {
		Controller_Test_Update();
	}
	else if (controller_mode == ModeAuto){
		Controller_Auto_Update();
	}
	else if (controller_mode == ModeManual){
		Controller_Manual_Update();
	}
}
