#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/PWM.h"
#include "Library/HM10.h"
#include "Library/Joystick.h"
#include "Library/Serial.h"

void init() {
	Joystick_Init();
//	PWM_Init();
	
	Serial_Init();
	HM10_Init();
	
	//Initialize GPIO pins
}

char buf[128];
int i;
volatile int x;

void handle_command() {
	buf[i++] = '\n';
	buf[i++] = '\0';
	HM10_Write(buf);
	i = 0;

	
	for (x = 0; x < 1000000; x++);
	
	HM10Buffer[HM10CurrentBufferIndex] = 0;
	strcpy(buf, HM10Buffer);
	HM10_ClearBuffer();
	serialTransmitData = buf;
	Serial_WriteData(*serialTransmitData++);
	while(!serialTransmitCompleted)
		;
	strcpy(buf, "");
}

void update() {
		char s[3] = {0,0,0};
			
		while(!serialNewDataAvailable)
			;
		
		s[0] = serialReceivedCharacter;
		if (s[0] == '\r') { 
			s[1] = '\n';
		}
		
		buf[i++] = s[0];
			
		serialTransmitData = s;
	  Serial_WriteData(*serialTransmitData++);
		while(!serialTransmitCompleted)
			;
		
		if (s[0] == '\r')
			handle_command();

		
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
