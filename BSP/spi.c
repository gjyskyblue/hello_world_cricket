#include "spi.h"
#include "usart.h"

//SPI Write and Read
uint16_t SPI_WriteAndRead(SPI_TypeDef* SPIx, uint16_t Data)
{
	uint16_t timeout = 0;
#if NO_OLED_MODE
		return 0;
#endif	
	
	while((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET)
	{
		++timeout;
		if(timeout >= 400)
		{
			return 0;
		}		
	}
	SPIx->DR = Data;
	
	timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET)
		{
		++timeout;
		if(timeout >= 400)
		{
			return 0;
		}
	}
	return (SPIx->DR);
}

//SPI write
bool SPI_Write(SPI_TypeDef* SPIx, uint8_t data)
{
	uint16_t timeout = 0;
#if NO_OLED_MODE
		return true;
#endif	
	while((SPIx->SR & SPI_I2S_FLAG_BSY) != RESET) //繁忙
	{
		++timeout;
		if(timeout >= 400)
		{
			return false;		
		}
	}
	
	SPIx->DR = data;
	
	timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET) //未启动发送
	{
		++timeout;
		if(timeout >= 400)
		{
			return false;
		}
	}
	
	timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_BSY) != RESET) //正在发送
	{
		++timeout;
		if(timeout >= 400)
		{
			return false;		
		}
	}
	
	return true;
}
