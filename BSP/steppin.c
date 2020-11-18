#include "steppin.h"

extern volatile uint32_t NVM_address;
extern volatile int64_t numSteps;
volatile int64_t steps = 0;

void dirPinSetup(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); //��PA1����ΪEXTI������
	
	EXTI_InitTypeDef  EXTI_InitStructure; //����DIR(PA1)Ϊ�������ж�,���ȼ�Ϊ0(��board������)	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�������ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_ClearITPendingBit(EXTI_Line1); //����жϱ�־λ
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
	
	TIM_SetCounter(INPUT_TIM, 0); //��ռ�����
	TIM_Cmd(INPUT_TIM, ENABLE); //������ʱ��
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
		INPUT_TIM->CR1 &= 0xffef; //0�����������ϼ��� 	TC4->COUNT16.CTRLBCLR.bit.DIR = 1;
	} else
	{
		INPUT_TIM->CR1 |= 0x0010; //1�����������¼���	TC4->COUNT16.CTRLBSET.bit.DIR = 1;
	}
}

void inputPinSetup(void)
{
	dirPinSetup(); //setup the dir pin

	stepPinSetup(); //setup the step pin
	
	dirChanged_ISR();
}

//EXTI1�ⲿ�жϴ�����
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line1); //����жϱ�־λ
		
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

	steps += dy; //��¼���ϵ翪ʼһ�����˶��ٲ���������ʾ��oled��
	
	lasty = y;
	
	return (int64_t)dy;
}

void enableINPUTInterrupts(void)
{
	dirChanged_ISR();

	EXTI_ClearITPendingBit(EXTI_Line1); 	//����жϱ�־λ
	EXTI->IMR |= EXTI_Line1;						  //����dir�ⲿ�ж�
	
	TIM_Cmd(INPUT_TIM, ENABLE);  					//����step��ʱ��
}

void disableINPUTInterrupts(void)
{
	TIM_Cmd(INPUT_TIM, DISABLE); 					//�ر�step��ʱ��
	
	EXTI->IMR &= (~EXTI_Line1);  					//�ر�dir�ⲿ�ж�
	EXTI_ClearITPendingBit(EXTI_Line1); 	//����жϱ�־λ
}
