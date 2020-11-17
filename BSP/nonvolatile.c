#include "nonvolatile.h"
#include "USART.h"

extern volatile MotorParams_t motorParams;
extern volatile SystemParams_t systemParams;
extern volatile bool TC1_ISR_Enabled;
extern volatile uint32_t fullMicrosteps;

volatile uint32_t NVM_address = FLASH_PAGE30_ADDR;

//求解NVM_address
void nonvolatile_begin(void)
{
	uint32_t i = ((FLASH_PAGE_SIZE / NONVOLATILE_STEPS) - 1); //(1024/62) = 16(0~15)
	
	NVM_address = FLASH_PAGE30_ADDR;
	
	for(i=((FLASH_PAGE_SIZE / NONVOLATILE_STEPS) - 1); i > 0; i--)
	{
		if( Flash_readHalfWord( (FLASH_PAGE30_ADDR + (i * NONVOLATILE_STEPS)) ) != invalid )
		{
			NVM_address = (FLASH_PAGE30_ADDR + (i * NONVOLATILE_STEPS));
			return;
		}
	}
}

bool nvmWriteCalTable(void *ptrData)
{
	bool state = TC1_ISR_Enabled;
	disableTCInterrupts(); //暂时关闭更新中断
	disableINPUTInterrupts();
	
	Flash_ProgramPage(FLASH_PAGE31_ADDR, ptrData, (sizeof(FlashCalData_t)/2));
	
	enableINPUTInterrupts();
	if (state) enableTCInterrupts();
	return true;
}

bool nvmWriteConfParms(nvm_t* ptrNVM)
{		
	bool state = TC1_ISR_Enabled;
	disableTCInterrupts(); //暂时关闭更新中断
	disableINPUTInterrupts();
	
	ptrNVM->motorParams.parametersValid  = valid;
	ptrNVM->SystemParams.parametersValid = valid;
	
	if(Flash_readHalfWord(NVM_address) != invalid && ((NVM_address + NONVOLATILE_STEPS) < FLASH_PAGE31_ADDR))
	{
		NVM_address += NONVOLATILE_STEPS;
		
		//验证NVM_address后面的30个uint16_t都为0xFFFF
		//如果中间有一个uint16_t不为0xffff，则跳过该段，再验证下一段空间
		while( Flash_checknvmFlash(NVM_address) == false )
		{																													 
			//验证失败，进行下一段验证
			if( (NVM_address + NONVOLATILE_STEPS) < FLASH_PAGE31_ADDR )
			{
				NVM_address += NONVOLATILE_STEPS;
			}
			else
			{
				NVM_address = FLASH_PAGE30_ADDR;
				Flash_ProgramPage(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2)); //擦除整页重新写入
				return true;
			}
		}
		//验证成功，不擦除直接进行写入
		Flash_ProgramSize(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2));
	}
	else //擦除整页重新写入
	{
		NVM_address = FLASH_PAGE30_ADDR;
		Flash_ProgramPage(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2));
	}
	
	motorParams = NVM->motorParams; //update motorParams
	systemParams = NVM->SystemParams; //update systemParams
	fullMicrosteps = (uint32_t)(ANGLE_STEPS / systemParams.microsteps);	//重新计算参数
	StepperCtrl_setLocationFromEncoder(); //measure new starting point
	
	enableINPUTInterrupts();
	if (state) enableTCInterrupts();	
	return true;
}
