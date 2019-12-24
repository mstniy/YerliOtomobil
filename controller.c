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

void Controller_Update() {
	static int cnt=0;
	cnt += 1;
	if (cnt%40==0)
		Onboard_LEDs_Set_State(1,1,1,1);
	if (cnt%40==20)
		Onboard_LEDs_Set_State(0,0,0,0);
}
