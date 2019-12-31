#include "ControllerLoop.h"

static Controller_Callback controller_callback;

static void Controller_Loop_Timer_Init() {
	//Turn on Timer1.
	PCONP |= 1<<2;
	//Change to Timer Mode.
	TIMER1->CTCR &= ~3;
	TIMER1->TCR &= ~(1 << 0);
	
	TIMER1->TCR |= (1 << 1);
	
	//Change PR Register value for 1 millisecond incrementing
	TIMER1->PR = PERIPHERAL_CLOCK_FREQUENCY/1000-1;
	NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_SetPriority(TIMER1_IRQn,5);
	NVIC_ClearPendingIRQ(TIMER1_IRQn);
}

static void Controller_Loop_Start_Timer() {
	TIMER1->MR0 = CONTROLLER_LOOP_PERIOD_MS + TIMER1->TC;
	//Raise an interrupt when MR0 matches TC.
	TIMER1->MCR = 1;
	//Remove the reset on counters.
	TIMER1->TCR &= ~(1<<1);
	//Enable Timer Counter and Prescale Counter for counting.
	TIMER1->TCR |= 1;
}

void Controller_Loop_Init(Controller_Callback cb) {
	controller_callback = cb;
	Controller_Loop_Timer_Init();
	Controller_Loop_Start_Timer();
}

uint32_t get_ms() {
	return TIMER1->TC;
}

void TIMER1_IRQHandler() {
	//Write HIGH bit value to IR Register for Corresponding Interrupt
	TIMER1->IR = 1;
	TIMER1->MR0 = TIMER1->TC + CONTROLLER_LOOP_PERIOD_MS;
	controller_callback();
}
