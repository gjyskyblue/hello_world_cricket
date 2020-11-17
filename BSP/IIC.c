#include "IIC.h"

/**
	*					产生起始信号START
	* @Brief	
						SCL为1时，SDA由1到0的跳变，表示IIC开始信号
	*	@Param	None
	**/
void IIC_Start (void)
{		
	SDA_H;//复位数据线
	delay_us(1);
	
	SCL_H;//拉高时钟线
	delay_us(1);	
	
	SDA_L;//发送起始信号
	delay_us(1);
	
	SCL_L;//复位时钟线
}


/**
	*					产生停止信号STOP
	* @Brief	
						SCL为1时，SDA由0到1的跳变，表示IIC停止信号
	*	@Param	None
	**/
void IIC_Stop (void)
{		
	SDA_L;//复位数据线
	delay_us(1);
	
	SCL_H;//拉高时钟线
	delay_us(1);
	
	SDA_H;//发送停止信号
	delay_us(1);
	
	SCL_L;//复位时钟线
}


/**
	*					产生应答信号ACK
	* @Brief	
						SCL为1时，SDA为0，表示IIC应答信号
	*	@Param	None
	**/
void IIC_Ack (void)
{
	
	SDA_L;//复位数据线
	delay_us(1);	
	
	SCL_H;//发送应答信号ACK
	delay_us(1);
		
	SCL_L;//复位时钟线
}


/**
	*					产生非应答信号NACK
	* @Brief	
						SCL为1时，SDA为1，表示IIC非应答信号
	*	@Param	None
	**/
void IIC_NAck (void)
{
	SDA_H;//复位数据线
	delay_us(1);
	
	SCL_H;//发送非应答信号NACK
	delay_us(1);
		
	SCL_L;//复位时钟线
}

/**
	*					等待应答信号ACK
	* @Brief	
	*	@Param	
						ERROR			接收应答失败
						SUCCESS		接收应答成功
	**/
ErrorStatus IIC_Get_Ack (void)
{
	uint8_t IIC_ErrTime = 0;
	
	SDA_IN();//SDA配置为上拉/下拉输入
	
	SDA_H;//拉高SDA线
	delay_us(1);
	
	SCL_H;//拉高SCL线
	delay_us(1);
	
	while(SDA_READ)//若从机不应答，则SDA始终为高，通信失败
	{
		++IIC_ErrTime;//统计通信失败时间
		
		if(IIC_ErrTime>10)
		{			
			SCL_L;//复位时钟线
			
			SDA_OUT();//SDA配置为开漏输出
			
			return ERROR;
		}
	}
	SCL_L;//复位时钟线
		
	SDA_OUT();//SDA配置为开漏输出
		
	return SUCCESS;
}


/**
	*					发送一个字节数据
	* @Brief	
						SCL由0到1的跳变，则发送BYTE的一位数据
	*	@Param	
						BYTE：待发送的字节数据
	**/
void IIC_ByteWrite (__IO uint8_t BYTE)
{
	uint8_t x=8;
	
	while(x--)
	{
		if (BYTE & 0x80)//判断BYTE要发送位的状态，进而对SDA进行输出
			SDA_H;
		else
			SDA_L;
		
		BYTE<<=1;//左移一位继续进行判断
		
		delay_us(1);
		
		SCL_H;//SCL由0到1的跳变发送该位
		
		delay_us(1);
			
		SCL_L;//复位时钟线
	}	
}

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
	IIC_Start ();	//起始信号
	
	IIC_ByteWrite (Slave_Addr); //发送器件I2C地址+写命令
	
	IIC_Get_Ack ();	//等待应答
	
	IIC_ByteWrite (Resister_Addr);//发送寄存器地址
	
	IIC_Get_Ack ();	//等待应答
	
	IIC_ByteWrite (DataByte);	//发送数据
	
	IIC_Get_Ack ();	//等待应答
	
	IIC_Stop ();	//停止信号
	
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
	
	IIC_Start () ;	//起始信号
	
	IIC_ByteWrite (Slave_Addr); //发送器件I2C地址+写命令
	
	IIC_Get_Ack ();	//等待应答
	
	IIC_ByteWrite (Resister_Addr);//发送寄存器地址
	
	IIC_Get_Ack ();	//等待应答
	
	while (Length--)
	{
		IIC_ByteWrite ((*pBuffer));	//发送一个数据
		IIC_Get_Ack ();						//等待应答
		++pBuffer;								//指向下一个要发送的地址
	}
	
	IIC_Stop ();	//停止信号
	
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
	
	IIC_Start () ;	//起始信号
	
	IIC_ByteWrite (Slave_Addr); //发送器件I2C地址+写命令
	
	IIC_Get_Ack ();	//等待应答
	
	IIC_ByteWrite (Resister_Addr);//发送寄存器地址
	
	IIC_Get_Ack ();	//等待应答
	
	while (Length--)
	{
		IIC_ByteWrite (0x00);	//发送一个数据
		IIC_Get_Ack ();						//等待应答
	}
	
	IIC_Stop ();	//停止信号
	
	return SUCCESS;
}
