#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "delay.h"
#include "nonvolatile.h"
#include "stepper_controller.h"

typedef struct{
	char str[15];
}options_t;

typedef struct{
	char str[15]; //菜单名
	int (*func)(int argc, char *argv[]); //处理函数
	options_t *ptrOptions; //子选项
}menuItem_t;

void display_begin(void);
void display_show(char* line1, char* line2, char* line3, char* line4);
void display_setMenu(menuItem_t *pMenu);
void display_showMenu(void);
void display_showOptions(void);
void display_updateMenu(void);
void display_forceMenuActive(void);
void display_process(void);
void display_updateLCD(void);

#endif
