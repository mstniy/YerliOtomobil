#include "Ultrasonic.h"

uint32_t ultrasonicSensorRisingCaptureTime[2];
uint32_t ultrasonicSensorFallingCaptureTime[2];

int8_t ultrasonicSensorNewDataAvailable[2] = {0};

static void Ultrasonic_Trigger_Timer_Init(void);
static void Ultrasonic_Capture_Timer_Init(void);

void Ultrasonic_Init() {
	IOCON_ULTRASONIC_0_ECHO |= 0x03;
	IOCON_ULTRASONIC_0_TRIGGER |= 0x03;
	IOCON_ULTRASONIC_1_ECHO |= 0x03;
	IOCON_ULTRASONIC_1_TRIGGER |= 0x03;
	Ultrasonic_Trigger_Timer_Init();
	Ultrasonic_Capture_Timer_Init();
}

static void Ultrasonic_Trigger_Timer_Init() {
	uint32_t temp;
	PCONP |= (1<<22); //Turn on Timer2.
	TIMER2->CTCR &= ~3; //Change to Timer Mode.
	TIMER2->TCR &= ~(1 << 0); // Disable counters
	TIMER2->TCR |= (1 << 1); // Reset the counters on the next clock cycle
	TIMER2->PR = PERIPHERAL_CLOCK_FREQUENCY / 1000000; //1 microsecond
	
	temp = TIMER2->EMR;
	temp &= ~((1<<2)|(1<<3)); // LOW initial value for EM2 and EM3
	temp |= (1<<8)|(1<<10); // Clear EM2 and EM3 when TC matches MR2/3
	temp &= ~((1<<9)|(1<<11)); // Continuation of the previous line
	TIMER2->EMR = temp;
	
	NVIC_EnableIRQ(TIMER2_IRQn);
	NVIC_SetPriority(TIMER2_IRQn,5);
	NVIC_ClearPendingIRQ(TIMER2_IRQn);
}

static void Ultrasonic_Capture_Timer_Init() {
	PCONP |= (1<<23); //Turn on Timer3
	TIMER3->CTCR &= ~3; //Change to Timer Mode.
	TIMER3->TCR &= ~(1 << 0); // Disable counters
	TIMER3->TCR |= (1 << 1); // Reset the counters on the next clock cycle
	TIMER3->PR = PERIPHERAL_CLOCK_FREQUENCY / 1000000; //1 microsecond
	
	TIMER3->CCR |= (1<<0)|(1<<2)|(1<<3)|(1<<5); // Generate an interrupt for rising edges on any of the two capture pins
	
	TIMER3->TCR &= ~(1 << 1); // Remove the reset on the counters
	TIMER3->TCR |= (1 << 0); // Enable the counters

	NVIC_EnableIRQ(TIMER3_IRQn);
}

void Ultrasonic_Start_Trigger_Timer() {
	TIMER2->EMR |= 1<<2; // Set the trigger pin to high
	TIMER2->MR2 = 10 + TIMER2->TC; // Instruct the timer to wait for 10 ms
	TIMER2->MCR |= (1<<6); //Generate an interrupt when MR2 matches TC.
	TIMER2->TCR &= ~(1 << 1); // Remove the reset on the counters
	TIMER2->TCR |= (1 << 0); // Enable the counters
}

void TIMER2_IRQHandler() {
	uint32_t temp;
	static int ultrasonicSensorTriggerStart=0;
	static int measuringUltrasonicSensorID=1; // Start by measuring the first sensor
	TIMER2->IR = (1<<2)|(1<<3); // Clear IR
	if(ultrasonicSensorTriggerStart == 0) {
		TIMER2->MR2 = 60 + TIMER2->TC; // Wait for 60ms
		temp = TIMER2->MCR;
		temp |= (1<<6); //Generate an interrupt when MR2 matches TC.
		temp &= ~(1<<9); // Disable the interrupt for MR3
		TIMER2->MCR = temp;
		ultrasonicSensorTriggerStart = 1;
		measuringUltrasonicSensorID = !measuringUltrasonicSensorID;
	}
	else {
		if (measuringUltrasonicSensorID == 0) {
			TIMER2->EMR |= (1 << 2);
			TIMER2->MR2 = 10 + TIMER2->TC;
			temp = TIMER2->MCR;
			temp |= (1<<6); //Generate an interrupt when MR2 matches TC.
			temp &= ~(1<<9); // Disable the interrupt for MR3
			TIMER2->MCR = temp;
		}
		else {
			TIMER2->EMR |= (1 << 3);
			TIMER2->MR3 = 10 + TIMER2->TC;
			temp = TIMER2->MCR;
			temp |= (1<<9); //Generate an interrupt when MR3 matches TC.
			temp &= ~(1<<6); // Disable the interrupt for MR2
			TIMER2->MCR = temp;
		}
		
		ultrasonicSensorTriggerStart = 0;
	}
}

void TIMER3_IRQHandler() {
	uint32_t temp;
	int sensorID;
	static int ultrasonicSensorCaptureRisingEdge=1; // Since we never measure both of the sensor simustaneously, we have only one of this variable.
	temp = TIMER3->IR;
	TIMER3->IR = (1 << 4)|(1 << 5);
	if (temp & (1<<4))
		sensorID = 0;
	else
		sensorID = 1;
	
	if(ultrasonicSensorCaptureRisingEdge == 1) {
		if (sensorID == 0) {
			ultrasonicSensorRisingCaptureTime[sensorID] = TIMER3->CR0;
			temp = LPC_TIM3->CCR;
			temp |= (1 << 1); // Capture falling edge on CAP0
			temp &= ~(1<<0);
			LPC_TIM3->CCR = temp;
		}
		else {
			ultrasonicSensorRisingCaptureTime[sensorID] = TIMER3->CR1;
			temp = LPC_TIM3->CCR;
			temp |= (1 << 4); // Capture falling edge on CAP1
			temp &= ~(1<<3);
			LPC_TIM3->CCR = temp;
		}
		
		ultrasonicSensorCaptureRisingEdge = 0;
	}
	else {
		if (sensorID == 0) {
			ultrasonicSensorFallingCaptureTime[sensorID] = TIMER3->CR0;
			temp = LPC_TIM3->CCR;
			temp |= (1 << 0); // Capture rising edge on CAP0
			temp &= ~(1<<1);
			LPC_TIM3->CCR = temp;
		}
		else {
			ultrasonicSensorFallingCaptureTime[sensorID] = TIMER3->CR1;
			temp = LPC_TIM3->CCR;
			temp |= (1 << 3); // Capture rising edge on CAP1
			temp &= ~(1<<4);
			LPC_TIM3->CCR = temp;
		}
		ultrasonicSensorNewDataAvailable[sensorID] = 1;
		
		ultrasonicSensorCaptureRisingEdge = 1;
	}
}

