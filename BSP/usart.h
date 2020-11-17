#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

static void NVIC_Configuration (void);
void USART_Config (void);
int fputc (int ch,FILE *f);
int fgetc (FILE *f);

#endif
