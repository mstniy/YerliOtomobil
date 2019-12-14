#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/PWM.h"
#include "Library/Joystick.h"

void init() {
	Joystick_Init();
	PWM_Init();
	
	//Initialize GPIO pins
}

void update() {
	
}
 
int main() {
    init();
 
    while(1) {
        update();
    }
}
