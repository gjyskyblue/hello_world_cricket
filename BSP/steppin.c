#include "steppin.h"

extern volatile uint32_t NVM_address;
extern volatile int64_t numSteps;
volatile int64_t steps = 0;

void dirPinSetup(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); //将PA1设置为EXTI输入线
	
	EXTI_InitTypeDef  EXTI_InitStructure; //设置DIR(PA1)为跳变沿中断,优先级为0(在board中设置)	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //跳变沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_ClearITPendingBit(EXTI_Line1); //清除中断标志位
	EXTI_Init(&EXTI_InitStructure);
}

void stepPinSetup(void)
{
	TIM_TimeBaseInitTypeDef  		TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = INPUT_MAX;
	TIM_TimeBaseStructure.TIM_Prescaler = 0 ;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(INPUT_TIM, &TIM_TimeBaseStructure);
	
	TIM_TIxExternalClockConfig(INPUT_TIM,TIM_TS_TI1FP1, TIM_ICPolarity_Rising, 5);
	
	TIM_SetCounter(INPUT_TIM, 0); //清空计数器
	TIM_Cmd(INPUT_TIM, ENABLE); //启动定时器
}

void dirChanged_ISR(void)
{
	int dir = 0;
	
	if(GPIO_ReadInputDataBit(PIN_INPUT, PIN_INPUT_DIR) != (uint8_t)Bit_RESET)
	{
		dir = 1;
	}
	
	if(CW_ROTATION == NVM->SystemParams.dirPinRotation)
	{
		dir = !dir; //reverse the rotation
	}
	
	if(dir)
	{
		INPUT_TIM->CR1 &= 0xffef; //0：计数器向上计数 	TC4->COUNT16.CTRLBCLR.bit.DIR = 1;
	} else
	{
		INPUT_TIM->CR1 |= 0x0010; //1：计数器向下计数	TC4->COUNT16.CTRLBSET.bit.DIR = 1;
	}
}

void inputPinSetup(void)
{
	dirPinSetup(); //setup the dir pin

	stepPinSetup(); //setup the step pin
	
	dirChanged_ISR();
}

//EXTI1外部中断处理函数
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line1); //清除中断标志位
		
		dirChanged_ISR();
	}
}

int64_t getSteps(void)
{
	uint16_t y;
	static uint16_t lasty = 0;
	int16_t dy;
	
	y = INPUT_TIM->CNT;
	
	dy = (int16_t)(y - lasty);

	steps += dy; //记录从上电开始一共走了多少步，用于显示在oled上
	
	lasty = y;
	
	return (int64_t)dy;
}

void enableINPUTInterrupts(void)
{
	dirChanged_ISR();

	EXTI_ClearITPendingBit(EXTI_Line1); 	//清除中断标志位
	EXTI->IMR |= EXTI_Line1;						  //开启dir外部中断
	
	TIM_Cmd(INPUT_TIM, ENABLE);  					//开启step定时器
}

void disableINPUTInterrupts(void)
{
	TIM_Cmd(INPUT_TIM, DISABLE); 					//关闭step定时器
	
	EXTI->IMR &= (~EXTI_Line1);  					//关闭dir外部中断
	EXTI_ClearITPendingBit(EXTI_Line1); 	//清除中断标志位
}
