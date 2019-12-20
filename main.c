#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/PWM.h"
#include "Library/Joystick.h"
#include "Library/Serial.h"

void init() {
	Joystick_Init();
//	PWM_Init();
	
	Serial_Init();
	
	//Initialize GPIO pins
}

void update() {
		char s[3] = {0,0,0};
			
		while(!serialNewDataAvailable)
			;
		
		s[0] = serialReceivedCharacter;
		if (s[0] == '\r') 
			s[1] = '\n';
		

		serialTransmitData = s;
	  Serial_WriteData(*serialTransmitData++);
		while(!serialTransmitCompleted)
			;
		
		serialNewDataAvailable = 0;
}
 
int main() {
		char* s = "hello world\r\n";


    init();
	
	
		while(!serialTransmitCompleted)
			;

		serialTransmitData = s;
	  Serial_WriteData(*serialTransmitData++);
		
		
		
 
    while(1) {
        update();
    }
}
