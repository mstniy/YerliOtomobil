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

static void LEFT_LED_Init() {
	LEFT_LED_IOCON &= ~7;
	GPIO_DIR_Write(LEFT_LED_PORT,LEFT_LED_MASK,OUTPUT);
}

static void RIGHT_LED_Init() {
	RIGHT_LED_IOCON &= ~7;
	GPIO_DIR_Write(RIGHT_LED_PORT,RIGHT_LED_MASK,OUTPUT);
}

static void FRONT_LED_Init() {
	FRONT_LED_IOCON &= ~7;
	GPIO_DIR_Write(FRONT_LED_PORT,FRONT_LED_MASK,OUTPUT);
}

static void BACK_LED_Init() {
	BACK_LED_IOCON &= ~7;
	GPIO_DIR_Write(BACK_LED_PORT,BACK_LED_MASK,OUTPUT);
}

void Offboard_LEDs_Init() {
	RIGHT_LED_Init();
	LEFT_LED_Init();
	FRONT_LED_Init();
	BACK_LED_Init();
	Offboard_LEDs_OFF();
}

void RIGHT_LED_On() {
	GPIO_PIN_Write(RIGHT_LED_PORT,RIGHT_LED_MASK,HIGH);
}

void LEFT_LED_On() {
	GPIO_PIN_Write(LEFT_LED_PORT,LEFT_LED_MASK,HIGH);
}

void FRONT_LED_On() {
	GPIO_PIN_Write(FRONT_LED_PORT,FRONT_LED_MASK,HIGH);
}

void BACK_LED_On() {
	GPIO_PIN_Write(BACK_LED_PORT,BACK_LED_MASK,HIGH);
}

void Offboard_LEDs_ON() {
	RIGHT_LED_On();
	LEFT_LED_On();
	FRONT_LED_On();
	BACK_LED_On();
}

void RIGHT_LED_Off() {
	GPIO_PIN_Write(RIGHT_LED_PORT,RIGHT_LED_MASK,LOW);
}

void LEFT_LED_Off() {
	GPIO_PIN_Write(LEFT_LED_PORT,LEFT_LED_MASK,LOW);
}

void FRONT_LED_Off() {
	GPIO_PIN_Write(FRONT_LED_PORT,FRONT_LED_MASK,LOW);
}

void BACK_LED_Off() {
	GPIO_PIN_Write(BACK_LED_PORT,BACK_LED_MASK,LOW);
}

void Offboard_LEDs_OFF() {
	RIGHT_LED_Off();
	LEFT_LED_Off();
	FRONT_LED_Off();
	BACK_LED_Off();
}
