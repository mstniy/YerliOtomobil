#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "GPIO.h"
#include "Timer.h"
#include "SystemStructures.h"

#define IOCON_ULTRASONIC_TRIGGER	(*(volatile uint32_t*)0x4002C020)
#define IOCON_ULTRASONIC_ECHO	(*(volatile uint32_t*)0x4002C05C)

extern uint32_t ultrasonicSensorRisingCaptureTime;
extern uint32_t ultrasonicSensorFallingCaptureTime;
extern int8_t ultrasonicSensorNewDataAvailable;

void Ultrasonic_Start(void);

#endif
