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

#define ULTRASOUND_FAULT_THRESHOLD_CM 60

typedef enum {
	Usual=0,
	SearchTurningLeft, SearchForward, // Will loop until a wall is found
	GoAwayRight, GoAwayForward,
	ComeCloseLeft, ComeCloseForward, ComeCloseRight
} AutoControllerInternalState;

static AutoControllerInternalState acis = Usual;
static double lasts[3] = {0};
static int lasts_size = 0;
static int correction_action_start_ms;

volatile Controller_Test_State controller_test_state = Stop;
volatile Controller_Auto_State controller_auto_state;
volatile double controller_manual_left, controller_manual_right;
volatile Controller_Mode controller_mode = ModeTest; // Start in test mode

static const uint32_t TEST_LEFT_RIGHT_SPIN_COUNT = 6;
static const uint32_t TEST_LEFT_RIGHT_LED_BLINK_MS = 500;
static uint32_t test_left_right_start_spin_count;
static uint32_t test_left_right_start_ms;

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
	return ADC_GetLastValueOfLeftLDR() >= 500 || ADC_GetLastValueOfRightLDR() >= 500;
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

/*static double getMedian(double a, double b, double c) {
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
}*/

static void AutoControllerSetMotorStates() {
	if (acis == Usual) {
		Motors_Set_Scaled_Speed(0, 0.6); // "Usual" does not have a fixed speed, it may try to nodge the vehicle to the left or to the right, if the distance error is small enough. But this is a nice approximation.
		Motors_Set_Scaled_Speed(1, 0.6);
	}
	if (acis == SearchTurningLeft) {
		uart_write(3, "S\r\n");
		Motors_Set_Scaled_Speed(0, -0.8);
		Motors_Set_Scaled_Speed(1, 0.8);
	}
	else if (acis == SearchForward) {
		Motors_Set_Scaled_Speed(0, 0.6);
		Motors_Set_Scaled_Speed(1, 0.6);
	}
	else if (acis == GoAwayRight) {
		uart_write(3, "GA\r\n");
		Motors_Set_Scaled_Speed(0, 0.8);
		Motors_Set_Scaled_Speed(1, -0.8);
	}
	else if (acis == GoAwayForward) {
		Motors_Set_Scaled_Speed(0, 0.6);
		Motors_Set_Scaled_Speed(1, 0.6);
	}
	else if (acis == ComeCloseLeft) {
		uart_write(3, "CC\r\n");
		Motors_Set_Scaled_Speed(0, -1);
		Motors_Set_Scaled_Speed(1, 1);
	}
	else if (acis == ComeCloseForward) {
		Motors_Set_Scaled_Speed(0, 0.6);
		Motors_Set_Scaled_Speed(1, 0.6);
	}
	else if (acis == ComeCloseRight) {
		Motors_Set_Scaled_Speed(0, 0.8);
		Motors_Set_Scaled_Speed(1, -0.8);
	}
}

static void AutoControllerChangeState(AutoControllerInternalState _acis) {
	acis = _acis;
	AutoControllerSetMotorStates();
	correction_action_start_ms = get_ms();
}

void Controller_Auto_Update() {
	double minCM;
	
	if (controller_auto_state != StartedNew && controller_auto_state != StartedStale) {
		motors_stop();
		return;
	}
	
	if (controller_auto_state == StartedNew) {
		acis = Usual;
		lasts_size = 0;
		controller_auto_state = StartedStale;
		return ;
	}
	
	if (check_bright_light()) {
		controller_auto_state = StoppedNew;
		motors_stop();
		return ;
	}
	
	memmove(lasts, lasts+1, 2*sizeof(double));
	lasts[2] = ultrasonicSensorLastMeasurementCM;
	lasts_size++;
	if (lasts_size>=3) {
		minCM = lasts[0];
		if (lasts[1] < minCM) minCM = lasts[1];
		if (lasts[2] < minCM) minCM = lasts[2];
	}
	else {
		minCM = lasts[2];
	}
	
	if (acis == SearchTurningLeft) {
		if (minCM <= ULTRASOUND_FAULT_THRESHOLD_CM)
			AutoControllerChangeState(Usual);
		if (get_ms() - correction_action_start_ms >= 400)
			AutoControllerChangeState(SearchForward);
		return ;
	}
	else if (acis == SearchForward) {
		if (minCM <= ULTRASOUND_FAULT_THRESHOLD_CM)
			AutoControllerChangeState(Usual);
		if (get_ms() - correction_action_start_ms >= 1000)
			AutoControllerChangeState(SearchTurningLeft);
		return ;
	}
	else if (acis == GoAwayRight) {
		if (get_ms() - correction_action_start_ms >= 300)
			AutoControllerChangeState(GoAwayForward);
		return ;
	}
	else if (acis == GoAwayForward) {
		if (get_ms() - correction_action_start_ms >= 500)
			AutoControllerChangeState(Usual);
		return ;
	}
	else if (acis == ComeCloseLeft) {
		if (get_ms() - correction_action_start_ms >= 200)
			AutoControllerChangeState(ComeCloseForward);
		return ;
	}
	else if (acis == ComeCloseForward) {
		if (get_ms() - correction_action_start_ms >= 200)
			AutoControllerChangeState(ComeCloseRight);
		return ;
	}
	else if (acis == ComeCloseRight) {
		if (get_ms() - correction_action_start_ms >= 200)
			AutoControllerChangeState(Usual);
		return ;
	}
	
	if (lasts_size>=3 && minCM > ULTRASOUND_FAULT_THRESHOLD_CM) { // Wrong sensor measurement. Probably the wall turning sharply, thus the echo not making it to the sensor.
		AutoControllerChangeState(SearchTurningLeft);
		return ;
	}
	
	if (lasts_size>=3 && minCM < 12) {
		AutoControllerChangeState(GoAwayRight);
	}
	else if (minCM < 15) {
		Motors_Set_Scaled_Speed(0, 1);
		Motors_Set_Scaled_Speed(1, 0.3);
	}
	/*else if (lasts_size>=3 && minCM > 35) {
		AutoControllerChangeState(ComeCloseLeft);
	}*/
	else if (minCM > 30) {
		Motors_Set_Scaled_Speed(0, 0.3);
		Motors_Set_Scaled_Speed(1, 1);
	}
	else {
		Motors_Set_Scaled_Speed(0, 0.9);
		Motors_Set_Scaled_Speed(1, 0.9);
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
