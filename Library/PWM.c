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
	
	PCONP |= (1 << 5/* | 1 << 6*/); // Turn PWM0 on
	
	PWM0->PCR |= (1<<10)|(1<<11); // Enable PWM output on PWM[2,3]
	
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
	
	PWM_Write(0, 0);
	PWM_Write(1, 0);
}

void PWM_Write(int motor_index, double T_ON) {	
	uint32_t new_mr;
	if(T_ON > 1) {
		T_ON = 1;
	}
	
	new_mr = (uint32_t)(PWM0->MR0 * T_ON);
	
	if (new_mr == PWM0->MR0) {
		new_mr++;
	}
	if (motor_index == 0) PWM0->MR2 = new_mr;
	if (motor_index == 1) PWM0->MR3 = new_mr;
	PWM0->LER |= 1 << (motor_index+2);
}
