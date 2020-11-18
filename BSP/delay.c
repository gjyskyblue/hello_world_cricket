#include "delay.h"

void delay_us (__IO uint32_t us)
{
	uint32_t i;
	/**
		* SystemCoreClock/1000000     1us�ж�һ��
		* SystemCoreClock/100000			10us�ж�һ��
		* SystemCoreClock/1000				1ms�ж�һ��
		*/
	SysTick_Config (SystemCoreClock / 1000000); //1us�ж�һ��

	//����������ֵ��Ϊ0��CTRL��bit16����1����ȡ��λ���Զ�����	
	 for(i = 0; i < us; i++)
			while (!((SysTick -> CTRL) & (1 << 16))); //((SysTick -> CTRL)&(1<<16))�ж�CTRL��bit16�Ƿ�Ϊ1
	 
	 //�ر�SysTick��ʱ��
	 SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk; //SysTick_CTRL_ENABLE_Msk,��Ϊ(1<<0)	 
}

void delay_ms (__IO uint32_t ms)
{
		uint32_t i, t0;
	/**
		* SystemCoreClock/1000000     1us�ж�һ��
		* SystemCoreClock/100000			10us�ж�һ��
		* SystemCoreClock/1000				1ms�ж�һ��
		*/
	SysTick_Config (SystemCoreClock / 1000); //1ms�ж�һ��
	
	//����������ֵ��Ϊ0��CTRL��bit16����1����ȡ��λ���Զ�����
	 for(i = 0; i < ms; i++)
	 {
			t0 = 0;
			while (!((SysTick -> CTRL) & (1 << 16))) //((SysTick -> CTRL)&(1<<16))�ж�CTRL��bit16�Ƿ�Ϊ1
			{
				t0++;
				if(t0 > 20000) //1.3ms������ʱ
				{
					break;
				}
			}
	 }	 
	 
	 //�ر�SysTick��ʱ��
	 SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk; //SysTick_CTRL_ENABLE_Msk,��Ϊ(1<<0)	 
}
