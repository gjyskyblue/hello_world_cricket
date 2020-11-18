#ifndef __IIC_H
#define __IIC_H

#include "stm32f10x.h"
#include "delay.h"
//cricket2009_AOC push
//cricket2009_HP push

/*�����ӿڣ�GPIOģ��IIC*/
//SCL-->PB6
#define SCL_L		GPIOB->BRR = GPIO_Pin_6
#define SCL_H		GPIOB->BSRR = GPIO_Pin_6
//SDA-->PB7
#define SDA_L		GPIOB->BRR = GPIO_Pin_7
#define SDA_H		GPIOB->BSRR = GPIO_Pin_7

//��ȡSDA�ߵ� ����״̬
#define SDA_READ   GPIOB->IDR & GPIO_Pin_7

//����SDA�˿ڵ��������:����/��������/��©���
#define SDA_IN()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define SDA_OUT() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x70000000;}


/***************IICͨ��ʱ���������***************/

void IIC_Start (void);//��ʼ�ź�
void IIC_Stop (void);//ֹͣ�ź�
void IIC_Ack (void);//Ӧ���ź�
void IIC_NAck (void);//��Ӧ���ź�
ErrorStatus IIC_Get_Ack (void);//�ȴ��ӻ�Ӧ���ź�
void IIC_ByteWrite (__IO uint8_t BYTE);//����һ���ֽ�����


/***************IICͨ�Ź���ʵ�ֺ���***************/

//��ָ��������ָ���Ĵ�����ַ����һ���ֽ�����
ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte);
//����Length���ֽ�����
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer);
//����Length���ֽ�0����
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length);

#endif
