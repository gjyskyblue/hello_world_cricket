#ifndef __A1333_H
#define __A1333_H

#include "stm32f10x.h"
#include "board.h"
#include "delay.h"
#include "spi.h"

uint16_t PrimaryRead(uint16_t address);
uint16_t PrimaryWrite(uint16_t address, uint16_t value);
bool A1333_begin(void);
uint16_t A1333_readAddress(uint16_t addr);
uint16_t A1333_readEncoderAngle(void);

#endif
