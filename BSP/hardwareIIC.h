#ifndef __IIC_H
#define __IIC_H

#include "stm32f10x.h"
#include "delay.h"

/***************IIC通信功能实现函数***************/

//向指定器件、指定寄存器地址发送一个字节数据
ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte);
//发送Length个字节数据
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer);
//发送Length个字节0数据
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length);

#endif
