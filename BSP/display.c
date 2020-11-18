#include "display.h"

extern volatile SystemParams_t systemParams;
extern volatile int64_t zeroAngleOffset;
extern volatile int32_t loopError;
extern volatile int64_t steps;

menuItem_t*		ptrMenu;				//菜单栏
uint8_t 			menuIndex;			//菜单栏索引
options_t*		ptrOptions;			//选项栏
uint8_t 			optionIndex;		//选项栏索引

uint8_t 			buttonState;		//按键状态
bool 					menuActive;			//激活菜单

void display_begin(void)
{
	
	buttonState = 0;
	menuActive = false;
	
	ptrMenu = NULL;
	menuIndex = 0;
	ptrOptions = NULL;
	optionIndex = 0;
}

//显示4行字符串
void display_show(char* line1, char* line2, char* line3, char* line4)
{
	oled_clearLine(line1, line2, line3, line4);	
	oled_drawStr(0,0,line1);
	oled_drawStr(0,2,line2);
	oled_drawStr(0,4,line3);
	oled_drawStr(0,6,line4);
}

//设置菜单栏(MenuMain/MenuCal)
void display_setMenu(menuItem_t *pMenu)
{
	ptrMenu = pMenu;
	menuIndex = 0;
}

//显示3个菜单项
void display_showMenu(void)
{
	uint8_t i,j;
	char str[4][16] = {0};

	
	i = menuIndex;
	j = 0;
	while(ptrMenu[i].func!=NULL && j < 4)
	{
		if (i == menuIndex)
		{
			sprintf(str[j],"-> %s",ptrMenu[i].str);
		}else
		{
			sprintf(str[j],"   %s",ptrMenu[i].str);
		}
		j++;
		i++;
	}

	//show exit if there is room
	if (j < 4)
	{
		if (j == 0)
		{
			sprintf(str[j],"-> Exit");
		}else
		{
			sprintf(str[j],"   Exit");
		}
	}

	display_show(str[0], str[1], str[2], str[3]);
}

//显示3个选项
void display_showOptions(void)
{
	uint8_t i,j;
	char str[4][16] = {0};

	i = optionIndex;
	j = 0;
	while(strlen(ptrOptions[i].str) > 0 && j < 4)
	{
		if (i == optionIndex)
		{
			sprintf(str[j],"-> %s",ptrOptions[i].str);
		}else
		{
			sprintf(str[j],"   %s",ptrOptions[i].str);
		}
		j++;
		i++;
	}

	display_show(str[0], str[1], str[2], str[3]);
}

//激活菜单
void display_forceMenuActive(void)
{
	
	menuActive = true;
}

//更新菜单
void display_updateMenu(void)
{
	
		if (ptrOptions != NULL)
		{
			display_showOptions();
		}else
		{
			display_showMenu();
		}

	//handle push buttons
	if (GPIO_ReadInputDataBit(PIN_SW, PIN_SW3_ENTER) == 0 && (buttonState & 0x02) == 0)	//enter
	{
		buttonState |= 0x02;

		if (ptrMenu[menuIndex].func == NULL)
		{
			//exit pressed
			menuIndex = 0; //reset menu index
			menuActive = false;
			return;
		}

		if (ptrMenu[menuIndex].func != NULL)
		{
			if (ptrOptions != NULL)	//enter选项操作
			{
				char *ptrArgV[1];
				char str[25] = {0};
				ptrArgV[0] = str;
				sprintf(str,"%d",optionIndex);
				ptrMenu[menuIndex].func(1,ptrArgV);
				ptrOptions = NULL;	//退出选项ptrOptions=NULL
				optionIndex = 0;
			}else									//enter菜单操作
			{
				int i;
				i = ptrMenu[menuIndex].func(0,NULL);
				if (ptrMenu[menuIndex].ptrOptions != NULL)
				{
					ptrOptions = ptrMenu[menuIndex].ptrOptions;	//进入选项ptrOptions=ptrMenu[menuIndex].ptrOptions
					optionIndex = i;
				}
			}
			return;
		}
	}
	//(buttonState & 0x01)==0防止按键一直按住
	if (GPIO_ReadInputDataBit(PIN_SW, PIN_SW1_NEXT) == 0 && (buttonState & 0x01) == 0)	//next
	{
		buttonState |= 0x01;
		
		if (ptrOptions != NULL)	//next选项操作
		{
			optionIndex++;
			if (strlen(ptrOptions[optionIndex].str) == 0)
			{
				optionIndex = 0;
			}
		} else
		{
			if (ptrMenu[menuIndex].func != NULL)	//next菜单操作
			{
				menuIndex++;
			} else
			{
				menuIndex = 0;	//返回0
			}
		}

	}

	if (GPIO_ReadInputDataBit(PIN_SW, PIN_SW1_NEXT))
	{
		buttonState &= (~0x01);
	}

	if (GPIO_ReadInputDataBit(PIN_SW, PIN_SW3_ENTER))
	{
		buttonState &= (~0x02);
	}
}

//olde显示
void display_process(void)
{
	
		if (false == menuActive || ptrMenu == NULL)
		{
			display_updateLCD();	//显示误差、PID模式等数据
		}
		else
		{
			display_updateMenu();	//显示菜单
		}

	if (GPIO_ReadInputDataBit(PIN_SW, PIN_SW4_EXIT) == 0 && (buttonState & 0x04) == 0)	//exit
	{
		buttonState |= 0x04;
		menuActive = (bool)(!menuActive);
	}

	if (GPIO_ReadInputDataBit(PIN_SW, PIN_SW4_EXIT))
	{
		buttonState &= (~0x04);
	}
}

//显示部分
void display_updateLCD(void)
{	
	char str[4][25] = {0}; //定义4行字符串

	//第1行显示PID控制模式
	sprintf(str[0], "Increment PID");
//	switch(systemParams.controllerMode)
//	{
//		case CTRL_SIMPLE:
//			sprintf(str[0], "Simple PID");
//			break;

//		case CTRL_POS_PID:
//			sprintf(str[0], "Position PID");
//			break;

//		case CTRL_POS_VELOCITY_PID:
//			sprintf(str[0], "Velocity PID");
//			break;

//		default:
//			sprintf(str[0], "Error %u",systemParams.controllerMode);
//			break;
//	}

	//第2行显示当前PID误差
	int32_t err = loopError;
	int32_t x, y, z;
	err = (err * 360 * 100) >> 16;
	z = err / 100;
	y = abs(err - (z * 100));
	sprintf(str[1],"%01d.%02d err", z,y);

	//第3行显示当前位置
	int64_t deg;
	deg = StepperCtrl_getDesiredLocation() - zeroAngleOffset;
	deg = (deg * 225) / (ANGLE_STEPS >> 4);	//deg = (deg * 360 * 10) / (int32_t)ANGLE_STEPS;

	__IO uint8_t K = 0;
	if (abs(deg) > 9999)
	{
		K = 1;
		deg = deg / 1000;
	}
	
	x = deg / 10;
	y = abs(deg - (x * 10));
	
	if (K == 1)
	{
		sprintf(str[2],"%03d.%01uKdeg", x,y);
	}else
	{
		sprintf(str[2],"%03d.%01udeg", x,y);
	}
	
	//第4行显示走过的步数
	if(steps < 200000000 && steps > -200000000)
	{
		sprintf(str[3],"%dsteps", (int32_t)steps);
	}else
	{
		if(steps > 200000000)
		{
			sprintf(str[3],">200000000");
		}else
		{
			sprintf(str[3],"<-200000000");
		}
	}
	
	display_show(str[0], str[1], str[2], str[3]);
}
