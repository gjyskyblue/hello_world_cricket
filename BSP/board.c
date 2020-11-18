#include "board.h"

//��ʼ������ʱ��
static void CLOCK_init(void)
{	
//	RCC->APB2ENR |= (1<<0);	//����AFIOʱ��
	RCC->APB2ENR |= (1<<2);	//����GPIOAʱ��
	RCC->APB2ENR |= (1<<3);	//����GPIOBʱ��
	RCC->APB2ENR |= (1<<4);	//����GPIOCʱ��
	RCC->APB2ENR |= (1<<12);//����SPI1ʱ��

	RCC->APB2ENR |= (1<<11);	//����TIM1ʱ��

	RCC->APB1ENR |= (1<<0);	//����TIM2ʱ��
	RCC->APB1ENR |= (1<<1);	//����TIM3ʱ��
//	RCC->APB1ENR |= (1<<2);	//����TIM4ʱ��
	RCC->APB1ENR |= (1<<14);//����SPI2ʱ��
}

//��ʼ���ж����ȼ�
static void NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //��4��:4λ��ռ���ȼ�
	NVIC_SetPriority(SysTick_IRQn,15); //����SysTick_IRQn��ռ���ȼ����
	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //��ռ���ȼ�0(dir����)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //��ռ���ȼ�Ϊ1(����ѭ��)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

//��ʼ��INPUT����(step dir enable)
static void INPUT_init(void)
{	
	GPIOA->CRL &= 0xfffff000;	//���STEP(PA0) DIR(PA1) ENABLE(PA2)�˿ڿ���λ
	GPIOA->CRL |= 0x00000888;	//����STEP(PA0) DIR(PA1) ENABLE(PA2)����ģʽ	
	GPIOA->ODR |= 0x00000007;	//Ĭ��STEP(PA0) DIR(PA1) ENABLE(PA2)��������
}

//��ʼ��SSD1306				    
static void OLED_init(void)
{ 		
#if NO_OLED_MODE
	GPIOA->CRL &= 0x0000ffff;	//���CS(PA4) D0(PA5) DC(PA6) D1(PA7)�˿ڿ���λ
	GPIOA->CRL |= 0x33330000;	//����PA4,PA5,PA6,PA7ͨ���������
	GPIOA->ODR |= 0x00000000;	//Ĭ��CS(PA4) D0(PA5) DC(PA6) D1(PA7)����͵�ƽ
	
#else	
	GPIOA->CRL &= 0x0000ffff;	//���CS(PA4) D0(PA5) DC(PA6) D1(PA7)�˿ڿ���λ
	GPIOA->CRL |= 0xb3b30000;	//����D0 D1�������������CS DCͨ���������
	GPIOA->ODR |= 0x000000f0;	//Ĭ��CS(PA4) D0(PA5) DC(PA6) D1(PA7)����ߵ�ƽ
	
	SPI_InitTypeDef SPI_InitStructure;	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;						//1��������ģʽ
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;												//����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;										//8bitģʽ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;													//CPOL=1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;												//CPHA=1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;														//�������CS�ŵ�ƽ
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //32��Ƶ��Ƶ��ԼΪ2.25MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//��λ����
	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRC����ʽ	
	SPI_Init (SPI1,&SPI_InitStructure);	
	SPI_Cmd (SPI1,ENABLE);	//ʹ��SPI1
#endif	
}

//��ʼ��SW����
static void SWITCH_init(void)
{
	GPIOA->CRH &= 0xfffff000;	//���SW4 SW3 SW1�˿ڿ���λ(PA8 PA9 PA10)	
	GPIOA->CRH |= 0x00000888;	//����SW4 SW3 SW1����ģʽ(PA8 PA9 PA10)	
	GPIOA->ODR |= 0x00000700;	//Ĭ��SW4 SW3 SW1��������(PA8 PA9 PA10)
}

//��ʼ��A4950
static void A4950_init(void)
{
	//��ʼ��A4950����
	GPIOB->CRL &= 0x00ffff00;	//���VREF12(PB0) VREF34(PB1) IN1(PB6) IN2(PB7)�˿ڿ���λ
	GPIOB->CRL |= 0x330000bb;	//����VREF12(PB0) VREF34(PB1)����������� IN1(PB6) IN2(PB7)ͨ���������
	GPIOB->CRH &= 0xffffff00;	//���IN3(PB8) IN4(PB9)�˿ڿ���λ	
	GPIOB->CRH |= 0x00000033;	//����IN3(PB8) IN4(PB9)ͨ���������	
	GPIOB->ODR &= 0xfffffc3f;	//Ĭ��IN1(PB6) IN2(PB7) IN3(PB8) IN4(PB9)����͵�ƽ
	
	//��ʼ��TIM3
	TIM_TimeBaseInitTypeDef  		TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = VREF_MAX;									//��װ��ֵΪ511
	TIM_TimeBaseStructure.TIM_Prescaler = 0;											//0��Ƶ��Ƶ��Ϊ72MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;									//ʹ���ڲ�ʱ�ӣ�	�ֲ���Ƶ��Ӱ��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���ģʽ
	TIM_TimeBaseInit(VREF_TIM, &TIM_TimeBaseStructure);
	
	//��ʼ��TIM3 CH3��CH4ΪPWM1ģʽ
	TIM_OCInitTypeDef  	        TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//��Ч��ƽΪ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;							//PWM1ģʽ	
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ��PWM���
	TIM_OCInitStructure.TIM_Pulse = 0;														//�ȽϼĴ���ֵĬ��Ϊ0
	TIM_OC3Init(VREF_TIM, &TIM_OCInitStructure);									//TIM3 CH3	PB0
	TIM_OC4Init(VREF_TIM, &TIM_OCInitStructure);									//TIM3 CH4	PB1
 
	TIM_OC3PreloadConfig(VREF_TIM, TIM_OCPreload_Enable);					//ʹ��TIM3 CH3��Ԥװ�ع���
	TIM_OC4PreloadConfig(VREF_TIM, TIM_OCPreload_Enable);					//ʹ��TIM3 CH4��Ԥװ�ع���
 
	TIM_Cmd(VREF_TIM, ENABLE);																		//ʹ��TIM3
}

//��ʼ��A1333
static void A1333_init (void)
{	
	GPIOB->CRH &= 0x0000ffff;	//���CS(PB12) SCK(PB13) MOSI(PB15) MISO(PB14)�˿ڿ���λ
	GPIOB->CRH |= 0xb8b30000;	//����CSͨ�����������SCK MOSI�������������MISO��������
	GPIOB->ODR |= 0x0000f000;	//Ĭ��CS SCK MOSI MISO����ߵ�ƽ

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//����ȫ˫��ģʽ
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;												//����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;									//16bitģʽ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;													//CPOL=1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;												//CPHA=1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;														//�������CS�ŵ�ƽ
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//8��Ƶ=4.5MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//��λ����
	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRC����ʽ	
	SPI_Init (SPI2,&SPI_InitStructure);	
	SPI_Cmd (SPI2,ENABLE);	//ʹ��SPI2
}

static void LED_init(void)
{
	//RED_LED
	GPIOA->CRL &= 0xffff0fff;	//PA3
	GPIOA->CRL |= 0x00003000; //ͨ���������
	GPIOA->ODR |= 0x00000008;	//�ߵ�ƽ����

	//BLUE_LED
	GPIOC->CRH &= 0xff0fffff;	//PC13
	GPIOC->CRH |= 0x00300000; //ͨ���������
	GPIOC->ODR |= 0x00002000;	//�ߵ�ƽϨ��
}

void board_init(void)
{
	CLOCK_init(); //ʱ�ӳ�ʼ��
	NVIC_init();  //�ж����ȼ���ʼ��
	INPUT_init(); //step dir enable���ų�ʼ��
	A4950_init(); //A4950���ų�ʼ��
	A1333_init(); //A1333���ų�ʼ��
	OLED_init();  //OLED���ų�ʼ��
	SWITCH_init();//�������ų�ʼ��
	LED_init(); 	//LED���ų�ʼ��
//	USART_Config ();
}

//��ɫָʾ��
//state = true  ��
//state = false ��
void RED_LED(bool state)
{
	GPIO_WriteBit(PIN_RED, PIN_LED_RED, (BitAction)state);
}

//��ɫ����ָʾ��
//state(error) = true  ��
//state(error) = false ��
void BLUE_LED(bool state)
{
	GPIO_WriteBit(PIN_BLUE, PIN_LED_BLUE, (BitAction)(!state));
}
