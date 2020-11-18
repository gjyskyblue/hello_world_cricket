#include "IIC.h"
//cricket2009_AOC push
//cricket2009_HP push

/**
	*					������ʼ�ź�START
	* @Brief	
						SCLΪ1ʱ��SDA��1��0�����䣬��ʾIIC��ʼ�ź�
	*	@Param	None
	**/
void IIC_Start (void)
{		
	SDA_H;//��λ������
	delay_us(1);
	
	SCL_H;//����ʱ����
	delay_us(1);	
	
	SDA_L;//������ʼ�ź�
	delay_us(1);
	
	SCL_L;//��λʱ����
}


/**
	*					����ֹͣ�ź�STOP
	* @Brief	
						SCLΪ1ʱ��SDA��0��1�����䣬��ʾIICֹͣ�ź�
	*	@Param	None
	**/
void IIC_Stop (void)
{		
	SDA_L;//��λ������
	delay_us(1);
	
	SCL_H;//����ʱ����
	delay_us(1);
	
	SDA_H;//����ֹͣ�ź�
	delay_us(1);
	
	SCL_L;//��λʱ����
}


/**
	*					����Ӧ���ź�ACK
	* @Brief	
						SCLΪ1ʱ��SDAΪ0����ʾIICӦ���ź�
	*	@Param	None
	**/
void IIC_Ack (void)
{
	
	SDA_L;//��λ������
	delay_us(1);	
	
	SCL_H;//����Ӧ���ź�ACK
	delay_us(1);
		
	SCL_L;//��λʱ����
}


/**
	*					������Ӧ���ź�NACK
	* @Brief	
						SCLΪ1ʱ��SDAΪ1����ʾIIC��Ӧ���ź�
	*	@Param	None
	**/
void IIC_NAck (void)
{
	SDA_H;//��λ������
	delay_us(1);
	
	SCL_H;//���ͷ�Ӧ���ź�NACK
	delay_us(1);
		
	SCL_L;//��λʱ����
}

/**
	*					�ȴ�Ӧ���ź�ACK
	* @Brief	
	*	@Param	
						ERROR			����Ӧ��ʧ��
						SUCCESS		����Ӧ��ɹ�
	**/
ErrorStatus IIC_Get_Ack (void)
{
	uint8_t IIC_ErrTime = 0;
	
	SDA_IN();//SDA����Ϊ����/��������
	
	SDA_H;//����SDA��
	delay_us(1);
	
	SCL_H;//����SCL��
	delay_us(1);
	
	while(SDA_READ)//���ӻ���Ӧ����SDAʼ��Ϊ�ߣ�ͨ��ʧ��
	{
		++IIC_ErrTime;//ͳ��ͨ��ʧ��ʱ��
		
		if(IIC_ErrTime>10)
		{			
			SCL_L;//��λʱ����
			
			SDA_OUT();//SDA����Ϊ��©���
			
			return ERROR;
		}
	}
	SCL_L;//��λʱ����
		
	SDA_OUT();//SDA����Ϊ��©���
		
	return SUCCESS;
}


/**
	*					����һ���ֽ�����
	* @Brief	
						SCL��0��1�����䣬����BYTE��һλ����
	*	@Param	
						BYTE�������͵��ֽ�����
	**/
void IIC_ByteWrite (__IO uint8_t BYTE)
{
	uint8_t x=8;
	
	while(x--)
	{
		if (BYTE & 0x80)//�ж�BYTEҪ����λ��״̬��������SDA�������
			SDA_H;
		else
			SDA_L;
		
		BYTE<<=1;//����һλ���������ж�
		
		delay_us(1);
		
		SCL_H;//SCL��0��1�����䷢�͸�λ
		
		delay_us(1);
			
		SCL_L;//��λʱ����
	}	
}

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
	IIC_Start ();	//��ʼ�ź�
	
	IIC_ByteWrite (Slave_Addr); //��������I2C��ַ+д����
	
	IIC_Get_Ack ();	//�ȴ�Ӧ��
	
	IIC_ByteWrite (Resister_Addr);//���ͼĴ�����ַ
	
	IIC_Get_Ack ();	//�ȴ�Ӧ��
	
	IIC_ByteWrite (DataByte);	//��������
	
	IIC_Get_Ack ();	//�ȴ�Ӧ��
	
	IIC_Stop ();	//ֹͣ�ź�
	
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
	
	IIC_Start () ;	//��ʼ�ź�
	
	IIC_ByteWrite (Slave_Addr); //��������I2C��ַ+д����
	
	IIC_Get_Ack ();	//�ȴ�Ӧ��
	
	IIC_ByteWrite (Resister_Addr);//���ͼĴ�����ַ
	
	IIC_Get_Ack ();	//�ȴ�Ӧ��
	
	while (Length--)
	{
		IIC_ByteWrite ((*pBuffer));	//����һ������
		IIC_Get_Ack ();						//�ȴ�Ӧ��
		++pBuffer;								//ָ����һ��Ҫ���͵ĵ�ַ
	}
	
	IIC_Stop ();	//ֹͣ�ź�
	
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
	
	IIC_Start () ;	//��ʼ�ź�
	
	IIC_ByteWrite (Slave_Addr); //��������I2C��ַ+д����
	
	IIC_Get_Ack ();	//�ȴ�Ӧ��
	
	IIC_ByteWrite (Resister_Addr);//���ͼĴ�����ַ
	
	IIC_Get_Ack ();	//�ȴ�Ӧ��
	
	while (Length--)
	{
		IIC_ByteWrite (0x00);	//����һ������
		IIC_Get_Ack ();						//�ȴ�Ӧ��
	}
	
	IIC_Stop ();	//ֹͣ�ź�
	
	return SUCCESS;
}
