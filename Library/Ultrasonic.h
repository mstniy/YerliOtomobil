#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "GPIO.h"
#include "Timer.h"
#include "SystemStructures.h"

#define IOCON_TRIGGER	(*(volatile uint32_t*)0x4002C024)

#define IOCON_ECHO	(*(volatile uint32_t*)0x4002C060)

extern uint32_t ultrasonicSensorRisingCaptureTime;
extern uint32_t ultrasonicSensorFallingCaptureTime;
extern uint8_t ultrasonicSensorNewDataAvailable;

extern uint8_t ultrasonicSensorTriggerStart;

void Ultrasonic_Init(void);

void Ultrasonic_Start_Trigger_Timer(void);

#endif
