#include "PWM.h"

static void ioconSwitchToPWM(volatile uint32_t* iocon) {
	uint32_t temp;
	temp = *iocon;
	temp |= 0x3;
	temp &= ~(1 << 2);
	*iocon = temp;
}

void PWM_Init() {
	ioconSwitchToPWM(IOCON_MOTOR_0_PWM);
	ioconSwitchToPWM(IOCON_MOTOR_1_PWM);
	ioconSwitchToPWM(IOCON_MOTOR_2_PWM);
	ioconSwitchToPWM(IOCON_MOTOR_3_PWM);
	
	PCONP |= (1 << 5/* | 1 << 6*/); // Turn PWM0 on
	
	PWM0->PCR |= (1<<9)|(1<<10)|(1<<11)|(1<<12); // Enable PWM output on PWM[1..4]
	
	PWM0->TCR = 1 << 1; // Reset the timer counter and the prescale counter on the next clock cycle
	
	PWM0->PR = 0;
	
	PWM0->MR0 = PERIPHERAL_CLOCK_FREQUENCY / 1000 * 1; // 1ms PWM period. We set MR0 to a high-ish value so that it can be divided accurately.
	
	PWM0->MCR = 1 << 1; // Reset the timer counter when MR0 matches it
	
	PWM0->LER |= 1 << 0; // Apply the change to MR0
	
	PWM0->TCR = (1 << 0 | 1 << 3); // Enable PWM, start the counters and remove the reset we have previously set on the counters.

	/*
	We don't need interrupts, but had we needed, here are the steps:
	//Clear pending for PWM0
	//Set Priority PWM0 IRQ as 5
	//Enable PWM0_IRQn (Interrupt Request)
	*/
	
	for (int i=0; i<4; i++)
		PWM_Write(0, 0);
}

void PWM_Write(int motor_index, uint32_t T_ON) {	
	if(T_ON > 100) {
		T_ON = 100;
	}
	
	T_ON = (uint32_t)(((PWM0->MR0) * T_ON) / 100);
	
	if (T_ON == PWM0->MR0) {
		T_ON++;
	}
	if (motor_index == 0) PWM0->MR1 = T_ON;
	if (motor_index == 1) PWM0->MR2 = T_ON;
	if (motor_index == 2) PWM0->MR3 = T_ON;
	if (motor_index == 3) PWM0->MR4 = T_ON;
	PWM0->LER |= 1 << (motor_index+1);
}
