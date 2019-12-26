#include "SpinCounter.h"

#include "LPC407x_8x_177x_8x.h"

static uint32_t count = 0;

void spin_counter_init() {
	LPC_IOCON->P0_8 &= ~7;
	
	LPC_GPIO0->DIR &= ~(1 << 8);
	LPC_GPIOINT->IO0IntEnR = (1 << 8);
	LPC_GPIOINT->IO0IntEnF = 0;
	
	NVIC_EnableIRQ(GPIO_IRQn);
	NVIC_ClearPendingIRQ(GPIO_IRQn);
}

void GPIO_IRQHandler() {
	LPC_GPIOINT->IO0IntClr = (1 << 8);
	count++;
}

int spin_counter_get_count(){
	return count;
}
