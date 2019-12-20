#include "Offboard_LED.h"

#define LEFT_LED_PORT                   GPIO_PORT2
#define LEFT_LED_MASK                   ((uint32_t) 1 << 10)
#define LEFT_LED_IOCON_ADDRESS          0x4002C128
#define LEFT_LED_IOCON                  *((volatile uint32_t*)(LEFT_LED_IOCON_ADDRESS))

#define RIGHT_LED_PORT                  GPIO_PORT1
#define RIGHT_LED_MASK                  ((uint32_t) 1 << 12)
#define RIGHT_LED_IOCON_ADDRESS         0x4002C0B0
#define RIGHT_LED_IOCON                 *((volatile uint32_t*)(RIGHT_LED_IOCON_ADDRESS))

#define FRONT_LED_PORT			            GPIO_PORT1
#define FRONT_LED_MASK			            ((uint32_t) 1 << 11)
#define FRONT_LED_IOCON_ADDRESS         0x4002C0AC
#define FRONT_LED_IOCON                 *((volatile uint32_t*)(FRONT_LED_IOCON_ADDRESS))

#define BACK_LED_PORT                   GPIO_PORT1
#define BACK_LED_MASK	                  ((uint32_t) 1 << 7)
#define BACK_LED_IOCON_ADDRESS          0x4002C09C
#define BACK_LED_IOCON                  *((volatile uint32_t*)(BACK_LED_IOCON_ADDRESS))

void Offboard_LEDs_Init() {
	GPIO_PORT1->DIR |= RIGHT_LED_MASK | FRONT_LED_MASK | BACK_LED_MASK;
	GPIO_PORT2->DIR |= LEFT_LED_MASK;
	Offboard_LEDs_Set_State(0,0,0,0);
}

void Offboard_LEDs_Set_State(int left_on, int right_on, int front_on, int back_on) {
	uint32_t port1shadow = GPIO_PORT1->PIN;
	
	if (left_on==1)    GPIO_PORT2->PIN |=  LEFT_LED_MASK;
	if (left_on==0)    GPIO_PORT2->PIN &= ~LEFT_LED_MASK;
	
	if (right_on==1)   port1shadow     |=  RIGHT_LED_MASK;
	if (right_on==0)   port1shadow     &= ~RIGHT_LED_MASK;
	
	if (front_on==1)   port1shadow     |=  FRONT_LED_MASK;
	if (front_on==0)   port1shadow     &= ~FRONT_LED_MASK;
	
	if (back_on==1)    port1shadow     |=  BACK_LED_MASK;
	if (back_on==0)    port1shadow     &= ~BACK_LED_MASK;
	
	GPIO_PORT1->PIN = port1shadow;
}
