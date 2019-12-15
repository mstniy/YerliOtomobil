#ifndef PWM_H
#define PWM_H

#include "LPC407x_8x_177x_8x.h"

#include "SystemStructures.h"

typedef struct
{
  volatile	uint32_t IR;
  volatile	uint32_t TCR;
  volatile	uint32_t TC;
  volatile	uint32_t PR;
  volatile	uint32_t PC;
  volatile	uint32_t MCR;
  volatile	uint32_t MR0;
  volatile	uint32_t MR1;
  volatile	uint32_t MR2;
  volatile	uint32_t MR3;
  volatile	uint32_t CCR;
  volatile  uint32_t CR0;
  volatile  uint32_t CR1;
  volatile  uint32_t CR2;
  volatile  uint32_t CR3;
						uint32_t RESERVED0;
  volatile	uint32_t MR4;
  volatile	uint32_t MR5;
  volatile	uint32_t MR6;
  volatile	uint32_t PCR;
  volatile	uint32_t LER;
						uint32_t RESERVED1[7];
  volatile	uint32_t CTCR;
} PWM_TypeDef;

#define IOCON_MOTOR_0_PWM	((volatile uint32_t*)0x4002C088)
#define IOCON_MOTOR_1_PWM	((volatile uint32_t*)0x4002C08C)
#define IOCON_MOTOR_2_PWM	((volatile uint32_t*)0x4002C094)
#define IOCON_MOTOR_3_PWM	((volatile uint32_t*)0x4002C098)

#define PWM0	((PWM_TypeDef*) 0x40014000)
#define PWM1	((PWM_TypeDef*) 0x40018000)

void PWM_Init(void);
void PWM_Write(int motor_index, double T_ON);

#endif
