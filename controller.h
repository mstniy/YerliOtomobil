#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

typedef enum {
	LeftNew, LeftOngoing, RightNew, RightOngoing, Forward, Back, Stop
} Controller_Test_State;

typedef enum {
	Wait, Started, StoppedNew
} Controller_Auto_State;

typedef enum {
	ModeAuto, ModeTest, ModeManual
} Controller_Mode;

// volatile to keep the compiler from changing write/read order or optimizing-out writes etc.
extern volatile Controller_Test_State controller_test_state;
extern volatile Controller_Auto_State controller_auto_state;
extern volatile double controller_manual_left, controller_manual_right;
extern volatile Controller_Mode controller_mode;
extern volatile double Kp, Kd, Ki, Kk;

void Controller_Update(void);

#endif
