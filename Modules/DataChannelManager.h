#ifndef DATA_CHANNEL_MANAGER_H_
#define DATA_CHANNEL_MANAGER_H_

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include <pthread.h>
#include "Radio_Services.h"
#include "Radio_QueueManager.h"
#include <my_global.h>
#include "ModbusTCPSlave.h"
#include "MySQL_ErrorCodes.h"
#include <mysql.h>
#include <time.h>



#define CHANNEL_MANAGER_CHANNEL_COUNT 			64
#define CHANNEL_MANAGER_MINIMUM_STEP_TIME 		30000

#define MAX_MYSQL_QUERY_LENGTH_IN_BYTES	 		1024
#define MAX_CHANNEL_NUMBER_ON_LORA_DEVICE   	24


#define MYSQL_THREAD_POOL_SIZE					24
#define CHANNEL_INFORMATION_TABLE_COLUMN_COUNT 	10

#define INVALID_MODBUS_REGISTER_ADDRESS			0xFFFF

typedef enum ChannelType
{
	DIGITAL_INPUT_CHANNEL,
	DIGITAL_OUTPUT_CHANNEL,
	ANALOG_INPUT_CHANNEL,
	ANALOG_OUTPUT_CHANNEL
}ChannelType;

typedef enum ChannelDataType
{
	S16_TYPE,
	U16_TYPE,
	FLOAT_TYPE,
	BIT_TYPE
}ChannelDataType;

typedef enum ChannelState
{
	CHANNEL_NOT_USED,
	CHANNEL_USED
}ChannelState;

typedef enum ChannelManagerReturnType
{
	CM_ILLEGAL_FUNCTION,
	CM_BUFFER_FULL_CANT_ADD,
	CM_CHANNEL_NON_EXIST,
	CM_THREAD_CREATION_FAILED,
	CM_CHANNEL_ADDED_BEFORE,
	CM_FILE_CANNOT_OPEN,
	CM_SUCCESFUL
}ChannelManagerReturnType;

typedef struct ChannelInfoStr
{
	uint16_t 				channelID;
	unitNo_t 				targetDeviceLoRaID;
	DeviceChannelTypes 		channelEnum;
	ChannelType 			type;
	ChannelState 			state;
	ChannelDataType			dataType;
	uint16_t 				readPeriod;
	uint16_t				readTimeOut;
	uint8_t					msgTryCount;
	uint16_t 				readPeriodIndex;
	uint16_t 				modbusRegister;
}ChannelInfoStr;


typedef struct ChannelValueRowStruct
{
	uint16_t 	deviceID;
	uint8_t		channelCount;
	uint8_t		channelIDs[MAX_CHANNEL_NUMBER_ON_LORA_DEVICE];
	uint8_t 	statusFlags[MAX_CHANNEL_NUMBER_ON_LORA_DEVICE];
	float 		values[MAX_CHANNEL_NUMBER_ON_LORA_DEVICE];
}ChannelValueRowStruct;


typedef union ModbusRegisterUnion
{
    float 			fvalue;
    uint16_t 		uvalue;
    unsigned char 	bytes[4];
    uint16_t 		words[2];
} ModbusRegisterUnion;

//externs
extern void delay_millisecond(unsigned long milliseconds);

//Globals
ChannelInfoStr 	channelsInfo[CHANNEL_MANAGER_CHANNEL_COUNT];
pthread_t 		channelManagerTask,channelManagerDbTask;
uint16_t 		totalActiveChannelsCount;


//Function Prototypes
void InitializeDataChannelManager(void);

ChannelManagerReturnType ChannelManager_AddChannel( unitNo_t deviceLoraID, DeviceChannelTypes channelEnumParam ,ChannelType type, ChannelDataType dataType, uint32_t readPeriod,uint16_t msgTimeout, uint8_t msgTryCount, uint16_t modbusRegister);
ChannelManagerReturnType ChannelManager_LoadChannelsFromDatabase(void);
ChannelManagerReturnType ChannelManager_LoadChannelsFromEEPROM( void );

ChannelManagerReturnType ChannelManager_SetChannel_Value( unitNo_t loraIdParam, DeviceChannelTypes channelEnumParam ,void *setData);
ChannelManagerReturnType ChannelManager_GetChannel_Value( unitNo_t loraIdParam, DeviceChannelTypes channelEnumParam ,void *getData);

ChannelManagerReturnType ChannelManager_SetChannel_ReadPeriod(uint16_t channelID,uint32_t readPeriod);

ChannelManagerReturnType ChannelManager_CreateRequiredDatabaseTables(void);
ChannelManagerReturnType ChannelManager_WriteNewValuesToDatabase(uint16_t deviceLoraID,uint8_t channelCount,uint8_t *channelIds,float *channelValues,uint8_t *channelStatus,  int signalRssiParam );
ChannelManagerReturnType ChannelManager_WriteNewValuesToFileSystem(uint16_t deviceLoraID,uint8_t channelCount,uint8_t *channelIds,float *channelValues,uint8_t *channelStatus ,  int signalRssiParam );


//Task Prototypes
void 			*ChannelManagerTask(void *arg);
void 			*ChannelManagerDatabaseTask(void *arg);
void 			*ChannelManagerCreateRequiredTablesTask(void *arg);
void 			*ChannelManagerLoadChannelTablesTask(void *arg);

#endif 