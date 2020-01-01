#include "Ultrasonic.h"

static uint32_t ultrasonicSensorRisingCaptureTime;
volatile uint8_t ultrasonicSensorNewDataAvailable = 0;
volatile double ultrasonicSensorLastMeasurementCM;

static void Ultrasonic_Trigger_Timer_Init() {
	uint32_t temp;
	//Turn on Timer2.
	PCONP |= 1<<22;
	//Change the mode of Timer2 to Timer Mode.
	TIMER2->CTCR &= ~3;
	TIMER2->TCR &= ~(1 << 0);
	
	TIMER2->TCR |= (1 << 1);
	
	//Change PR Register value for 1 microsecond incrementing
	TIMER2->PR = PERIPHERAL_CLOCK_FREQUENCY/1000000-1;
	//Write the Correct Configuration for EMR (LOW output value of Trigger Pin when match occurs and Initial value is LOW)
	temp = TIMER2->EMR;
	temp &= ~(1<<3);
	temp &= ~(3<<10);
	temp |= 1<<10;
	TIMER2->EMR = temp;
	NVIC_EnableIRQ(TIMER2_IRQn);
	
	NVIC_SetPriority(TIMER2_IRQn,5);
	
	NVIC_ClearPendingIRQ(TIMER2_IRQn);
}

static void Ultrasonic_Capture_Timer_Init() {
	//Turn on Timer3
	PCONP |= 1<<23;
	//Change the mode of Timer3 to Timer Mode
	TIMER3->CTCR &= ~3;
	TIMER3->TCR &= ~(1 << 0);
	
	TIMER3->TCR |= (1 << 1);
	
	//Change PR Register value for 1 microsecond incrementing
	TIMER3->PR = PERIPHERAL_CLOCK_FREQUENCY/1000000-1;
	//Change CCR value for getting Interrupt when Rising Edge Occur for CAP 1
	TIMER3->CCR = (1<<3)|(1<<5);
	TIMER3->TCR &= ~(1 << 1);
	
	TIMER3->TCR |= (1 << 0);

	NVIC_EnableIRQ(TIMER3_IRQn);
}

static void Ultrasonic_Start_Trigger_Timer() {
	//Change output value of Trigger Pin as HIGH
	TIMER2->EMR |= 1<<3;
	//Give correct value to corresponding MR Register for 10 microsecond
	TIMER2->MR3 = 10 + TIMER2->TC;
	//Enable interrupt for MR3 register, if MR3 register matches the TC.
	TIMER2->MCR |= 1<<9;
	//Remove the reset on counters of Timer2.
	TIMER2->TCR &= ~(1<<1);
	//Enable Timer Counter and Prescale Counter for counting.
	TIMER2->TCR |= 1;
}

void Ultrasonic_Init() {
	IOCON_TRIGGER |= 0x03;
	IOCON_ECHO |= 0x03;
	Ultrasonic_Trigger_Timer_Init();
	Ultrasonic_Capture_Timer_Init();
	Ultrasonic_Start_Trigger_Timer();
}

void TIMER2_IRQHandler() {
	static uint8_t ultrasonicSensorTriggerStart = 0;
	
	//Write HIGH bit value to IR Register for Corresponding Interrupt
	TIMER2->IR = 1<<3;
	if(ultrasonicSensorTriggerStart == 0) {
		//Change MR3 Register Value for Suggested Waiting
		TIMER2->MR3 = 60000 + TIMER2->TC;
		ultrasonicSensorTriggerStart = 1;
	}
	else {
		TIMER2->EMR |= (1 << 3);
		TIMER2->MR3 = 10 + TIMER2->TC;
		
		ultrasonicSensorTriggerStart = 0;
	}
}

void TIMER3_IRQHandler() {
	static uint8_t ultrasonicSensorCaptureRisingEdge = 1;
	
	TIMER3->IR = 1 << 5;
	
	if(ultrasonicSensorCaptureRisingEdge == 1) {
		ultrasonicSensorRisingCaptureTime = TIMER3->CR1;
		
		LPC_TIM3->CCR = (1 << 4) | (1 << 5);
		ultrasonicSensorCaptureRisingEdge = 0;
	}
	else {
		const uint32_t ultrasonicSensorFallingCaptureTime = TIMER3->CR1;
		ultrasonicSensorLastMeasurementCM = (ultrasonicSensorFallingCaptureTime-ultrasonicSensorRisingCaptureTime)/58.0;
		ultrasonicSensorNewDataAvailable = 1;
		
		LPC_TIM3->CCR = (1 << 3) | (1 << 5);
		ultrasonicSensorCaptureRisingEdge = 1;
	}
}
