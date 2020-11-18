#include "board.h"

//初始化外设时钟
static void CLOCK_init(void)
{	
//	RCC->APB2ENR |= (1<<0);	//开启AFIO时钟
	RCC->APB2ENR |= (1<<2);	//开启GPIOA时钟
	RCC->APB2ENR |= (1<<3);	//开启GPIOB时钟
	RCC->APB2ENR |= (1<<4);	//开启GPIOC时钟
	RCC->APB2ENR |= (1<<12);//开启SPI1时钟

	RCC->APB2ENR |= (1<<11);	//开启TIM1时钟

	RCC->APB1ENR |= (1<<0);	//开启TIM2时钟
	RCC->APB1ENR |= (1<<1);	//开启TIM3时钟
//	RCC->APB1ENR |= (1<<2);	//开启TIM4时钟
	RCC->APB1ENR |= (1<<14);//开启SPI2时钟
}

//初始化中断优先级
static void NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //第4组:4位抢占优先级
	NVIC_SetPriority(SysTick_IRQn,15); //设置SysTick_IRQn抢占优先级最低
	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //抢占优先级0(dir引脚)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //抢占优先级为1(控制循环)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

//初始化INPUT引脚(step dir enable)
static void INPUT_init(void)
{	
	GPIOA->CRL &= 0xfffff000;	//清除STEP(PA0) DIR(PA1) ENABLE(PA2)端口控制位
	GPIOA->CRL |= 0x00000888;	//配置STEP(PA0) DIR(PA1) ENABLE(PA2)输入模式	
	GPIOA->ODR |= 0x00000007;	//默认STEP(PA0) DIR(PA1) ENABLE(PA2)上拉输入
}

//初始化SSD1306				    
static void OLED_init(void)
{ 		
#if NO_OLED_MODE
	GPIOA->CRL &= 0x0000ffff;	//清除CS(PA4) D0(PA5) DC(PA6) D1(PA7)端口控制位
	GPIOA->CRL |= 0x33330000;	//配置PA4,PA5,PA6,PA7通用推挽输出
	GPIOA->ODR |= 0x00000000;	//默认CS(PA4) D0(PA5) DC(PA6) D1(PA7)输出低电平
	
#else	
	GPIOA->CRL &= 0x0000ffff;	//清除CS(PA4) D0(PA5) DC(PA6) D1(PA7)端口控制位
	GPIOA->CRL |= 0xb3b30000;	//配置D0 D1复用推挽输出，CS DC通用推挽输出
	GPIOA->ODR |= 0x000000f0;	//默认CS(PA4) D0(PA5) DC(PA6) D1(PA7)输出高电平
	
	SPI_InitTypeDef SPI_InitStructure;	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;						//1线主发送模式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;												//主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;										//8bit模式
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;													//CPOL=1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;												//CPHA=1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;														//软件控制CS脚电平
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //32分频，频率约为2.25MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//高位优先
	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRC多项式	
	SPI_Init (SPI1,&SPI_InitStructure);	
	SPI_Cmd (SPI1,ENABLE);	//使能SPI1
#endif	
}

//初始化SW引脚
static void SWITCH_init(void)
{
	GPIOA->CRH &= 0xfffff000;	//清除SW4 SW3 SW1端口控制位(PA8 PA9 PA10)	
	GPIOA->CRH |= 0x00000888;	//配置SW4 SW3 SW1输入模式(PA8 PA9 PA10)	
	GPIOA->ODR |= 0x00000700;	//默认SW4 SW3 SW1上拉输入(PA8 PA9 PA10)
}

//初始化A4950
static void A4950_init(void)
{
	//初始化A4950引脚
	GPIOB->CRL &= 0x00ffff00;	//清除VREF12(PB0) VREF34(PB1) IN1(PB6) IN2(PB7)端口控制位
	GPIOB->CRL |= 0x330000bb;	//配置VREF12(PB0) VREF34(PB1)复用推挽输出 IN1(PB6) IN2(PB7)通用推挽输出
	GPIOB->CRH &= 0xffffff00;	//清除IN3(PB8) IN4(PB9)端口控制位	
	GPIOB->CRH |= 0x00000033;	//配置IN3(PB8) IN4(PB9)通用推挽输出	
	GPIOB->ODR &= 0xfffffc3f;	//默认IN1(PB6) IN2(PB7) IN3(PB8) IN4(PB9)输出低电平
	
	//初始化TIM3
	TIM_TimeBaseInitTypeDef  		TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = VREF_MAX;									//重装载值为511
	TIM_TimeBaseStructure.TIM_Prescaler = 0;											//0分频，频率为72MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;									//使用内部时钟，	分不分频不影响
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数模式
	TIM_TimeBaseInit(VREF_TIM, &TIM_TimeBaseStructure);
	
	//初始化TIM3 CH3、CH4为PWM1模式
	TIM_OCInitTypeDef  	        TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//有效电平为高
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;							//PWM1模式	
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能PWM输出
	TIM_OCInitStructure.TIM_Pulse = 0;														//比较寄存器值默认为0
	TIM_OC3Init(VREF_TIM, &TIM_OCInitStructure);									//TIM3 CH3	PB0
	TIM_OC4Init(VREF_TIM, &TIM_OCInitStructure);									//TIM3 CH4	PB1
 
	TIM_OC3PreloadConfig(VREF_TIM, TIM_OCPreload_Enable);					//使能TIM3 CH3的预装载功能
	TIM_OC4PreloadConfig(VREF_TIM, TIM_OCPreload_Enable);					//使能TIM3 CH4的预装载功能
 
	TIM_Cmd(VREF_TIM, ENABLE);																		//使能TIM3
}

//初始化A1333
static void A1333_init (void)
{	
	GPIOB->CRH &= 0x0000ffff;	//清除CS(PB12) SCK(PB13) MOSI(PB15) MISO(PB14)端口控制位
	GPIOB->CRH |= 0xb8b30000;	//配置CS通用推挽输出，SCK MOSI复用推挽输出，MISO下拉输入
	GPIOB->ODR |= 0x0000f000;	//默认CS SCK MOSI MISO输出高电平

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//两线全双工模式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;												//主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;									//16bit模式
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;													//CPOL=1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;												//CPHA=1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;														//软件控制CS脚电平
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//8分频=4.5MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//高位优先
	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRC多项式	
	SPI_Init (SPI2,&SPI_InitStructure);	
	SPI_Cmd (SPI2,ENABLE);	//使能SPI2
}

static void LED_init(void)
{
	//RED_LED
	GPIOA->CRL &= 0xffff0fff;	//PA3
	GPIOA->CRL |= 0x00003000; //通用推挽输出
	GPIOA->ODR |= 0x00000008;	//高电平点亮

	//BLUE_LED
	GPIOC->CRH &= 0xff0fffff;	//PC13
	GPIOC->CRH |= 0x00300000; //通用推挽输出
	GPIOC->ODR |= 0x00002000;	//高电平熄灭
}

void board_init(void)
{
	CLOCK_init(); //时钟初始化
	NVIC_init();  //中断优先级初始化
	INPUT_init(); //step dir enable引脚初始化
	A4950_init(); //A4950引脚初始化
	A1333_init(); //A1333引脚初始化
	OLED_init();  //OLED引脚初始化
	SWITCH_init();//开关引脚初始化
	LED_init(); 	//LED引脚初始化
//	USART_Config ();
}

//红色指示灯
//state = true  亮
//state = false 灭
void RED_LED(bool state)
{
	GPIO_WriteBit(PIN_RED, PIN_LED_RED, (BitAction)state);
}

//蓝色错误指示灯
//state(error) = true  亮
//state(error) = false 灭
void BLUE_LED(bool state)
{
	GPIO_WriteBit(PIN_BLUE, PIN_LED_BLUE, (BitAction)(!state));
}
