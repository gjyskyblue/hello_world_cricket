#ifndef __MKS_H
#define __MKS_H

#include "display.h"
#include "stepper_controller.h"
#include "stdlib.h"
#include "steppin.h"

int menuCalibrate(int argc, char *argv[]);
int menuTestCal(int argc, char *argv[]);
int motorSteps(int argc, char *argv[]);
int motorCurrent(int argc, char *argv[]);
int motorHoldCurrent(int argc, char *argv[]);
int microsteps(int argc, char *argv[]);
int controlLoop(int argc, char *argv[]);
int enablePin(int argc, char *argv[]);
int dirPin(int argc, char *argv[]);

void validateAndInitNVMParams(void);
static void enableInput(void);
void MKS_begin(void);
void MKS_loop(void);

#endif
