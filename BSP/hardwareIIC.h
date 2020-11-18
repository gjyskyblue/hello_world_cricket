#ifndef __IIC_H
#define __IIC_H

#include "stm32f10x.h"
#include "delay.h"

/***************IICͨ�Ź���ʵ�ֺ���***************/

//��ָ��������ָ���Ĵ�����ַ����һ���ֽ�����
ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte);
//����Length���ֽ�����
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer);
//����Length���ֽ�0����
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length);

#endif
