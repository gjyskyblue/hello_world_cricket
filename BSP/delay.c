#include "delay.h"

void delay_us (__IO uint32_t us)
{
	uint32_t i;
	/**
		* SystemCoreClock/1000000     1us中断一次
		* SystemCoreClock/100000			10us中断一次
		* SystemCoreClock/1000				1ms中断一次
		*/
	SysTick_Config (SystemCoreClock / 1000000); //1us中断一次

	//当计数器的值减为0，CTRL的bit16会置1，读取该位将自动清零	
	 for(i = 0; i < us; i++)
			while (!((SysTick -> CTRL) & (1 << 16))); //((SysTick -> CTRL)&(1<<16))判断CTRL的bit16是否为1
	 
	 //关闭SysTick定时器
	 SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk; //SysTick_CTRL_ENABLE_Msk,即为(1<<0)	 
}

void delay_ms (__IO uint32_t ms)
{
		uint32_t i, t0;
	/**
		* SystemCoreClock/1000000     1us中断一次
		* SystemCoreClock/100000			10us中断一次
		* SystemCoreClock/1000				1ms中断一次
		*/
	SysTick_Config (SystemCoreClock / 1000); //1ms中断一次
	
	//当计数器的值减为0，CTRL的bit16会置1，读取该位将自动清零
	 for(i = 0; i < ms; i++)
	 {
			t0 = 0;
			while (!((SysTick -> CTRL) & (1 << 16))) //((SysTick -> CTRL)&(1<<16))判断CTRL的bit16是否为1
			{
				t0++;
				if(t0 > 20000) //1.3ms左右延时
				{
					break;
				}
			}
	 }	 
	 
	 //关闭SysTick定时器
	 SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk; //SysTick_CTRL_ENABLE_Msk,即为(1<<0)	 
}
