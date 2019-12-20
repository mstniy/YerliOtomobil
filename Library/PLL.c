#include "PLL.h"

void PLL_Init() {
	CCLKSEL = 0x01;
	PCLKSEL = 0x04;
}

void PLL_Disable() {
	PLLCON = 0x00;
}

void PLL_Feed() {
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
}

void PLL_Change_Source() {
	// SELECT IRC CLOCK
	CLKSRCSEL = 0x00;
}

void PLL_Change_Frequency() {
	//Change PLL Frequency to 60MHz

	// MSEL = 4, PSEL = 1 -> M = 5, P = 2, out = 12000000*5 = 60Mhz, ffc0 = 60Mhz*2*2 = 240Mhz, 150<240<350
	// PLLCFG = 0100100
	PLLCFG = 0x24;
}

void PLL_Enable() {
	PLLCON = 0x01;
}

void PLL_Check_State() {
	//Wait until the related PLL is locked onto the requested frequency
	while(0 == (PLLSTAT & (1<<10)));
}

void PLL_Start() {
	CCLKSEL = 0x0101;

	PCLKSEL = 0x03;
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

