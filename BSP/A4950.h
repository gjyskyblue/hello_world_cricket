#ifndef __A4950_H
#define __A4950_H

#include "stm32f10x.h"
#include "stdlib.h"
#include "board.h"
#include "sine.h"

#define A4950_NUM_MICROSTEPS ((int32_t)100)
#define A4950_STEP_MICROSTEPS ((int32_t)256)

inline static void bridge1(int state);
inline static void bridge2(int state);
inline static void setVREF(uint16_t VREF12, uint16_t VREF34);
void A4950_enable(bool enable);
int32_t A4950_move(int32_t stepAngle, uint32_t mA);
void A4954_begin(void);

#endif
