#include "LED.h"

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

void LED_Init() {
	RIGHT_LED_Init();
	LEFT_LED_Init();
	FRONT_LED_Init();
	BACK_LED_Init();
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

void LEDs_ON() {
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

void LEDs_OFF() {
	RIGHT_LED_Off();
	LEFT_LED_Off();
	FRONT_LED_Off();
	BACK_LED_Off();
}
