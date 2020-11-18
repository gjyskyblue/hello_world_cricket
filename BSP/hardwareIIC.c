#include "IIC.h"

/**
	*					д��ָ���豸��ָ���Ĵ���һ���ֽ�����
	* @Brief	
	*	@Param	
						1.Slave_Addr  		�ӻ���ַ+��д����
						2.Resister_Addr		Ҫд��ļĴ�����ַ
						3.DataByte   			Ҫ���͵��ֽ�����
	* @Retval  	
	**/
ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte)
{	
	I2C_GenerateSTART(I2C1,ENABLE);	//��ʼ�ź�
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1,Slave_Addr,I2C_Direction_Transmitter); //��������I2C��ַ+д����
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2C1,Resister_Addr); //���ͼĴ�����ַ
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1,DataByte); //��������
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1,ENABLE);	//ֹͣ�ź�
	
	return SUCCESS;
}

/**
	*					д��ָ���豸��ָ���Ĵ�������ֽ�����
	* @Brief	
	*	@Param	
						1.Slave_Addr���ӻ�IIC��ַ
						2.Slave_Addr����д��ӻ��ļĴ�����ַ
						3.Length�� 	 	Ҫд����ֽ���
						3.pBuffer��  	��д����������ŵĵ�ַ
	* @Retval  
						SUCCESS
						ERROR
	**/
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer)
{	
	if(Length<=0)
		return ERROR;
	
	I2C_GenerateSTART(I2C1,ENABLE);	//��ʼ�ź�
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)); //EV5
	
	I2C_Send7bitAddress(I2C1,Slave_Addr,I2C_Direction_Transmitter); //��������I2C��ַ+д����
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //EV6
	
	I2C_SendData(I2C1,Resister_Addr); //���ͼĴ�����ַ
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
	
	while (Length--)
	{
		I2C_SendData(I2C1,*pBuffer);	//����һ������
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
		++pBuffer;										//ָ����һ��Ҫ���͵ĵ�ַ
	}
	
	I2C_GenerateSTOP(I2C1,ENABLE); //ֹͣ�ź�
	
	return SUCCESS;
}

/**
	*					д��ָ���豸��ָ���Ĵ������0�ֽ�
	* @Brief	
	*	@Param	
						1.Slave_Addr���ӻ�IIC��ַ
						2.Slave_Addr����д��ӻ��ļĴ�����ַ
						3.Length�� 	 	Ҫд����ֽ���
	* @Retval  
						SUCCESS
						ERROR
	**/
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length)
{	
	if(Length<=0)
		return ERROR;
	
	I2C_GenerateSTART(I2C1,ENABLE);	//��ʼ�ź�
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)); //EV5
	
	I2C_Send7bitAddress(I2C1,Slave_Addr,I2C_Direction_Transmitter); //��������I2C��ַ+д����
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //EV6
	
	I2C_SendData(I2C1,Resister_Addr); //���ͼĴ�����ַ
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
	
	while (Length--)
	{
		I2C_SendData(I2C1,0x00);	//����һ������
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
	}
	
	I2C_GenerateSTOP(I2C1,ENABLE); //ֹͣ�ź�
	
	return SUCCESS;
}
