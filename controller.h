#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef enum {
	LeftNew, LeftOngoing, RightNew, RightOngoing, Forward, Back, Stop
} Controller_Test_State;

typedef enum {
	Wait, Started, StoppedNew
} Controller_Auto_State;

// volatile to keep the compiler from changing write/read order or optimizing-out writes etc.
extern volatile Controller_Test_State controller_test_state;
extern volatile Controller_Auto_State controller_auto_state;
extern volatile int controller_in_test;

void Controller_Update(void);

#endif
