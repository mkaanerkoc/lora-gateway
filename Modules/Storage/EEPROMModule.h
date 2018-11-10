/*
 * EEPROMModule.h
 *
 *  Created on: Jan 10, 2018
 *      Author: Kaan
 */

#ifndef MODULES_EEPROMMODULE_H_
#define MODULES_EEPROMMODULE_H_

/*Includes */
#ifndef GATEWAY_CODE
	#include  "cmsis_os.h"
	#include "stm32l4xx_hal.h"
#endif 

#include 	"stdio.h"
#include  	"stdint.h"
#include 	"stdlib.h"
#include  	"stdbool.h"
#include 	"Common_TypeDefs.h"
#include 	"EEPROM_MemoryMap.h"


/*Externs */
#ifndef GATEWAY_CODE
	extern I2C_HandleTypeDef hi2c3;
#endif 

/*Defines */
#define EEPROM_1_I2C_ADDR
#define EEPROM_2_I2C_ADDR

/*Enums */
typedef enum EEPROM_DeviceNumber
{
	EEPROM_2 = 0,
	EEPROM_1 = 1,
	EEPROM_DeviceCount = 2
}EEPROM_DeviceNumber_t;

/*Globals */
#ifndef GATEWAY_CODE
	SemaphoreHandle_t eepromI2CReadSemaphore;
	SemaphoreHandle_t eepromI2CWriteSemaphore;
#endif 

/*Function Prototypes */
void 			InitializeEEPROMModule(void);


/*User Level Functions */
uint8_t 		EEPROM_CheckStatus(EEPROM_DeviceNumber_t devNum,Context_Enum context);

/*BSP Layer Functions */
void 			EEPROM_ReadBulk(EEPROM_DeviceNumber_t devNum, uint32_t address,uint8_t *data,uint16_t length,Context_Enum context);
void 			EEPROM_WriteBulk(EEPROM_DeviceNumber_t devNum, uint32_t address,uint8_t *data,uint16_t length,Context_Enum context);

uint8_t 		EEPROM_ReadByte(EEPROM_DeviceNumber_t devNum, uint32_t address,Context_Enum context);
bool 			EEPROM_WriteByte(EEPROM_DeviceNumber_t devNum, uint32_t address,uint8_t inData,Context_Enum context);

#ifndef GATEWAY_CODE
void 			EEPROM_SetWriteProtectPin(GPIO_PinState pinState);
uint8_t 		EEPROM_CalculateI2CAddres(EEPROM_DeviceNumber_t  devNum, uint32_t address);
#else
void			EEPROM_CreateInitialConfigFile(void);
#endif

#endif /* MODULES_EEPROMMODULE_H_ */
