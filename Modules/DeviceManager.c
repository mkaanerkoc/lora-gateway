/*
 * DeviceManager.c
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */


#include "DeviceManager.h"


void 			InitializeDeviceManager(void)
{

	getDeviceUniqueID(true,NON_THREAD);
	getDeviceLoRaID(true,NON_THREAD);
	getDeviceLoRaGatewayID(true,NON_THREAD);
}


void 			setDeviceUniqueID(uint16_t devID,Context_Enum context)
{
	uint8_t uniqueID[2];
	uniqueID[0] = (uint8_t)(devID>>8);
	uniqueID[1] = (uint8_t)(devID&(0xff));
	EEPROM_WriteBulk(EEPROM_1,E2PROM_DEVICE_UNIQUE_ID_INDEX,uniqueID,2,context);
	deviceUniqueID = devID;
}

uint16_t 		getDeviceUniqueID(bool fromEEPROM, Context_Enum context)
{
	uint8_t _tempLoraGateway[2];

	if( fromEEPROM )
	{
		EEPROM_ReadBulk(EEPROM_1,E2PROM_DEVICE_UNIQUE_ID_INDEX,_tempLoraGateway,2,context);
		deviceUniqueID = (_tempLoraGateway[0]<<8)|(_tempLoraGateway[1]);
	}
	return deviceUniqueID;
}


void 			setDeviceLoRaID( unitNo_t loraIDParam, Context_Enum context)
{
	unitNo_t loraIDTemp;
	loraIDTemp = (unitNo_t)(loraIDParam);
	EEPROM_WriteBulk( EEPROM_1,E2PROM_LORA_UNIQUE_ID_INDEX, &loraIDTemp ,sizeof( unitNo_t ), context);
	deviceLoRaID = loraIDParam;
}

unitNo_t 		getDeviceLoRaID( bool fromEEPROM, Context_Enum context )
{
	unitNo_t _tempLoraGateway;

	if( fromEEPROM )
	{
		EEPROM_ReadBulk(EEPROM_1,E2PROM_LORA_UNIQUE_ID_INDEX, &(_tempLoraGateway), 1 ,context);
		deviceLoRaID = _tempLoraGateway;
	}

	return deviceLoRaID;
}

void 			setDeviceLoRaGatewayID( unitNo_t gatewayID, Context_Enum context )
{
	unitNo_t deviceLoraGatewayId;
	deviceLoraGatewayId  = (unitNo_t)(gatewayID );
	EEPROM_WriteBulk( EEPROM_1, E2PROM_LORA_GATEWAY_ID_INDEX, &deviceLoraGatewayId ,sizeof( unitNo_t ),context);
	deviceLoRaGatewayID = gatewayID;
}

unitNo_t 		getDeviceLoRaGatewayID(bool fromEEPROM, Context_Enum context)
{
	unitNo_t _tempLoraGateway;

	if( fromEEPROM )
	{
		EEPROM_ReadBulk(EEPROM_1,E2PROM_LORA_GATEWAY_ID_INDEX,&(_tempLoraGateway), sizeof( unitNo_t ),context);
		deviceLoRaGatewayID = _tempLoraGateway;
	}
	return deviceLoRaGatewayID;
}


void				checkForPeripherals(void)
{

}
