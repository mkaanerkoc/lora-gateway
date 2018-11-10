/*
 * Common_TypeDefsContants.h
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#ifndef COMMON_TYPEDEFS_H_
#define COMMON_TYPEDEFS_H_

#include "stdint.h"

#define MAX_RX_COMMAND_LENGTH 1024

/* Defines */
#define COMMAND_STARTER_1 		0x03
#define COMMAND_STARTER_2   	0x04
#define COMMAND_FINISHER_1  	0x33
#define COMMAND_FINISHER_2  	0x34

typedef enum Context_Enum
{
	THREAD = 0,
	NON_THREAD = 1,
	FROM_ISR = 2
}Context_Enum;

typedef enum Peripheral_Status
{
	NON_EXIST = 0,
	EXIST_CANNOT_RCV_ISR = 1,
	EXIST_SUCCESS = 2
}Peripheral_Status;

typedef struct DeviceConfiguration
{
	uint16_t UniqueID;
	uint16_t LoRaID;
	uint16_t LoRaGatewayID;
}DeviceConfiguration;

/*Structs */
typedef struct MessageDatagram
{
	uint16_t MessageID;
	uint16_t Length;
	uint16_t MessageCRC;
	uint8_t Data[MAX_RX_COMMAND_LENGTH];
}MessageDatagram;

typedef enum DeviceChannelTypes
{

	BME280_TEMPERATURE  		= 1,
	BME280_HUMIDITY     		= 2,
	BME280_AIR_PRESSURE 		= 3,
	BME280_ALTITUDE     		= 4,

	MAX31865_1_TEMPERATURE  	= 5,
	MAX31865_2_TEMPERATURE  	= 6,

	GPS_LONGTITUDE 				= 7,
	GPS_LATITUDE   				= 8,
	GPS_COS						= 9,

	INA219_SUPPLY_CURRENT 		= 10,
	INA219_SOLAR_CURRENT 		= 11,

	INA219_SUPPLY_VOLTAGE 		= 12,
	INA219_SOLAR_VOLTAGE 		= 13,

	SIGNAL_RSSI_VALUE			= 14

}DeviceChannelTypes;

#ifdef GATEWAY_CODE
	typedef uint32_t TickType_t;
#endif

typedef uint8_t 	unitNo_t;


#endif /* COMMON_TYPEDEFS_H_ */
