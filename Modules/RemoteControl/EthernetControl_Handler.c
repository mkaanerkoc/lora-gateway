#include "EthernetControl_Handler.h"
#include "DeviceManager.h"


void handleDeviceUniqueID_Set_ETHERNET( uint8_t *msgData,uint16_t msgLength,struct mg_connection *nc, Context_Enum context )
{
	uint16_t uniqueIDtoSet;
	uint8_t ackResponse[2] = {1,INCOMING_CMD_DEVICE_UNIQUE_ID_SET_REQUEST}; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	uniqueIDtoSet = (msgData[0] <<8 ) | ( msgData[1] );

	setDeviceUniqueID( uniqueIDtoSet, context);

	EthernetControl_SendCommand( OUTGOING_CMD_ACK,ackResponse,2, nc);
}
void handleDeviceUniqueID_Get_ETHERNET(struct mg_connection *nc,Context_Enum context)
{
	uint16_t 			uniqueIDtoSend;
	uint16_t 			messageLength = 2;
	uint8_t	 			messageData[2];
	uint16_t 			messageCmd = OUTGOING_CMD_DEVICE_UNIQUE_ID_RESPONSE;
	uniqueIDtoSend = getDeviceUniqueID( false, context);  
	messageData[0] = (uint8_t)(uniqueIDtoSend>>8);
	messageData[1] = (uint8_t)(uniqueIDtoSend&0xff);

	EthernetControl_SendCommand(messageCmd,messageData,messageLength, nc);
}

void handleDeviceLoRaID_Set_ETHERNET(uint8_t *msgData, uint16_t msgLength, struct mg_connection *nc, Context_Enum context)
{
	unitNo_t loraIDtoSet;
	uint8_t ackResponse[2] = {1, INCOMING_CMD_DEVICE_LORA_ID_SET_REQUEST }; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	loraIDtoSet = msgData[0];
	if ( loraIDtoSet > MAX_UNIT_NO_IN_NET ) // ERROR
	{
		ackResponse[0] = 0; // ERROR BYTE
		EthernetControl_SendCommand( OUTGOING_CMD_ACK,ackResponse,2, nc);
		return;
	}
	else
	{
		setDeviceLoRaID(loraIDtoSet,context);
		EthernetControl_SendCommand( OUTGOING_CMD_ACK,ackResponse,2, nc);
		return;
	}
	
}
void handleDeviceLoRaID_Get_ETHERNET(struct mg_connection *nc,Context_Enum context)
{
	unitNo_t 			loraIDtoSend;
	uint16_t 			messageLength = 1;
	uint8_t	 			messageData;
	uint16_t 			messageCmd = OUTGOING_CMD_DEVICE_LORA_ID_RESPONSE;
	loraIDtoSend 		= getDeviceLoRaID(false, context);
	messageData			= loraIDtoSend;

	EthernetControl_SendCommand(messageCmd,&messageData,messageLength,nc);
}


void handleDeviceLoRaGatewayID_Set_ETHERNET(uint8_t *msgData, uint16_t msgLength ,struct mg_connection *nc, Context_Enum context)
{
	unitNo_t loraGatewayIDtoSet;
	uint8_t ackResponse[2] = {1,INCOMING_CMD_DEVICE_LORA_GATEWAY_ID_SET_REQUEST}; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	loraGatewayIDtoSet = (msgData[0]);
	if ( loraGatewayIDtoSet > MAX_UNIT_NO_IN_NET )
	{
		ackResponse[0] = 0; // ERROR BYTE
		EthernetControl_SendCommand( OUTGOING_CMD_ACK,ackResponse,2, nc);
		return;
	}
	else
	{
		setDeviceLoRaGatewayID(loraGatewayIDtoSet,context);
		EthernetControl_SendCommand( OUTGOING_CMD_ACK,ackResponse,2, nc);
		return;
	}
	
}

void handleDeviceLoRaGatewayID_Get_ETHERNET(struct mg_connection *nc, Context_Enum context)
{
	unitNo_t 			loraGatewayIDtoSend;
	uint16_t 			messageLength = 1;
	uint8_t	 			messageData;
	uint16_t 			messageCmd = OUTGOING_CMD_DEVICE_LORA_GATEWAY_ID_RESPONSE;
	loraGatewayIDtoSend = getDeviceLoRaGatewayID(false,context);
	messageData = (unitNo_t)(loraGatewayIDtoSend );
	EthernetControl_SendCommand( messageCmd, &messageData, messageLength, nc );
}


void handleLoRaConfiguration_Set_ETHERNET(uint8_t *msgData,uint16_t msgLength,struct mg_connection *nc,Context_Enum context)
{
	uint8_t loraCodingRate,loraSpreadingFactor,loraBandwidth;
	uint8_t ackResponse[2] = {1,INCOMING_CMD_LORA_CODING_RATE_SET_REQUEST}; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	loraCodingRate = msgData[0];
	loraSpreadingFactor = msgData[1];
	loraBandwidth = msgData[2];
	SX1272_SetCodingRate((RadioCR_Enum)loraCodingRate,context);
	SX1272_SetSpreadingFactor((RadioSF_Enum)loraSpreadingFactor,context);
	SX1272_SetBandwidth((RadioBW_Enum)loraBandwidth,context);
	EthernetControl_SendCommand(OUTGOING_CMD_ACK,ackResponse,2,nc);
}
void handleLoRaConfiguration_Get_ETHERNET(struct mg_connection *nc, Context_Enum context)
{
	uint16_t 			messageLength = 3;
	uint8_t	 			messageData[3];
	uint16_t 			messageCmd = OUTGOING_CMD_LORA_CONFIG_RESPONSE;
	messageData[0] = (uint8_t)SX1272_GetCodingRate(context);
	messageData[1] = (uint8_t)SX1272_GetSpreadingFactor(context);
	messageData[2] = (uint8_t)SX1272_GetBandwidth(context);
	EthernetControl_SendCommand(messageCmd,messageData,messageLength ,nc);
}

void handleLoRaEchoMonitor_Set_ETHERNET(uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum context)
{
	uint8_t ackResponse[2] = {1,INCOMING_CMD_LORA_ECHO_MONITOR_SET_REQUEST}; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	Radio_SetEchoSettings(msgData[0],(bool) msgData[1]);
	EthernetControl_SendCommand(OUTGOING_CMD_ACK,ackResponse,2,nc);
}
void handleLoRaEchoMonitor_Get_ETHERNET(uint8_t *msgData,uint16_t msgLength,struct mg_connection *nc,Context_Enum context)
{
	uint16_t 			messageLength = 2;
	uint8_t	 			messageData[2];
	uint16_t 			messageCmd = OUTGOING_CMD_LORA_ECHO_MONITOR_REPONSE;
	messageData[0] = msgData[0];
	messageData[1] = Radio_GetEchoSettings(messageData[0]);
	EthernetControl_SendCommand(messageCmd,messageData,messageLength ,nc);
}


void handleLoRaSendPackage_ETHERNET(uint8_t *msgData, uint16_t msgLength, struct mg_connection *nc,Context_Enum context)
{
	uint8_t ackResponse[2] 	= {1,INCOMING_CMD_LORA_SEND_PACKAGE}; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	uint16_t destinationID 	= (msgData[0]);
	uint8_t messageType  	= msgData[1];
	uint8_t	servisType 		= msgData[2];
	uint16_t timeoutTime 	= (msgData[3]<<8)|(msgData[4]);
	uint8_t retryCount 		= msgData[5];
	uint8_t length 			= msgData[6];
	uint8_t data[length];
	memcpy(data, msgData+7, length);
	//printf("handleLoRaSendPackage_ETHERNET -> destinationID : %d \n",destinationID);
	//Radio_Send_Package(destinationID, messageType, ackType ,servisType, data, length, timeoutTime, retryCount);
	addNewMessageToQueue(	
							destinationID,	
							messageType, 
							servisType,
							MESSAGE_PRIORITY_0,
							data,
							length,
							timeoutTime,
							retryCount,
							false,
							FROM_ETHERNET
						);
	EthernetControl_SendCommand(OUTGOING_CMD_ACK,ackResponse, 2, nc);
}


void handleLoRa_StaticRoutingTable_Set_ETHERNET( uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam )
{
	uint8_t _sRoutingTable[sizeof(sRoutingTableElement)*MAX_STATIC_ROUTING_TABLE_SIZE];
	uint8_t ackResponse[2] = {1,INCOMING_CMD_LORA_STATIC_ROUTING_TABLE_SET_REQUEST}; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	memcpy(_sRoutingTable, msgData ,sizeof(sRoutingTableElement)*MAX_STATIC_ROUTING_TABLE_SIZE);
	EEPROM_WriteBulk(EEPROM_1,E2PROM_LORA_ROUTING_MAP_INDEX,_sRoutingTable,sizeof(sRoutingTableElement)*MAX_STATIC_ROUTING_TABLE_SIZE,contextParam);// kaanbak fonksiyon check'ini return ile yap.. ona gore ackResponse'u duzenle...
	EthernetControl_SendCommand( OUTGOING_CMD_ACK, ackResponse,2, nc );
}

void handleLoRa_StaticRoutingTable_Get_ETHERNET( struct mg_connection *nc, Context_Enum contextParam )
{

	uint8_t	 			messageData[sizeof(sRoutingTableElement)*MAX_STATIC_ROUTING_TABLE_SIZE];
	uint16_t 			messageCmd = OUTGOING_CMD_LORA_STATIC_ROUNTING_TABLE_RESPONSE;
	uint16_t 			messageLength = E2PROM_LORA_ROUTING_MAP_LENGTH;
	EEPROM_ReadBulk(EEPROM_1,E2PROM_LORA_ROUTING_MAP_INDEX,messageData,messageLength,contextParam);
	EthernetControl_SendCommand(messageCmd,messageData,messageLength ,nc);
}


void handle_DataChannels_All_Set_ETHERNET( uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam )
{
	//uint8_t _sDataChannels[sizeof( ChannelInfoStr )* CHANNEL_MANAGER_CHANNEL_COUNT];
	uint8_t ackResponse[2] = {1, INCOMING_CMD_DATA_CHANNELS_ALL_SET_REQUEST}; // hangi komuta ACK yada NACK verdigimizi belirtiyoruz,,,
	
	uint8_t activeChannelCount = msgData[0];
	printf("<DataChannels> SET -> ActiveChannelCount : %d\n", activeChannelCount);
	for ( int i = 0 ; activeChannelCount ; i++)
	{
		if ( i < activeChannelCount )
		{
			// used channel
			unitNo_t deviceLoraId = msgData[1 + 11 * i ];
			DeviceChannelTypes channelEnum = ( DeviceChannelTypes )msgData[2 + 11 * i];
			ChannelType channelType = ( ChannelType )msgData[3 + 11 * i];
			ChannelDataType channelDataType = (ChannelDataType)msgData[4 + 11 * i];
			uint16_t readPeriod = (uint16_t)  ( msgData[5 + 11 * i] << 8 | msgData[6 + 11 * i] );
			uint16_t readTimeOut = (uint16_t)  ( msgData[7 + 11 * i] << 8 | msgData[8 + 11 * i] );
			uint8_t  retryCount = msgData[9 + 11 * i];
			uint16_t modbusRegister = (uint16_t)  ( msgData[10 + 11 * i] << 8 | msgData[11 + 11 * i] );
			printf ( "<DataChannels > LoRaID :%d, ChannelEnum : %d , ChannelType : %d, ChannelDataType : %d, ReadPeriod :%d, ReadTimeOut :%d , RetryCount :%d, ModbusRegister :%d\n",
					deviceLoraId, channelEnum ,channelType, channelDataType, readPeriod, readTimeOut, retryCount, modbusRegister );

		}
		else
		{
			// not used channel
		}
	}
	return;
}

void handle_DataChannels_All_Get_ETHERNET( struct mg_connection *nc, Context_Enum contextParam )
{
}

void handle_DataChannels_Set_ETHERNET( uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam )
{

}


void handle_DataChannels_Get_ETHERNET( struct mg_connection *nc, Context_Enum contextParam )
{

}