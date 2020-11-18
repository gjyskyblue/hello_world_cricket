#ifndef __NONVOLATILE_H
#define __NONVOLATILE_H

#include "calibration.h"
#include "flash.h"
#include "stepper_controller.h"

//枚举类型大小与编译器有关
//在STM32平台/kei 5编译器，默认情况下枚举类型的内存大小与枚举的最大值的字节大小相同
//但最大字节数不超过4字节，超出部分不可用，情况未知
//可以通过编译器设定，强制将任何一种枚举型都转为4字节(C/C++选项中设置)
typedef enum {
	CW_ROTATION = 0,
	CCW_ROTATION = 1,
} RotationDir_t; //sizeof(RotationDir_t)=1

typedef enum {
	ERROR_PIN_MODE_ENABLE = 0,            //error pin works like enable on step sticks
	ERROR_PIN_MODE_ACTIVE_LOW_ENABLE = 1, //error pin works like enable on step sticks
} ErrorPinMode_t; //sizeof(ErrorPinMode_t)=1

typedef enum {
	CTRL_SIMPLE = 0, //simple error controller
	CTRL_POS_PID =1, //PID  Position controller
	CTRL_POS_VELOCITY_PID = 2, //PID  Velocity controller
} feedbackCtrl_t; //sizeof(feedbackCtrl_t)=1

typedef struct {
	__attribute__((__aligned__(4))) float Kp;
	__attribute__((__aligned__(4))) float Ki;
	__attribute__((__aligned__(4))) float Kd;
} PIDparams_t;

typedef struct {
	__attribute__((__aligned__(2))) uint16_t currentMa;   //maximum current for the motor
	__attribute__((__aligned__(2))) uint16_t currentHoldMa; //hold current for the motor
	__attribute__((__aligned__(2))) bool motorWiring;  //forward wiring of motor or reverse
	__attribute__((__aligned__(2))) uint16_t fullStepsPerRotation; //how many full steps per rotation is the motor
	__attribute__((__aligned__(2))) uint16_t parametersValid;
} MotorParams_t; //sizeof(MotorParams_t)=14

typedef struct {
	__attribute__((__aligned__(2))) uint16_t microsteps;    //number of microsteps on the dir/step pin interface from host
	__attribute__((__aligned__(2))) RotationDir_t dirPinRotation;  //is the direction pin high for clockwise or counterClockWise
	__attribute__((__aligned__(2))) uint16_t errorLimit;    //error limit before error pin asserts 65536==360degrees
	__attribute__((__aligned__(2))) ErrorPinMode_t errorPinMode;  //is error pin used for enable, error, or bidirectional
	__attribute__((__aligned__(2))) feedbackCtrl_t controllerMode; //feedback mode for the controller
	__attribute__((__aligned__(2))) uint16_t parametersValid;
} SystemParams_t; //sizeof(SystemParams_t)=18

typedef struct {
	__attribute__((__aligned__(2))) uint16_t angle[16384];
}FastCal_t;

typedef struct {
	SystemParams_t 	SystemParams;
	MotorParams_t 	motorParams;
	PIDparams_t 		sPID; //simple PID parameters
	PIDparams_t 		pPID; //position PID parameters
	PIDparams_t 		vPID; //velocity PID parameters
} nvm_t;

#define NVM										((nvm_t*)NVM_address)	//将NVM_address强制转换为nvm_t指针
#define nvmFlashCalData				((FlashCalData_t*)FLASH_PAGE31_ADDR)
#define nvmFastCal 						((FastCal_t *)FLASH_PAGE32_ADDR)

#define	valid									(uint16_t)0x0001
#define invalid								(uint16_t)0xffff
#define NONVOLATILE_STEPS				((uint32_t)62)		//sizeof(nvm_t) = 60

void nonvolatile_begin(void);
bool nvmWriteCalTable(void *ptrData);
bool nvmWriteConfParms(nvm_t* ptrNVM);

#endif
