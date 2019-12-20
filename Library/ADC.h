#ifndef ADC_H
#define ADC_H

#include "LPC407x_8x_177x_8x.h"

#include "SystemStructures.h"

typedef struct {
  volatile	uint32_t CR;
  volatile	uint32_t GDR;
						uint32_t RESERVED0;
  volatile	uint32_t INTEN;
  volatile	uint32_t DR[8];
  volatile	uint32_t STAT;
  volatile	uint32_t TRM;
} ADC_TypeDef;

#define ADC_CLOCK_FREQUENCY 1000000


//Write the IOCON address of Analog Pin
#define	LDR_LEFT_PIN_IOCON_ADDRESS	0x4002C064	//p0_25; ADC0[2]
#define LDR_LEFT_PIN_IOCON	*((volatile uint32_t*)(ANALOG_PIN_IOCON_ADDRESS))

#define	LDR_RIGHT_PIN_IOCON_ADDRESS	0x4002C068 //p0_26; ADC0[3]
#define LDR_RIGHT_PIN_IOCON	*((volatile uint32_t*)(ANALOG_PIN_IOCON_ADDRESS))

//Write the max value of ADC.
#define ADC_MAX_VALUE 0x00000FFF

//Define a ADC_CLKDIV variable for given ADC_CLOCK_FREQUENCY.
//This ASSUMES 60 MHz PCLK frequency.
#define ADC_CLKDIV 59

#define ADC_BASE	0x40034000
#define ADC_LEFT	((ADC_TypeDef*) ADC_BASE)
#define ADC_RIGHT	((ADC_TypeDef*) ADC_BASE)

extern uint32_t ADC_Last[];
extern uint8_t ADC_New_Data_Available[];

void ADC_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
uint32_t ADC_GetLastValue(void);

#endif
