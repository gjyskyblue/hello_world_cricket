#ifndef __SINE_H
#define __SINE_H

#include "stm32f10x.h"

#define SINE_STEPS	((uint16_t)1024)

#define SINE_MAX	(uint16_t)(511)

int32_t sine(uint16_t angle);
int32_t cosine(uint16_t angle);
int32_t fastAbs(int32_t v);

#endif
