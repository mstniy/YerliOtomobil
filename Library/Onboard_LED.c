#include "Onboard_LED.h"

#define LED1_PORT			GPIO_PORT1
#define LED1_MASK			((uint32_t) 1 << 18)

#define LED2_PORT			GPIO_PORT0
#define LED2_MASK			((uint32_t) 1 << 13)

#define LED3_PORT			GPIO_PORT1
#define LED3_MASK			((uint32_t) 1 << 13)

#define LED4_PORT			GPIO_PORT2
#define LED4_MASK			((uint32_t) 1 << 19)

void Onboard_LEDs_Init() {
	GPIO_PORT0->DIR |= LED2_MASK;
	GPIO_PORT1->DIR |= LED1_MASK | LED3_MASK;
	GPIO_PORT2->DIR |= LED4_MASK;
	Onboard_LEDs_Set_State(0,0,0,0);
}

void Onboard_LEDs_Set_State(int one_on, int two_on, int three_on, int four_on) {
	uint32_t port0shadow = GPIO_PORT0->PIN;
	uint32_t port1shadow = GPIO_PORT1->PIN;
	uint32_t port2shadow = GPIO_PORT2->PIN;
	
	if (one_on==0)   port1shadow |=  LED1_MASK; // LED1 and LED2 are connected in reverse
	if (one_on==1)   port1shadow &= ~LED1_MASK;
	
	if (two_on==0)   port0shadow |=  LED2_MASK;
	if (two_on==1)   port0shadow &= ~LED2_MASK;
	
	if (three_on==1) port1shadow |=  LED3_MASK;
	if (three_on==0)   port1shadow &= ~LED3_MASK;
	
	if (four_on==1)  port2shadow |=  LED4_MASK;
	if (four_on==0)   port2shadow &= ~LED4_MASK;
	
	GPIO_PORT0->PIN = port0shadow;
	GPIO_PORT1->PIN = port1shadow;
	GPIO_PORT2->PIN = port2shadow;
}
