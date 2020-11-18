#include "USART.h"

///**
//	* @brief   配置USART中断优先级
//	* @param   无
//	* @retval  无
//	*/
//static void NVIC_Configuration (void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}

///**
//	* @brief   USART初始化
//	* @param   无
//	* @retval  无
//	*/
//void USART_Config (void)
//{
//	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB| RCC_APB2Periph_USART1,ENABLE);	
//	
//	/***********************GPIO初始化**********************/
//	//RX引脚初始化
//	GPIO_InitTypeDef USART_GPIO_InitStructure;
//	USART_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	USART_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init (GPIOB,&USART_GPIO_InitStructure);
//	
//	//Tx引脚初始化
//	USART_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	USART_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	USART_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init (GPIOB,&USART_GPIO_InitStructure);

//	/***********************USART初始化**********************/
//	USART_InitTypeDef USART_InitStructure;	
//	USART_InitStructure.USART_BaudRate = 9600;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//	USART_Init (USART1,&USART_InitStructure);
//	
//	NVIC_Configuration ();
//	
//	//使能USART
//	USART_Cmd (USART1,ENABLE);
//}


#define USE_DEBUG_VIEWER
#ifdef USE_DEBUG_VIEWER

/**
	* @brief   重定向C库函数到keil仿真窗口
	* @param   int ch
	* @param   FILE* f
	* @retval  ch
	*/
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))        
#define TRCENA          0x01000000
 
//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//FILE __stdin;
 
int fputc(int ch, FILE *f)
{
    if (DEMCR & TRCENA)
    {
        while (ITM_Port32(0) == 0);
        ITM_Port8(0) = ch;
    }
    return ch;
}

#else

/**
	* @brief   重定向C库函数到串口
	* @param   int ch
	* @param   FILE* f
	* @retval  ch
	*/
	int fputc (int ch,FILE *f)
	{
		USART_SendData (USART1,(uint8_t)ch);
		while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
		return (ch);
	}	
		
	
	int fgetc (FILE *f)
	{
		while (USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET);
		return (int)USART_ReceiveData(USART1);
	}

#endif
