#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "GPIO.h"
#include "Timer.h"
#include "SystemStructures.h"

#define IOCON_ULTRASONIC_0_TRIGGER	(*(volatile uint32_t*)0x4002C020)
#define IOCON_ULTRASONIC_0_ECHO	(*(volatile uint32_t*)0x4002C05C)
#define IOCON_ULTRASONIC_1_TRIGGER	(*(volatile uint32_t*)0x4002C024)
#define IOCON_ULTRASONIC_1_ECHO	(*(volatile uint32_t*)0x4002C060)

extern uint32_t ultrasonicSensorRisingCaptureTime[];
extern uint32_t ultrasonicSensorFallingCaptureTime[];
extern int8_t ultrasonicSensorNewDataAvailable[];

void Ultrasonic_Init(void);

void Ultrasonic_Start_Trigger_Timer(void);

#endif
