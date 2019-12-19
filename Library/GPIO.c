#include "GPIO.h"

void GPIO_DIR_Write(GPIO_TypeDef* PORT,uint32_t MASK,uint8_t value) {
	if(value == 0) {
		PORT->DIR &= ~MASK;
	}
	else {
		PORT->DIR |= MASK;
	}
}

void GPIO_PIN_Write(GPIO_TypeDef* PORT,uint32_t MASK,uint8_t value) {
	if(value == 0) {
		PORT->PIN &= ~MASK;
	}
	else {
		PORT->PIN |= MASK;
	}
}

uint32_t GPIO_PIN_Read(GPIO_TypeDef* PORT,uint32_t MASK) {
	return PORT->PIN &= MASK;
}

volatile uint32_t* getIOCON(int a, int b){
	uint32_t offset = (a*32+b)*4;
	return (volatile uint32_t*)(0x4002C000+offset);
}
volatile GPIO_TypeDef* getGPIO(int a){
	return &((volatile GPIO_TypeDef*)0x20098000)[a];
}
