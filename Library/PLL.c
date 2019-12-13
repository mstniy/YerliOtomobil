#include "PLL.h"

void PLL_Init() {
	CCLKSEL = 0x01;
	PCLKSEL = 0x04;
	EMCCLKSEL = 0x00;
	USBCLKSEL = 0x00;
	SPIFICLKSEL = 0x00;
}

void PLL_Disable() {
	PLLCON = 0x00;
}

void PLL_Feed() {
  PLLFEED = 0xAA;
	PLLFEED = 0x55;
}

void PLL_Change_Source() {
	CLKSRCSEL = 0x01;
}

void PLL_Change_Frequency() {
	//Change PLL Frequency to 60MHz
}

void PLL_Enable() {
	PLLCON = 0x01;
}

void PLL_Check_State() {
	//Wait until the related PLL is locked onto the requested frequency
}

void PLL_Start() {
	CCLKSEL = 0x0101;

	PCLKSEL = 0x03;
	
	EMCCLKSEL = 0x01;
	USBCLKSEL = 0x201;
	SPIFICLKSEL = 0x02;
}

void PLL_Change_Configuration() {
	PLL_Init();
	PLL_Disable();
	PLL_Feed();
	PLL_Change_Source();
	PLL_Change_Frequency();
	PLL_Enable();
	PLL_Feed();
	PLL_Check_State();
	PLL_Start();
}

