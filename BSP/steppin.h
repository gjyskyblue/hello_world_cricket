#ifndef __STEPPIN_H
#define __STEPPIN_H

#include "board.h"
#include "nonvolatile.h"
#include "stepper_controller.h"

void dirPinSetup(void);
void stepPinSetup(void);
void inputPinSetup(void);
void dirChanged_ISR(void);
int64_t getSteps(void);
void disableINPUTInterrupts(void);
void enableINPUTInterrupts(void);

#endif
