#include "IIC.h"

/**
	*					写入指定设备、指定寄存器一个字节数据
	* @Brief	
	*	@Param	
						1.Slave_Addr  		从机地址+读写命令
						2.Resister_Addr		要写入的寄存器地址
						3.DataByte   			要发送的字节数据
	* @Retval  	
	**/
ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte)
{	
	I2C_GenerateSTART(I2C1,ENABLE);	//起始信号
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1,Slave_Addr,I2C_Direction_Transmitter); //发送器件I2C地址+写命令
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2C1,Resister_Addr); //发送寄存器地址
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1,DataByte); //发送数据
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1,ENABLE);	//停止信号
	
	return SUCCESS;
}

/**
	*					写入指定设备、指定寄存器多个字节数据
	* @Brief	
	*	@Param	
						1.Slave_Addr：从机IIC地址
						2.Slave_Addr：待写入从机的寄存器地址
						3.Length： 	 	要写入的字节数
						3.pBuffer：  	待写入数据所存放的地址
	* @Retval  
						SUCCESS
						ERROR
	**/
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer)
{	
	if(Length<=0)
		return ERROR;
	
	I2C_GenerateSTART(I2C1,ENABLE);	//起始信号
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)); //EV5
	
	I2C_Send7bitAddress(I2C1,Slave_Addr,I2C_Direction_Transmitter); //发送器件I2C地址+写命令
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //EV6
	
	I2C_SendData(I2C1,Resister_Addr); //发送寄存器地址
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
	
	while (Length--)
	{
		I2C_SendData(I2C1,*pBuffer);	//发送一个数据
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
		++pBuffer;										//指向下一个要发送的地址
	}
	
	I2C_GenerateSTOP(I2C1,ENABLE); //停止信号
	
	return SUCCESS;
}

/**
	*					写入指定设备、指定寄存器多个0字节
	* @Brief	
	*	@Param	
						1.Slave_Addr：从机IIC地址
						2.Slave_Addr：待写入从机的寄存器地址
						3.Length： 	 	要写入的字节数
	* @Retval  
						SUCCESS
						ERROR
	**/
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length)
{	
	if(Length<=0)
		return ERROR;
	
	I2C_GenerateSTART(I2C1,ENABLE);	//起始信号
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)); //EV5
	
	I2C_Send7bitAddress(I2C1,Slave_Addr,I2C_Direction_Transmitter); //发送器件I2C地址+写命令
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //EV6
	
	I2C_SendData(I2C1,Resister_Addr); //发送寄存器地址
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
	
	while (Length--)
	{
		I2C_SendData(I2C1,0x00);	//发送一个数据
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
	}
	
	I2C_GenerateSTOP(I2C1,ENABLE); //停止信号
	
	return SUCCESS;
}
