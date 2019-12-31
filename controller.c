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

#include "math.h"

volatile Controller_Test_State controller_test_state = Stop;
volatile Controller_Auto_State controller_auto_state;
volatile double controller_manual_left, controller_manual_right;
volatile Controller_Mode controller_mode = ModeTest; // Start in test mode

static const uint32_t TEST_LEFT_RIGHT_SPIN_COUNT = 6;
static const uint32_t TEST_LEFT_RIGHT_LED_BLINK_MS = 500;
static uint32_t test_left_right_start_spin_count;
static uint32_t test_left_right_start_controller_loop_counter;

static uint32_t controller_loop_counter=0;

volatile double Kp = 0.05, Kd = 1, Ki = 0, Kk = 60;
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

void setPID(double _Kp, double _Ki, double _Kd) {
	Kp = _Kp;
	Ki = _Ki;
	Kd = _Kd;
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

#define abs(xx) (xx < 0 ? -(xx) : (xx))

void Controller_Auto_Update() {
	
	static int arr[10];
	int s = 0;
	
	if (controller_auto_state == Wait || controller_auto_state == StoppedNew) {
		motors_stop();
		s = 0;
		return;
	}
	if (controller_auto_state == Started) {
		if (check_bright_light()) {
			controller_auto_state = StoppedNew;
			motors_stop();
			s = 0;
		}
		else {
			// TODO: Probably use a PID controller
			
			int ultrasonValueFiltered;
			int mini = 1000, maxi = -1000;
			
			if (s < 10){
				arr[s++] = ultrasonicSensorLastMeasurementCM;
			}
			else {
				int i;
				for (i=0; i<9; i++)
					arr[i] = arr[i+1];
				arr[9] = ultrasonicSensorLastMeasurementCM;
			}
			
			int i,j;
			int arr2[10];
			for(i=0;i<10;i++)
				arr2[i] = arr[i];
			
			for (i=0; i<s; i++)
				for (j=i+1; j<s; j++)
					if (arr2[i]>arr2[j]){
						int temp = arr2[i];
						arr2[i] = arr2[j];
						arr2[j] = temp;
					}
					
			ultrasonValueFiltered = (arr2[4]+arr2[5]) / 2;
			
			
			double Ku = Kp / 0.6;
			double Tu = dt;
			double Ti = Tu / 2;
			double Td = Tu / 8;
			
			double Kii = Ki*Ku/Tu;
			double Kdd = Kd*Ku*Tu/40;
			
			double target = 13.;
			
			double err = (ultrasonicSensorLastMeasurementCM - target);
			
			double derivative = (err-lastError) / dt;
			cumulativeError += err * dt;
			
			double pid_value = Kp*err + Kii*cumulativeError +  Kdd*derivative;
			
			double rightMotorSpeed = 0.5+pid_value;
			double leftMotorSpeed  = 0.5-pid_value;
			
			if (err > lastError && abs(pid_value) > 5)
			{
				rightMotorSpeed = leftMotorSpeed = 0.5;
			}
			
			if(leftMotorSpeed < -0.1) leftMotorSpeed = -0.1;
			if(rightMotorSpeed < -0.1) rightMotorSpeed = -0.1;
			
			Motors_Set_Scaled_Speed(0, leftMotorSpeed);
			Motors_Set_Scaled_Speed(1, rightMotorSpeed);
			
			lastError = err;
		}
	}
}

void Controller_Manual_Update() {
	Motors_Set_Scaled_Speed(0, controller_manual_left);
	Motors_Set_Scaled_Speed(1, controller_manual_right);
}

void Controller_Update() {
	controller_loop_counter++;
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
