#include "ADC.h"

uint32_t ADC_Last[3];
uint8_t ADC_New_Data_Available[3] = {0};


#define ADC_LEFT_CHANNEL 2
#define ADC_RIGHT_CHANNEL 3
#define ADC_POTENTIOMETER_CHANNEL 4

static int channel_map[3] = {ADC_LEFT_CHANNEL, ADC_RIGHT_CHANNEL, ADC_POTENTIOMETER_CHANNEL};
static int current_channel = LEFT_LDR;

static int toLightLevel(int meas) {
	double res = -0.35275*meas+1340.45;
	if (res < 0)
		res = 0;
	if (res > 1023)
		res = 1023;
	return res;
}

static void ADC_Timer0_Init() {
	T0_M1_PIN_IOCON &= ~(0x07);
	T0_M1_PIN_IOCON |= 0x03;
	PCONP |= 1 << 1;
	TIMER0->CTCR = 0x0;
	TIMER0->TCR &= ~(1 << 0);
	TIMER0->TCR |= (1 << 1);
	//Make it increment every 1 microseconds
	TIMER0->PR = PERIPHERAL_CLOCK_FREQUENCY / 1000000 - 1;
	//Toggle the corresponding External Match bit/output when MR1 matches with TC.
	TIMER0->EMR |= (1<<1);
	TIMER0->EMR |= (3<<6);
	//Reset the TC value whenever MR1 matches it
	TIMER0->MCR |= (1<<4);
	//Configure MR1 as ADC will start every 30 milliseconds (Do not forget you configured ADC when rising edge occurs on TIMER 0 MATCH 1) 
	TIMER0->MR1 = 15000;
	TIMER0->TCR &= ~(1 << 1);
	TIMER0->TCR |= (1 << 0);
}

void ADC_Init() {
	ADC_Timer0_Init();
	//Change the function value of pin to ADC.
	LDR_LEFT_PIN_IOCON &= ~7;
	LDR_LEFT_PIN_IOCON |= 1;

	LDR_RIGHT_PIN_IOCON &= ~7;
	LDR_RIGHT_PIN_IOCON |= 1;

	POTENTIOMETER_PIN_IOCON &= ~7;
	POTENTIOMETER_PIN_IOCON |= 3;

	//Change the mode value of pin to mode which should be selected if Analog mode is used.
	LDR_LEFT_PIN_IOCON &= ~(3<<3);
	LDR_RIGHT_PIN_IOCON &= ~(3<<3);
	POTENTIOMETER_PIN_IOCON &= ~(3<<3);

	//Change Analog/Digital mode of pin to Analog.
	LDR_LEFT_PIN_IOCON &= ~(1<<7);
	LDR_RIGHT_PIN_IOCON &= ~(1<<7);
	POTENTIOMETER_PIN_IOCON &= ~(1<<7);
	
	//Turn on ADC.
	PCONP |= (1<<12);
	
	//Set the CLKDIV
	ADC->CR &= ~(0xff<<8);
	ADC->CR |= (ADC_CLKDIV)<<8;
	
	//Make the A/D converter operational
	ADC->CR |= (1<<21);
	
	//Software controlled conversion
	ADC->CR &= ~(1<<16);
	
	//Set channel bits CR_SEL
	ADC->CR &= ~(0xff);
	ADC->CR |= (1<<channel_map[current_channel]);
				
	//Enable interrupt with current Channel
	ADC->INTEN &= ~0xff;
	ADC->INTEN |= (1<<channel_map[current_channel]);

	//Enable ADC_IRQn (Interrupt Request).
	NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_Start () {
	//Write a code for starting A/D conversion on a rising edge on the TIMER 0 MATCH 1.
	ADC->CR &= ~(7<<24);
	ADC->CR |= (4<<24);
	ADC->CR &= ~(1<<27);
}

uint32_t ADC_GetLastValueOfLeftLDR() {
	ADC_New_Data_Available[LEFT_LDR] = 0;
	return ADC_Last[LEFT_LDR];
}

uint32_t ADC_GetLastValueOfRightLDR() {
	ADC_New_Data_Available[RIGHT_LDR] = 0;
	return ADC_Last[RIGHT_LDR];
}

uint32_t ADC_GetLastValueOfPotentiometer() {
	ADC_New_Data_Available[POTENTIOMETER] = 0;
	return ADC_Last[POTENTIOMETER];
}

void ADC_IRQHandler() {
	uint32_t temp;
	ADC->GDR &= ~((uint32_t)1 << 31);
	switch (current_channel) {
	    case LEFT_LDR:
	    	ADC_Last[LEFT_LDR] = toLightLevel((ADC->DR[ADC_LEFT_CHANNEL]>>4)&0xfff);
	    	ADC_New_Data_Available[LEFT_LDR] = 1;
	    	current_channel = RIGHT_LDR;
	      	break;
	    case RIGHT_LDR:
	     	ADC_Last[RIGHT_LDR] = toLightLevel((ADC->DR[ADC_RIGHT_CHANNEL]>>4)&0xfff);
	    	ADC_New_Data_Available[RIGHT_LDR] = 1;
	    	current_channel = POTENTIOMETER;
	      	break;
	    case POTENTIOMETER:
			  ADC_Last[POTENTIOMETER] = (ADC->DR[ADC_POTENTIOMETER_CHANNEL]>>4)&0xfff;
	    	ADC_New_Data_Available[POTENTIOMETER] = 1;
	    	current_channel = LEFT_LDR;
	      	break;
	}
	//Set channel bits of CR_SEL
	temp = ADC->CR;
	temp &= ~(0xff);
	temp |= (1<<channel_map[current_channel]);
	ADC->CR = temp;
				
	//Enable interrupt with current Channel
	temp = ADC->INTEN;
	temp &= ~0xff;
	temp |= (1<<channel_map[current_channel]);
	ADC->INTEN = temp;
}
