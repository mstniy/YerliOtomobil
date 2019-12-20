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

#define	POTENTIOMETER_PIN_IOCON_ADDRESS	0x4002C05C //IOCON_P0_23 P0[23] adc0[0]
#define POTENTIOMETER_PIN_IOCON	*((volatile uint32_t*)(ANALOG_PIN_IOCON_ADDRESS))

//Write the max value of ADC.
#define ADC_MAX_VALUE 0x00000FFF

#define ADC_CLKDIV (PERIPHERAL_CLOCK_FREQUENCY/ADC_CLOCK_FREQUENCY)-1

#define ADC_BASE	0x40034000
#define ADC	((ADC_TypeDef*) ADC_BASE)

#define ADC_LEFT_CHANNEL 2
#define ADC_RIGHT_CHANNEL 3
#define ADC_POTENTIOMETER_CHANNEL 0

extern uint32_t ADC_Last[];
extern uint8_t ADC_New_Data_Available[];

void ADC_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
uint32_t ADC_GetLastValue(void);

#endif
