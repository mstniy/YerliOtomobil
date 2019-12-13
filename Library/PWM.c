#include "PWM.h"

void PWM_Init() {
	uint32_t ioconRegisterValue = IOCON_RED_RGB;
	ioconRegisterValue |= 0x3;
	ioconRegisterValue &= ~(1 << 2);
	IOCON_RED_RGB = ioconRegisterValue;
	
	PCONP |= (1 << 5 | 1 << 6);
	
	//Enable PWM output for corresponding pin.
	
	PWMX->TCR = 1 << 1;
	
	PWMX->PR = 9;
	
	PWM0->MR0 = (PERIPHERAL_CLOCK_FREQUENCY / 1000000) * 20 * 1000;
	
	PWMX->MCR = 1 << 1;
	
	PWMX->LER |= 1 << 0;
	
	PWMX->TCR = (1 << 0 | 1 << 3);

	//Clear pending for PWM0
	
	//Set Priority PWM0 IRQ as 5
	
	//Enable PWM0_IRQn (Interrupt Request)
	
	PWM_Write(0);
}

void PWM_Cycle_Rate(uint32_t period_In_Cycles) {
	PWM0->MR0 = (PERIPHERAL_CLOCK_FREQUENCY / 1000000) * period_In_Cycles * 1000;
	
	PWMX->LER |= 1 << 0;
}

void PWM_Write(uint32_t T_ON) {	
	if(T_ON > 100) {
		T_ON = 100;
	}
	
	T_ON = (uint32_t)(((PWM0->MR0) * T_ON) / 100);
	
	if (T_ON == PWM0->MR0) {
		T_ON++;
	}
	PWM0->MR6 = T_ON;
	
	PWM0->LER |= 1 << 6;
}
