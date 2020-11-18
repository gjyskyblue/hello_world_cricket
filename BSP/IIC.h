#ifndef __IIC_H
#define __IIC_H

#include "stm32f10x.h"
#include "delay.h"
//cricket2009_AOC push
//cricket2009_HP push

/*驱动接口，GPIO模拟IIC*/
//SCL-->PB6
#define SCL_L		GPIOB->BRR = GPIO_Pin_6
#define SCL_H		GPIOB->BSRR = GPIO_Pin_6
//SDA-->PB7
#define SDA_L		GPIOB->BRR = GPIO_Pin_7
#define SDA_H		GPIOB->BSRR = GPIO_Pin_7

//读取SDA线的 输入状态
#define SDA_READ   GPIOB->IDR & GPIO_Pin_7

//设置SDA端口的输出方向:上拉/下拉输入/开漏输出
#define SDA_IN()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define SDA_OUT() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x70000000;}


/***************IIC通信时序基本函数***************/

void IIC_Start (void);//起始信号
void IIC_Stop (void);//停止信号
void IIC_Ack (void);//应答信号
void IIC_NAck (void);//非应答信号
ErrorStatus IIC_Get_Ack (void);//等待从机应答信号
void IIC_ByteWrite (__IO uint8_t BYTE);//发送一个字节数据


/***************IIC通信功能实现函数***************/

//向指定器件、指定寄存器地址发送一个字节数据
ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte);
//发送Length个字节数据
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer);
//发送Length个字节0数据
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length);

#endif
