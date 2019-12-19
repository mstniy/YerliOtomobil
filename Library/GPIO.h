#ifndef GPIO_H
#define GPIO_H

#include "LPC407x_8x_177x_8x.h"

typedef struct
{
  volatile	uint32_t DIR;
						uint32_t RESERVED0[3];
  volatile	uint32_t MASK;
  volatile	uint32_t PIN;
  volatile	uint32_t SET;
  volatile  uint32_t CLR;
} GPIO_TypeDef;

typedef enum {
	INPUT = 0,
	OUTPUT = 1
} GPIO_Direction;

typedef enum {
	LOW = 0,
	HIGH = 1
} GPIO_Value;


#define GPIO_BASE	0x20098000

#define GPIO_PORT0	((GPIO_TypeDef*) (GPIO_BASE + 0x000))
#define GPIO_PORT1	((GPIO_TypeDef*) (GPIO_BASE + 0x020))
#define GPIO_PORT2	((GPIO_TypeDef*) (GPIO_BASE + 0x040))
#define GPIO_PORT3	((GPIO_TypeDef*) (GPIO_BASE + 0x060))
#define GPIO_PORT4	((GPIO_TypeDef*) (GPIO_BASE + 0x080))
#define GPIO_PORT5	((GPIO_TypeDef*) (GPIO_BASE + 0x0A0))

volatile uint32_t* getIOCON(int a, int b);

void GPIO_DIR_Write(GPIO_TypeDef* PORT,uint32_t MASK,uint8_t value);
void GPIO_PIN_Write(GPIO_TypeDef* PORT,uint32_t MASK,uint8_t value);
uint32_t GPIO_PIN_Read(GPIO_TypeDef* PORT,uint32_t MASK);

#endif
