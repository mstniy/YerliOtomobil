#ifndef LED_H
#define LED_H

#include "GPIO.h"

#define LEFT_LED_PORT                   GPIO_PORT2
#define LEFT_LED_MASK                   ((uint32_t) 1 << 10)
#define LEFT_LED_IOCON_ADDRESS          0x4002C128
#define LEFT_LED_IOCON                  *((volatile uint32_t*)(LEFT_LED_IOCON_ADDRESS))

#define RIGHT_LED_PORT                  GPIO_PORT1
#define RIGHT_LED_MASK                  ((uint32_t) 1 << 12)
#define RIGHT_LED_IOCON_ADDRESS         0x4002C0B0
#define RIGHT_LED_IOCON                 *((volatile uint32_t*)(RIGHT_LED_IOCON_ADDRESS))

#define FRONT_LED_PORT			        GPIO_PORT1
#define FRONT_LED_MASK			        ((uint32_t) 1 << 11)
#define FRONT_LED_IOCON_ADDRESS         0x4002C0AC
#define FRONT_LED_IOCON                 *((volatile uint32_t*)(FRONT_LED_IOCON_ADDRESS))

#define BACK_LED_PORT                   GPIO_PORT1
#define BACK_LED_MASK	                ((uint32_t) 1 << 7)
#define BACK_LED_IOCON_ADDRESS          0x4002C09C
#define BACK_LED_IOCON                  *((volatile uint32_t*)(BACK_LED_IOCON_ADDRESS))

void LEDs_Init(void);

void LEDs_ON(void);
void LEFT_LED_On(void);
void SAG_On(void);
void FRONT_LED_On(void);
void BACK_LED_On(void);

void LEDs_OFF(void);
void LEFT_LED_Off(void);
void RIGHT_LED_Off(void);
void FRONT_LED_Off(void);
void BACK_LED_Off(void);

#endif
