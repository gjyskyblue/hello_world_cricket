#include "nonvolatile.h"
#include "USART.h"

extern volatile MotorParams_t motorParams;
extern volatile SystemParams_t systemParams;
extern volatile bool TC1_ISR_Enabled;
extern volatile uint32_t fullMicrosteps;

volatile uint32_t NVM_address = FLASH_PAGE30_ADDR;

//���NVM_address
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
	disableTCInterrupts(); //��ʱ�رո����ж�
	disableINPUTInterrupts();
	
	Flash_ProgramPage(FLASH_PAGE31_ADDR, ptrData, (sizeof(FlashCalData_t)/2));
	
	enableINPUTInterrupts();
	if (state) enableTCInterrupts();
	return true;
}

bool nvmWriteConfParms(nvm_t* ptrNVM)
{		
	bool state = TC1_ISR_Enabled;
	disableTCInterrupts(); //��ʱ�رո����ж�
	disableINPUTInterrupts();
	
	ptrNVM->motorParams.parametersValid  = valid;
	ptrNVM->SystemParams.parametersValid = valid;
	
	if(Flash_readHalfWord(NVM_address) != invalid && ((NVM_address + NONVOLATILE_STEPS) < FLASH_PAGE31_ADDR))
	{
		NVM_address += NONVOLATILE_STEPS;
		
		//��֤NVM_address�����30��uint16_t��Ϊ0xFFFF
		//����м���һ��uint16_t��Ϊ0xffff���������öΣ�����֤��һ�οռ�
		while( Flash_checknvmFlash(NVM_address) == false )
		{																													 
			//��֤ʧ�ܣ�������һ����֤
			if( (NVM_address + NONVOLATILE_STEPS) < FLASH_PAGE31_ADDR )
			{
				NVM_address += NONVOLATILE_STEPS;
			}
			else
			{
				NVM_address = FLASH_PAGE30_ADDR;
				Flash_ProgramPage(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2)); //������ҳ����д��
				return true;
			}
		}
		//��֤�ɹ���������ֱ�ӽ���д��
		Flash_ProgramSize(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2));
	}
	else //������ҳ����д��
	{
		NVM_address = FLASH_PAGE30_ADDR;
		Flash_ProgramPage(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2));
	}
	
	motorParams = NVM->motorParams; //update motorParams
	systemParams = NVM->SystemParams; //update systemParams
	fullMicrosteps = (uint32_t)(ANGLE_STEPS / systemParams.microsteps);	//���¼������
	StepperCtrl_setLocationFromEncoder(); //measure new starting point
	
	enableINPUTInterrupts();
	if (state) enableTCInterrupts();	
	return true;
}
