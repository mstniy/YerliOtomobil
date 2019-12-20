#include "Ultrasonic.h"

uint32_t ultrasonicSensorRisingCaptureTime;
uint32_t ultrasonicSensorFallingCaptureTime;

int8_t ultrasonicSensorNewDataAvailable = 0;

static void Ultrasonic_Trigger_Timer_Init(void);
static void Ultrasonic_Capture_Timer_Init(void);
static void Ultrasonic_Start_Trigger_Timer(void);

void Ultrasonic_Start() {
	ultrasonicSensorNewDataAvailable = 0;
	
	IOCON_ULTRASONIC_ECHO |= 0x03;
	IOCON_ULTRASONIC_TRIGGER |= 0x03;
	Ultrasonic_Trigger_Timer_Init();
	Ultrasonic_Capture_Timer_Init();
	Ultrasonic_Start_Trigger_Timer();
}

static void Ultrasonic_Trigger_Timer_Init() {
	uint32_t temp;
	PCONP |= (1<<22); //Turn on Timer2.
	TIMER2->CTCR &= ~3; //Change to Timer Mode.
	TIMER2->TCR &= ~(1 << 0); // Disable counters
	TIMER2->TCR |= (1 << 1); // Reset the counters on the next clock cycle
	TIMER2->PR = PERIPHERAL_CLOCK_FREQUENCY / 1000000 - 1; //1 microsecond
	
	temp = TIMER2->EMR;
	temp &= ~(1<<2); // LOW initial value for EM2
	temp |= (1<<8); // Clear EM2 when TC matches MR2
	temp &= ~(1<<9); // Continuation of the previous line
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
	TIMER3->PR = PERIPHERAL_CLOCK_FREQUENCY / 1000000 - 1; //1 microsecond
	
	TIMER3->CCR = 1 | (1<<2); // Generate an interrupt for rising edges
	
	TIMER3->TCR &= ~(1 << 1); // Remove the reset on the counters
	TIMER3->TCR |= (1 << 0); // Enable the counters

	NVIC_EnableIRQ(TIMER3_IRQn);
	NVIC_SetPriority(TIMER3_IRQn,5);
	NVIC_ClearPendingIRQ(TIMER3_IRQn);
}

static void Ultrasonic_Start_Trigger_Timer() {
	TIMER2->EMR |= 1<<2; // Set the trigger pin to high
	TIMER2->MR2 = 50 + TIMER2->TC; // Instruct the timer to wait for 10 us
	TIMER2->MCR |= (1<<6); //Generate an interrupt when MR2 matches TC.
	TIMER2->TCR &= ~(1 << 1); // Remove the reset on the counters
	TIMER2->TCR |= (1 << 0); // Enable the counters
}

void TIMER2_IRQHandler() {
	static int ultrasonicSensorTriggerStart=0;
	TIMER2->IR = (1<<2); // Clear IR
	if(ultrasonicSensorTriggerStart == 0) {
		TIMER2->MR2 = 1000000 + TIMER2->TC; // Wait for 60ms
		ultrasonicSensorTriggerStart = 1;
	}
	else {
		TIMER2->EMR |= (1 << 2); // Set the trigger bit to high. It will be set to low when the timer expires.
		TIMER2->MR2 = 50 + TIMER2->TC;
		
		ultrasonicSensorTriggerStart = 0;
	}
}

void TIMER3_IRQHandler() {
	uint32_t temp;
	static int ultrasonicSensorCaptureRisingEdge=1;
	TIMER3->IR = (1 << 4);
	
	if(ultrasonicSensorCaptureRisingEdge == 1) {
		ultrasonicSensorRisingCaptureTime = TIMER3->CR0;
		TIMER3->CCR = (1<<1) | (1<<2);
		
		ultrasonicSensorCaptureRisingEdge = 0;
	}
	else {
		ultrasonicSensorFallingCaptureTime = TIMER3->CR0;
		TIMER3->CCR = 1 | (1<<2);
		
		ultrasonicSensorCaptureRisingEdge = 1;
		ultrasonicSensorNewDataAvailable = 1;
	}
}

