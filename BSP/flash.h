#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"
#include "nonvolatile.h"


extern const uint16_t nvm_t_data[30];
extern const uint16_t flash_calTable[259];


#define FLASH_WAIT_TIMEOUT			100000
#define FLASH_PAGE_SIZE   			(1024) //1k bytes
#define FLASH_ROW_SIZE   			  (512)  //1k bytes = 512 uint16_t

#define	FLASH_PAGE21_ADDR				0x08005400	//��21ҳ�洢оƬID��
#define	FLASH_PAGE30_ADDR				0x08007800	//��30ҳ�洢���ò���
#define	FLASH_PAGE31_ADDR				0x08007C00	//��31ҳ�洢CalTable�����4���ֽڴ洢checkSum
#define FLASH_PAGE32_ADDR				0x08008000	//��32-63�洢FastCalTable
#define FLASH_checkSum_ADDR			0x08007FFC	//0x08007FFC-0x08007FFF
#define FLASH_chipID_ADDR				0x080054F0	//0x080054F0-0x080054FB

void Flash_ProgramPage(uint32_t flashAddr, uint16_t* ptrData, uint16_t size);
void Flash_ProgramSize(uint32_t flashAddr, uint16_t* ptrData, uint16_t size);
uint16_t Flash_readHalfWord(uint32_t address);
uint32_t Flash_readWord(uint32_t address);
bool Flash_checknvmFlash(uint32_t address);

#endif
