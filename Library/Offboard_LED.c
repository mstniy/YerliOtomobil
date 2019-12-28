#include "Offboard_LED.h"

#define FL_LED_PORT                   GPIO_PORT2
#define FL_LED_MASK                   ((uint32_t) 1 << 10)
#define FL_LED_IOCON                  (*(volatile uint32_t*)0x4002C128)

#define FR_LED_PORT                   GPIO_PORT1
#define FR_LED_MASK                   ((uint32_t) 1 << 12)        
#define FR_LED_IOCON                  (*(volatile uint32_t*)0x4002C0B0)

#define BL_LED_PORT			              GPIO_PORT1
#define BL_LED_MASK			              ((uint32_t) 1 << 11)
#define BL_LED_IOCON                  (*(volatile uint32_t*)0x4002C0AC)

#define BR_LED_PORT                   GPIO_PORT1
#define BR_LED_MASK	                  ((uint32_t) 1 << 7)
#define BR_LED_IOCON                  (*(volatile uint32_t*)0x4002C09C)

void Offboard_LEDs_Init() {
	GPIO_PORT1->DIR |= FR_LED_MASK | BL_LED_MASK | BR_LED_MASK;
	GPIO_PORT2->DIR |= FL_LED_MASK;
	Offboard_LEDs_Set_State(0,0,0,0);
}

void Offboard_LEDs_Set_State(int fl_on, int fr_on, int bl_on, int br_on) {
	uint32_t port1shadow = GPIO_PORT1->PIN;
	
	if (fl_on==1)   GPIO_PORT2->PIN |=  FL_LED_MASK;
	if (fl_on==0)   GPIO_PORT2->PIN &= ~FL_LED_MASK;
	
	if (fr_on==1)   port1shadow     |=  FR_LED_MASK;
	if (fr_on==0)   port1shadow     &= ~FR_LED_MASK;
	
	if (bl_on==1)   port1shadow     |=  BL_LED_MASK;
	if (bl_on==0)   port1shadow     &= ~BL_LED_MASK;
	
	if (br_on==1)   port1shadow     |=  BR_LED_MASK;
	if (br_on==0)   port1shadow     &= ~BR_LED_MASK;
	
	GPIO_PORT1->PIN = port1shadow;
}
