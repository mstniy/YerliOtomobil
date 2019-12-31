#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "GPIO.h"
#include "Timer.h"
#include "SystemStructures.h"

#define IOCON_TRIGGER	*((volatile uint32_t*)0x4002C024)
#define IOCON_ECHO	*((volatile uint32_t*)0x4002C060)

extern volatile uint8_t ultrasonicSensorNewDataAvailable;
extern volatile double ultrasonicSensorLastMeasurementCM;

void Ultrasonic_Init(void);

#endif
