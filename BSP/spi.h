#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

uint16_t SPI_WriteAndRead(SPI_TypeDef* SPIx, uint16_t Data);
bool SPI_Write(SPI_TypeDef* SPIx, uint8_t data);

#endif
