#include "DataChannelManager.h"

float values[3] = {0.9231,12.312,4.33};
uint8_t channelIdsGlobal[3] = {1,2,3};

ChannelValueRowStruct rowStructTemp;
pthread_attr_t tattr;

// Thread Pool Thread Array
pthread_t 		threadPoolArray[MYSQL_THREAD_POOL_SIZE];
uint8_t			threadPoolIndex;

pthread_t 		getFromDatabaseTask;



uint16_t 	lastAddedInputRegisterIndex;
uint16_t 	lastAddedHoldingRegisterIndex;

void InitializeDataChannelManager(void)
{
	printf("<DataChannelManager> Initialization ! \n");
	lastAddedInputRegisterIndex 	= 0;
	lastAddedHoldingRegisterIndex 	= 0;
	//ChannelManager_LoadChannelsFromDatabase();

	Modbus_AddRegister(100, INPUT_REGISTER_TYPE); /* 5. cihaz device ID */
	Modbus_AddRegister(101, INPUT_REGISTER_TYPE); /* 5. Cihaz RSSI value */
	ChannelManager_AddChannel(5	, BME280_TEMPERATURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		102 );
	ChannelManager_AddChannel(5	, BME280_AIR_PRESSURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS );
	ChannelManager_AddChannel(5	, BME280_ALTITUDE			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS );
	ChannelManager_AddChannel(5 , MAX31865_2_TEMPERATURE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	104 );
	ChannelManager_AddChannel(5 , GPS_LONGTITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	106 );
	ChannelManager_AddChannel(5 , GPS_LATITUDE 				, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	108 );
	ChannelManager_AddChannel(5 , GPS_COS				 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	110 );
	ChannelManager_AddChannel(5 , INA219_SUPPLY_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	112 );
	ChannelManager_AddChannel(5 , INA219_SOLAR_CURRENT 		, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	114 );
	ChannelManager_AddChannel(5 , INA219_SUPPLY_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	116 );
	ChannelManager_AddChannel(5 , INA219_SOLAR_VOLTAGE 		, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	118 );
	ChannelManager_AddChannel(5	, BME280_HUMIDITY			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		120 );

	Modbus_AddRegister(200, INPUT_REGISTER_TYPE); /* 10. cihaz device ID */
	Modbus_AddRegister(201, INPUT_REGISTER_TYPE); /* 10. Cihaz RSSI value */
	ChannelManager_AddChannel(10 , BME280_TEMPERATURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		202);
	ChannelManager_AddChannel(10 , BME280_AIR_PRESSURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS );
	ChannelManager_AddChannel(10 , BME280_ALTITUDE			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS );
	ChannelManager_AddChannel(10 , MAX31865_2_TEMPERATURE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	204 );
	ChannelManager_AddChannel(10 , GPS_LONGTITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	206 );
	ChannelManager_AddChannel(10 , GPS_LATITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	208 );
	ChannelManager_AddChannel(10 , GPS_COS				 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	210 );
	ChannelManager_AddChannel(10 , INA219_SUPPLY_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	212 );
	ChannelManager_AddChannel(10 , INA219_SOLAR_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	214 );
	ChannelManager_AddChannel(10 , INA219_SUPPLY_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	216 );
	ChannelManager_AddChannel(10 , INA219_SOLAR_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	218 );
	ChannelManager_AddChannel(10 , BME280_HUMIDITY			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		220 );


	Modbus_AddRegister(300, INPUT_REGISTER_TYPE); /* 15. cihaz device ID */
	Modbus_AddRegister(301, INPUT_REGISTER_TYPE); /* 15. Cihaz RSSI value */
	ChannelManager_AddChannel(15 , BME280_TEMPERATURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		302);
	ChannelManager_AddChannel(15 , BME280_AIR_PRESSURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS );
	ChannelManager_AddChannel(15 , BME280_ALTITUDE			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS );
	ChannelManager_AddChannel(15 , MAX31865_2_TEMPERATURE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	304);
	ChannelManager_AddChannel(15 , GPS_LONGTITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	306);
	ChannelManager_AddChannel(15 , GPS_LATITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	308);
	ChannelManager_AddChannel(15 , GPS_COS				 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	310);
	ChannelManager_AddChannel(15 , INA219_SUPPLY_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	312);
	ChannelManager_AddChannel(15 , INA219_SOLAR_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	314);
	ChannelManager_AddChannel(15 , INA219_SUPPLY_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	316);
	ChannelManager_AddChannel(15 , INA219_SOLAR_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	318);
	ChannelManager_AddChannel(15 , BME280_HUMIDITY			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		320 );

	Modbus_AddRegister(400, INPUT_REGISTER_TYPE); /* 20. cihaz device ID */
	Modbus_AddRegister(401, INPUT_REGISTER_TYPE); /* 20. Cihaz RSSI value */
	ChannelManager_AddChannel(20 , BME280_TEMPERATURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		402);
	ChannelManager_AddChannel(20 , BME280_AIR_PRESSURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS);
	ChannelManager_AddChannel(20 , BME280_ALTITUDE			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS);
	ChannelManager_AddChannel(20 , MAX31865_2_TEMPERATURE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	404);
	ChannelManager_AddChannel(20 , GPS_LONGTITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	406);
	ChannelManager_AddChannel(20 , GPS_LATITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	408);
	ChannelManager_AddChannel(20 , GPS_COS				 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	410);
	ChannelManager_AddChannel(20 , INA219_SUPPLY_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	412);
	ChannelManager_AddChannel(20 , INA219_SOLAR_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	414);
	ChannelManager_AddChannel(20 , INA219_SUPPLY_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	416);
	ChannelManager_AddChannel(20 , INA219_SOLAR_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	418);
	ChannelManager_AddChannel(20 , BME280_HUMIDITY			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		420);



	Modbus_AddRegister(500, INPUT_REGISTER_TYPE); /* 25. cihaz device ID */
	Modbus_AddRegister(501, INPUT_REGISTER_TYPE); /* 25. Cihaz RSSI value */
	ChannelManager_AddChannel(25 , BME280_TEMPERATURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		502);
	ChannelManager_AddChannel(25 , BME280_AIR_PRESSURE		, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS);
	ChannelManager_AddChannel(25 , BME280_ALTITUDE			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		INVALID_MODBUS_REGISTER_ADDRESS);
	ChannelManager_AddChannel(25 , MAX31865_2_TEMPERATURE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	504);
	ChannelManager_AddChannel(25 , GPS_LONGTITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	506);
	ChannelManager_AddChannel(25 , GPS_LATITUDE 			, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	508);
	ChannelManager_AddChannel(25 , GPS_COS				 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	510);
	ChannelManager_AddChannel(25 , INA219_SUPPLY_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	512);
	ChannelManager_AddChannel(25 , INA219_SOLAR_CURRENT 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	514);
	ChannelManager_AddChannel(25 , INA219_SUPPLY_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	516);
	ChannelManager_AddChannel(25 , INA219_SOLAR_VOLTAGE 	, ANALOG_INPUT_CHANNEL, FLOAT_TYPE, 60000   , 2000, 3,   	518);
	ChannelManager_AddChannel(25 , BME280_HUMIDITY			, ANALOG_INPUT_CHANNEL,	FLOAT_TYPE,	60000	, 2000, 3,		520);



	pthread_attr_init(&tattr);
	int ret;
	/* set the thread detach state */
	ret = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
	pthread_create(&channelManagerTask,NULL,ChannelManagerTask,NULL);
	ChannelManager_CreateRequiredDatabaseTables();
	threadPoolIndex = 0;
	//ChannelManager_WriteNewValuesToDatabase(12,2,channelIdsGlobal,values);

	return;
}



ChannelManagerReturnType ChannelManager_AddChannel( unitNo_t deviceLoraID, DeviceChannelTypes channelEnumParam ,ChannelType typeParam, ChannelDataType dataTypeParam, uint32_t readPeriodParam ,uint16_t msgTimeout, uint8_t msgTryCount, uint16_t modbusRegisterParam)
{
	ChannelManagerReturnType retVal = CM_ILLEGAL_FUNCTION;
	ChannelInfoStr *channelPtr;

	if(totalActiveChannelsCount == CHANNEL_MANAGER_CHANNEL_COUNT)
	{
		retVal = CM_BUFFER_FULL_CANT_ADD;
		return retVal;
	}

	for ( int k = 0 ; k < CHANNEL_MANAGER_CHANNEL_COUNT ; k++ )
	{
		if ( deviceLoraID == channelsInfo[ k ].targetDeviceLoRaID && channelEnumParam == channelsInfo[ k ].channelEnum && CHANNEL_USED == channelsInfo[ k ].state )
		{
			retVal = CM_CHANNEL_ADDED_BEFORE;
			return retVal;
		}
	}


	channelPtr = &(channelsInfo[totalActiveChannelsCount++]);
	// fill params
	channelPtr->channelID = totalActiveChannelsCount;
	channelPtr->targetDeviceLoRaID = deviceLoraID;
	channelPtr->channelEnum = channelEnumParam;
	channelPtr->type = typeParam;
	channelPtr->dataType = dataTypeParam;
	channelPtr->state = CHANNEL_NOT_USED;
	channelPtr->readPeriod = (readPeriodParam - (readPeriodParam % CHANNEL_MANAGER_MINIMUM_STEP_TIME)) / CHANNEL_MANAGER_MINIMUM_STEP_TIME;

	if(channelPtr->readPeriod == 0)
	{
		channelPtr->readPeriod = 1;
	}

	channelPtr->readPeriodIndex = channelPtr->readPeriod;

	//kaanbak, buralari biraz daha doldurmak gerekecek
	channelPtr->modbusRegister = modbusRegisterParam;
	if ( modbusRegisterParam != INVALID_MODBUS_REGISTER_ADDRESS ) 
	{
		switch(channelPtr->dataType)
		{
			case U16_TYPE:
				Modbus_AddRegister(channelPtr->modbusRegister, INPUT_REGISTER_TYPE);
				break;
			case FLOAT_TYPE:
				Modbus_AddRegister(channelPtr->modbusRegister,INPUT_REGISTER_TYPE);
				Modbus_AddRegister((channelPtr->modbusRegister+1),INPUT_REGISTER_TYPE);
				break;
			default:
				break;
		}
	}
	retVal = CM_SUCCESFUL;
	return retVal;
}

//kaanbak : return value' lar ayarlanacak
ChannelManagerReturnType ChannelManager_LoadChannelsFromDatabase(void)
{
	ChannelManagerReturnType retVal = CM_SUCCESFUL;
	printf("Channel reading started\n");
	MYSQL *con = mysql_init(NULL);
	if (mysql_real_connect(con, "localhost", "monty", "some_pass", NULL, 0, NULL, 0) == NULL) 
	{
		//fprintf(stderr, "%s\n", mysql_error(con));
		printf("DB Connection Error : %s\n", mysql_error(con));
		MySQL_ErrorCode errcode = (MySQL_ErrorCode)mysql_errno(con);
		mysql_close(con);
		//pthread_exit(1);
		return retVal;
	}
	if(mysql_query(con,"USE LoRaGatewayDB"))
	{
		printf("Database could not selected -> %s\n", mysql_error(con));
		mysql_close(con);
		return retVal;
	}
	if(mysql_query(con,"SELECT * FROM `DeviceChannelInformation`"))
	{
		printf("Table Read Error : %s\n", mysql_error(con));
		MySQL_ErrorCode errcode = (MySQL_ErrorCode)mysql_errno(con);
		mysql_close(con);
		//pthread_exit(2);	
		return retVal;
	}
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL) 
	{
		printf("Result is NULL \n");
		mysql_close(con);
		//pthread_exit(3);
		return retVal;
	}
	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	if(num_fields == CHANNEL_INFORMATION_TABLE_COLUMN_COUNT)
	{
		while ((row = mysql_fetch_row(result))) 
	  	{
	  		ChannelManager_AddChannel( atoi(row[2]),atoi(row[3]),atoi(row[4]),atoi(row[5]),atoi(row[6])*1000,atoi(row[7]),atoi(row[8]),atoi(row[9]));
	  	}
	}
	printf("Channels read succesfully\n");
  	mysql_free_result(result);
  	mysql_close(con);
  	//pthread_exit(0);
  	return retVal;
}

ChannelManagerReturnType ChannelManager_LoadChannelsFromEEPROM( void )
{
	ChannelManagerReturnType retValTemp;
	uint8_t dataChannelsDataBuffer[ sizeof(ChannelInfoStr) * CHANNEL_MANAGER_CHANNEL_COUNT ];
	EEPROM_ReadBulk( EEPROM_1 , E2PROM_DATA_CHANNELS_INDEX, dataChannelsDataBuffer,  E2PROM_DATA_CHANNELS_LENGTH , THREAD );
	//memcpy ( &)
	return retValTemp;
}


ChannelManagerReturnType ChannelManager_WriteNewValuesToDatabase(uint16_t deviceLoraIDParam,uint8_t channelCountParam,uint8_t *channelIdsParam,float *channelValuesParam,uint8_t *channelStatusParam, int signalRssiParam )
{
	int ret;
	int rc;
	ChannelManagerReturnType retVal;
	ModbusRegisterUnion regUnionTemp;
	pthread_attr_t tattr;
	//Update Modbus Registers here !
	ChannelInfoStr *channelPtr;

	//
	for ( int j = 0 ; j < totalActiveChannelsCount ; j ++)
	{
		channelPtr = &(channelsInfo[j]); // get channel info
		if( channelPtr->targetDeviceLoRaID == deviceLoraIDParam )
		{
			for ( int i = 0 ; i < channelCountParam ; i++)
			{
				if( channelPtr->channelEnum == channelIdsParam[i])
				{
					//printf("Channel-Register Match -> channel Id : %d, targetDeviceChannelId : %d, channelIdsParam :%d  \n",channelPtr->channelID,channelPtr->targetDeviceChannelID,channelIdsParam[i]);
					switch(channelPtr->dataType)
					{
						case U16_TYPE:
							regUnionTemp.uvalue = channelValuesParam[i];
							Modbus_SetRegisterValue(channelPtr->modbusRegister 	,INPUT_REGISTER_TYPE,regUnionTemp.words[0]);
							break;
						case FLOAT_TYPE:
							regUnionTemp.fvalue = channelValuesParam[i];
							Modbus_SetRegisterValue(channelPtr->modbusRegister 	,INPUT_REGISTER_TYPE,regUnionTemp.words[0]);
							Modbus_SetRegisterValue(channelPtr->modbusRegister+1,INPUT_REGISTER_TYPE,regUnionTemp.words[1]);
							break;
						default:
							break;
					}
				}
			}
		}
	}
	/* set the thread detach state */
	pthread_attr_init(&tattr);
	ret = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);

	rowStructTemp.deviceID = deviceLoraIDParam;
	rowStructTemp.channelCount = channelCountParam;
	for(int i = 0 ; i < channelCountParam ; i++)
	{
		rowStructTemp.channelIDs[i] = channelIdsParam[i];
		rowStructTemp.statusFlags[i] = channelStatusParam[i];
		rowStructTemp.values[i] = channelValuesParam[i];
		

	}
	if(threadPoolIndex >= MYSQL_THREAD_POOL_SIZE)
	{
		printf("Error ! MySQL Thread Pool is full ! cannot create new thread \n");
		return;
	}
	else
	{
		printf("WriteNewValuesToDatabase -> threadPool Index : %d\n",threadPoolIndex);
	}
	if(rc = pthread_create(&(threadPoolArray[threadPoolIndex]),&tattr,ChannelManagerDatabaseTask,(void*)&rowStructTemp))
	{
		printf("Error : %d ! While creating database write thread ! \n",rc);
		return CM_THREAD_CREATION_FAILED;
	}
	else
	{
		threadPoolIndex++;
	}
	//pthread_detach(pthread_self());
	retVal = CM_SUCCESFUL;
	return retVal;
}

ChannelManagerReturnType ChannelManager_WriteNewValuesToFileSystem(uint16_t deviceLoraIDParam ,uint8_t channelCountParam ,uint8_t *channelIdsParam ,float *channelValuesParam ,uint8_t *channelStatusParam ,  int signalRssiParam )
{
	ChannelManagerReturnType 	retValueTemp;
	ChannelInfoStr				*channelPtr;
	TimeStruct_t 				timeStTemp;
	DateStruct_t				dateStTemp;
	FILE 		 				*fileTemp;
	char 						applicationDirectory[64] = "/home/pi/lora-gateway/";
	char 						fileNameTemp[128];
	char 						fileNewRowStrTemp[512];

	RTC_GetTime(&timeStTemp);
	RTC_GetDate(&dateStTemp);

	// kaanbak: burasi cok amelece oldu simdilik boyle kalsin...
	uint16_t rssiValueRegisterTemp = 0;
	uint16_t devIdRegisterTemp = 0;
	bool updateCommunicationRegisters = true; // bu flag ile bu yaptigimiz amelece cozum sonucu paketi gonderen cihazin bizim 
												// cihazlar listesnde olup olmadiinin kontrolu gibi bisi bu...degistirilmesi gerek device tree yapilmali//
	switch ( deviceLoraIDParam )
	{
		case 5:
			devIdRegisterTemp = 100;
			rssiValueRegisterTemp = 101;
		break;
		case 10:
			devIdRegisterTemp = 200;
			rssiValueRegisterTemp = 201;
		break;
		case 15:
			devIdRegisterTemp = 300;
			rssiValueRegisterTemp = 301;
		break;
		case 20:
			devIdRegisterTemp = 400;
			rssiValueRegisterTemp = 401;
		break;
		case 25:
			devIdRegisterTemp = 500;
			rssiValueRegisterTemp = 501;
		break;
		/*case 30:
			devIdRegisterTemp = 600;
			rssiValueRegisterTemp = 601;
		break;
		case 35:
			devIdRegisterTemp = 700;
			rssiValueRegisterTemp = 701;
		break;
		case 40:
			devIdRegisterTemp = 800;
			rssiValueRegisterTemp = 801;
		break;*/
		default:
			updateCommunicationRegisters = false; /* listemizde yok...*/
			break;
	}
	// update modbus registers 
	if ( updateCommunicationRegisters )
	{
		Modbus_SetRegisterValue( devIdRegisterTemp, INPUT_REGISTER_TYPE, deviceLoraIDParam);
		Modbus_SetRegisterValue( rssiValueRegisterTemp, INPUT_REGISTER_TYPE, signalRssiParam);

		for ( int j = 0 ; j < totalActiveChannelsCount ; j ++)
		{
			channelPtr = &(channelsInfo[j]); // get channel info
			if ( channelPtr->modbusRegister != INVALID_MODBUS_REGISTER_ADDRESS ) // channel has modbus output
			{
				if( channelPtr->targetDeviceLoRaID == deviceLoraIDParam )
				{
					for ( int i = 0 ; i < channelCountParam ; i++)
					{
						if( channelPtr->channelEnum == channelIdsParam[i])
						{
							//printf("Channel-Register Match -> channel Id : %d, targetDeviceChannelId : %d, channelIdsParam :%d  \n",channelPtr->channelID,channelPtr->targetDeviceChannelID,channelIdsParam[i]);
							
							ModbusRegisterUnion regUnionTemp;
							switch(channelPtr->dataType)
							{
								case U16_TYPE:
									regUnionTemp.uvalue = channelValuesParam[i];
									Modbus_SetRegisterValue(channelPtr->modbusRegister 	,INPUT_REGISTER_TYPE,regUnionTemp.words[0]);
									break;
								case FLOAT_TYPE:
									regUnionTemp.fvalue = channelValuesParam[i];
									Modbus_SetRegisterValue(channelPtr->modbusRegister 	,INPUT_REGISTER_TYPE,regUnionTemp.words[0]);
									Modbus_SetRegisterValue(channelPtr->modbusRegister+1,INPUT_REGISTER_TYPE,regUnionTemp.words[1]);
									break;
								default:
									break;
							}
						}
					}
				}
			}
		}
	}
	
	// create file name
	sprintf(fileNameTemp,"%sChannel_Logs_%02d_%02d_%d.csv",applicationDirectory,dateStTemp.day,dateStTemp.month,dateStTemp.year);
	
	fileTemp = fopen((const char*)fileNameTemp,"a+");
	if ( fileTemp == NULL )
	{
		retValueTemp = CM_FILE_CANNOT_OPEN;
		return retValueTemp;
	}
	// put time , deviceLoraId, channel Count
	sprintf(fileNewRowStrTemp,"%02d:%02d:%02d;%02d;%d;%d",timeStTemp.hour,timeStTemp.minute,timeStTemp.second,deviceLoraIDParam, signalRssiParam, channelCountParam );
	//sprintf(fileNewRowStrTemp,"%02d:%02d:%02d;%02d;%d",timeStTemp.hour,timeStTemp.minute,timeStTemp.second,deviceLoraIDParam, channelCountParam );
	
	for ( int k = 0 ; k < channelCountParam ; k ++)
	{
		sprintf(fileNewRowStrTemp,"%s;%d;%.3f;%d",fileNewRowStrTemp,channelIdsParam[ k ], channelValuesParam[ k ], channelStatusParam[ k ] );
	}
	// add data buffer and new line char
	fprintf(fileTemp,"%s",fileNewRowStrTemp);
	fprintf(fileTemp,"\n");
	// close the file..
	fclose(fileTemp);
	retValueTemp = CM_SUCCESFUL;
	return retValueTemp;
}

ChannelManagerReturnType ChannelManager_CreateRequiredDatabaseTables(void)
{
	ChannelManagerReturnType retVal;
	pthread_create(&channelManagerDbTask,NULL,ChannelManagerCreateRequiredTablesTask,NULL);
	retVal = CM_SUCCESFUL;
	return retVal;
}

void 			*ChannelManagerTask(void *arg)
{
	ChannelInfoStr *channelPtr;
	ChannelInfoStr *channelPtrSearch;
	uint8_t		channelsOnSameDeviceBuffer[CHANNEL_MANAGER_CHANNEL_COUNT];
	uint16_t 	channelsCountOnSameDevice=0x00;
	uint16_t 	deviceIDTemp = 0;
	uint32_t 	globalCounter = 0;
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	printf ( "Current local time and date: %s", asctime (timeinfo) );
	printf("The differ to the discrete min : %d  second..\n",(60-timeinfo->tm_sec));
	delay_millisecond((60-timeinfo->tm_sec)*1000);


	while(1)
	{
		delay_millisecond(CHANNEL_MANAGER_MINIMUM_STEP_TIME);
		globalCounter++;
		//printf("ChannelManager Task triggered  Time : %d! \n",globalCounter*CHANNEL_MANAGER_MINIMUM_STEP_TIME);
		channelsCountOnSameDevice = 0;
		memset(channelsOnSameDeviceBuffer,0,sizeof(uint16_t)*CHANNEL_MANAGER_CHANNEL_COUNT);
		for(int i = 0; i < totalActiveChannelsCount ; i++)
		{
			channelPtr = &(channelsInfo[i]);
			channelPtr->readPeriodIndex--;
		}
		for(int i = 0 ; i < totalActiveChannelsCount ; i++)
		{
			channelPtr = &(channelsInfo[i]);
			channelsCountOnSameDevice = 0;
			if(channelPtr->readPeriodIndex == 0)
			{
				channelPtr->readPeriodIndex = channelPtr->readPeriod;
				deviceIDTemp = channelPtr->targetDeviceLoRaID;
				channelsOnSameDeviceBuffer[channelsCountOnSameDevice++] = channelPtr->channelEnum;
				for(int j = 0 ; j < totalActiveChannelsCount ; j++)
				{
					channelPtrSearch = &(channelsInfo[j]);
					if(channelPtrSearch->readPeriodIndex == 0 && channelPtrSearch->targetDeviceLoRaID == deviceIDTemp)
					{
						channelPtrSearch->readPeriodIndex = channelPtrSearch->readPeriod;
						channelsOnSameDeviceBuffer[channelsCountOnSameDevice++] = channelPtrSearch->channelEnum;
					}
				}
				// Send DATA_CHANNEL_REQUEST on LoRa...
				Radio_SendDataChannelGetValuesRequest(deviceIDTemp,channelsCountOnSameDevice,channelsOnSameDeviceBuffer,1000,3); //kaanbak, burada ortak bi timeout ve retry bulunmali
				printf( "Read Period For Device : %d, Channels : %d",deviceIDTemp,channelsOnSameDeviceBuffer[0]);
				for(int k = 1 ; k < channelsCountOnSameDevice ; k++)
				{
					printf( ", %d",channelsOnSameDeviceBuffer[k]);
				} 
				printf( "\n");
			}
		}
	}
}

void			*ChannelManagerDatabaseTask(void *arg)
{
	ChannelValueRowStruct *rowStructTemp;
	rowStructTemp = (ChannelValueRowStruct*)arg;
	char mysql_query_main[MAX_MYSQL_QUERY_LENGTH_IN_BYTES];
	char mysql_query_temp[MAX_MYSQL_QUERY_LENGTH_IN_BYTES];
	MYSQL *con = mysql_init(NULL);
	if (mysql_real_connect(con, "localhost", "monty", "some_pass", NULL, 0, NULL, 0) == NULL) 
	{
		//fprintf(stderr, "%s\n", mysql_error(con));
		printf("%s\n", mysql_error(con));
		MySQL_ErrorCode errcode = (MySQL_ErrorCode)mysql_errno(con);
  		if(errcode == ER_BAD_DB_ERROR)
  		{
			if (mysql_query(con, "CREATE DATABASE LoRaGatewayDB")) 
			{
				printf("Database could not created -> %s\n", mysql_error(con));
				mysql_close(con);
				threadPoolIndex--;
				return NULL;
			}
  		}
  		else
  		{
  			printf("Init error differs from ER_BAD_DB_ERROR -> %s\n", mysql_error(con));
			mysql_close(con);
			threadPoolIndex--;
			return NULL;
  		}
	}  
	else
	{
		//printf("Connected Database succesfully\n");
		if(mysql_query(con,"CREATE DATABASE IF NOT EXISTS LoRaGatewayDB"))
		{
			printf("Database could not created -> %s\n", mysql_error(con));
			mysql_close(con);
			threadPoolIndex--;
			return;
		}
		if(mysql_query(con,"USE LoRaGatewayDB"))
		{
			printf("Database could not selected -> %s\n", mysql_error(con));
			mysql_close(con);
			threadPoolIndex--;
			return;
		}
	}
	// populate sql query 
	sprintf(mysql_query_main,"INSERT INTO DeviceChannelValues (deviceID,dateTime");
	for(int i = 0 ; i  < rowStructTemp->channelCount;i++)
	{
		sprintf(mysql_query_temp,",channelValue%d,channelStatus%d",(i+1),(i+1)); // set column names to be set
		strcat(mysql_query_main,mysql_query_temp);
	}
	strcat(mysql_query_main,") VALUES (");
	sprintf(mysql_query_temp,"%d,NULL",rowStructTemp->deviceID);
	strcat(mysql_query_main,mysql_query_temp);
	//printf("mysql_query_main : %s\n",mysql_query_main);
	for(int i = 0 ; i  < rowStructTemp->channelCount;i++)
	{
		sprintf(mysql_query_temp,",%f,%d",rowStructTemp->values[i],rowStructTemp->statusFlags[i]);
		strcat(mysql_query_main,mysql_query_temp);
	}
	strcat(mysql_query_main,")");
	//printf("mysql_query_main : %s\n",mysql_query_main);
  	if (mysql_query(con, mysql_query_main)) 
  	{
  		MySQL_ErrorCode errcode = (MySQL_ErrorCode)mysql_errno(con);
      	printf("%s\n", mysql_error(con));
      	printf("ERROR CODE : %d \n",mysql_errno(con));
		mysql_close(con);
		threadPoolIndex--;
		return NULL;
  	}
  	else
  	{
  		printf("Row Added succesfully\n");
  		mysql_close(con);
  		threadPoolIndex--;
		return NULL;
  	}
}

void 			*ChannelManagerCreateRequiredTablesTask(void *arg)
{
	char mysql_query_temp[MAX_MYSQL_QUERY_LENGTH_IN_BYTES];
	MYSQL *con = mysql_init(NULL);

	// group channels by device id's
	for(int i = 0 ; i < totalActiveChannelsCount ; i++)
	{
		for(int j = 0 ; j < totalActiveChannelsCount ; j++)
		{

		}	
	}
	sprintf(mysql_query_temp, "CREATE TABLE `LoRaGatewayDB`.`DeviceChannelValues` ( `deviceID` INT NOT NULL , `dateTime` DATE NOT NULL , `channelValue1` FLOAT NOT NULL , `channelStatus1` INT NOT NULL , `channelValue2` FLOAT NOT NULL , `channelStatus2` INT NOT NULL ) IF NOT EXISTS ENGINE = InnoDB;");
	if (mysql_real_connect(con, "localhost", "monty", "some_pass", NULL, 0, NULL, 0) == NULL) 
	{
		//fprintf(stderr, "%s\n", mysql_error(con));
		printf("%s\n", mysql_error(con));
		MySQL_ErrorCode errcode = (MySQL_ErrorCode)mysql_errno(con);
  		if(errcode == ER_BAD_DB_ERROR)
  		{
			if (mysql_query(con, "CREATE DATABASE LoRaGatewayDB")) 
			{
				printf("Database could not created -> %s\n", mysql_error(con));
				mysql_close(con);
				return;
			}
  		}
  		else
  		{
  			printf("Init error differs from ER_BAD_DB_ERROR -> %s\n", mysql_error(con));
			mysql_close(con);
			return;
  		}
	}
	else
	{
		printf("Connected Database succesfully\n");
		if(mysql_query(con,"CREATE DATABASE IF NOT EXISTS LoRaGatewayDB"))
		{
			printf("Database could not created -> %s\n", mysql_error(con));
			mysql_close(con);
			return;
		}
		if(mysql_query(con,"USE LoRaGatewayDB"))
		{
			printf("Database could not selected -> %s\n", mysql_error(con));
			mysql_close(con);
			return;
		}
		if(mysql_query(con,mysql_query_temp))
		{
			printf("ChannelValues Table could not created -> %s\n", mysql_error(con));
			mysql_close(con);
			return;
		}
	}
}

void 			*ChannelManagerLoadChannelTablesTask(void *arg)
{
	MYSQL *con = mysql_init(NULL);
	if (mysql_real_connect(con, "localhost", "monty", "some_pass", NULL, 0, NULL, 0) == NULL) 
	{
		//fprintf(stderr, "%s\n", mysql_error(con));
		printf("DB Connection Error : %s\n", mysql_error(con));
		MySQL_ErrorCode errcode = (MySQL_ErrorCode)mysql_errno(con);
		mysql_close(con);
		//pthread_exit(1);
		return NULL;
	}
	if(mysql_query(con,"USE LoRaGatewayDB"))
	{
		printf("Database could not selected -> %s\n", mysql_error(con));
		mysql_close(con);
		pthread_exit(4);
	}
	if(mysql_query(con,"SELECT * FROM `DeviceChannelInformation`"))
	{
		printf("Table Read Error : %s\n", mysql_error(con));
		MySQL_ErrorCode errcode = (MySQL_ErrorCode)mysql_errno(con);
		mysql_close(con);
		//pthread_exit(2);	
		return NULL;
	}
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL) 
	{
		printf("Result is NULL \n");
		mysql_close(con);
		//pthread_exit(3);
		return NULL;
	}
	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	if(num_fields == CHANNEL_INFORMATION_TABLE_COLUMN_COUNT)
	{
		while ((row = mysql_fetch_row(result))) 
	  	{
	  		ChannelManager_AddChannel( atoi(row[2]),atoi(row[3]),atoi(row[4]),atoi(row[5]),atoi(row[6])*1000,atoi(row[7]),atoi(row[8]),atoi(row[9]));
	  	}
	}

  	mysql_free_result(result);
  	mysql_close(con);
  	//pthread_exit(0);
  	return NULL;
}


ChannelManagerReturnType ChannelManager_SetChannel_Value( unitNo_t loraIdParam, DeviceChannelTypes channelEnumParam ,void *setData)
{
	ChannelManagerReturnType retValueTemp = CM_CHANNEL_NON_EXIST;
	for ( int k = 0 ; k < CHANNEL_MANAGER_CHANNEL_COUNT ; k++ )
	{
		if ( loraIdParam == channelsInfo[ k ].targetDeviceLoRaID && channelEnumParam == channelsInfo[ k ].channelEnum && CHANNEL_USED == channelsInfo[ k ].state )
		{
			retValueTemp = CM_SUCCESFUL;
		}
	}
	return retValueTemp;
}

ChannelManagerReturnType ChannelManager_GetChannel_Value( unitNo_t loraIdParam, DeviceChannelTypes channelEnumParam ,void *getData)
{
	ChannelManagerReturnType retValueTemp = CM_CHANNEL_NON_EXIST;
	for ( int k = 0 ; k < CHANNEL_MANAGER_CHANNEL_COUNT ; k++ )
	{
		if ( loraIdParam == channelsInfo[ k ].targetDeviceLoRaID && channelEnumParam == channelsInfo[ k ].channelEnum && CHANNEL_USED == channelsInfo[ k ].state )
		{
			retValueTemp = CM_SUCCESFUL;
		}
	}
	return retValueTemp;
}
