#ifndef PLL_H
#define PLL_H

#include "LPC407x_8x_177x_8x.h"

#define PLLCON_ADDRESS			0x400FC080
#define PLLCFG_ADDRESS			0x400FC084
#define PLLSTAT_ADDRESS 		0x400FC088
#define PLLFEED_ADDRESS			0x400FC08C

#define PLLCON					*((volatile uint32_t*)(PLLCON_ADDRESS))
#define PLLCFG					*((volatile uint32_t*)(PLLCFG_ADDRESS))
#define PLLSTAT					*((volatile uint32_t*)(PLLSTAT_ADDRESS))
#define PLLFEED 				*((volatile uint32_t*)(PLLFEED_ADDRESS))

#define CLKSRCSEL_ADDRESS		0x400FC10C
#define CCLKSEL_ADDRESS			0x400FC104
#define PCLKSEL_ADDRESS		    0x400FC1A8

#define CLKSRCSEL	            *((volatile uint32_t*)(CLKSRCSEL_ADDRESS))
#define CCLKSEL		            *((volatile uint32_t*)(CCLKSEL_ADDRESS))
#define PCLKSEL	                *((volatile uint32_t*)(PCLKSEL_ADDRESS))

void PLL_Init(void);
void PLL_Disable(void);
void PLL_Feed(void);
void PLL_Change_Source(void);
void PLL_Change_Frequency(void);
void PLL_Enable(void);
void PLL_Check_State(void);
void PLL_Start(void);
void PLL_Change_Configuration(void);

#endif
