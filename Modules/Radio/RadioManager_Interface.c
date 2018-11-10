/*
 * RadioManager_Interface.c
 *
 *  Created on: Apr 15, 2018
 *      Author: Kaan
 */


#include "RadioManager_Interface.h"

#ifndef GATEWAY_CODE

	#include "Channel_Manager.h" // stm devices channel manager
	/* externs */
	extern ChannelHandle_t channels[16];
#endif


#define MAX_DATA_CHANNEL_MESSAGE_CHANNEL_COUNT 40


bool InitializeRadioInterface( void )
{


	return true;
}

bool addNewInterface( 	RadioMessageType messageId, bool haveAnyResponse, RadioMessageType responseMessageId,
						uint16_t responseTimeoutInMillisDefault, uint8_t retryCountDefault, messageCallback successCallback)
{
	return true;
}

bool 	handleReceivedMessage( 	bool areWeReceivedPackageWeAreWaitingParam , bool isItRequestPackageParam, 	RadioMessageType messageIdParam )
{

	bool retValueTemp = false;
	if( areWeReceivedPackageWeAreWaitingParam || isItRequestPackageParam  ) // response , request ayrimi yapilmali burda...
	{
		for( int i = 0 ; i < RADIO_PROTOCOL_MESSAGE_COUNT ; i++ )
		{
			if ( loraMessageArray[i].messageId == messageIdParam )
			{
				if ( loraMessageArray[i].receivedMessageCallback != NULL )
				{
					retValueTemp = true;
					loraMessageArray[i].receivedMessageCallback();
				}
			}
		}
	}

	return retValueTemp;
}

bool 	handleErrorMessage(bool areWeReceivedPackageWeAreWaitingParam,  RadioMessageType messageIdParam )
{
	bool retValueTemp = false;
	if( !areWeReceivedPackageWeAreWaitingParam ) //kaancheck
	{
		for( int i = 0 ; i < RADIO_PROTOCOL_MESSAGE_COUNT ; i++ )
		{
			if ( loraMessageArray[i].messageId == messageIdParam )
			{
				if ( loraMessageArray[i].errorCallback != NULL )
				{
					retValueTemp = true;
					loraMessageArray[i].errorCallback();
				}
			}
		}
	}
	return retValueTemp;
}


bool getResponseMessageIDofThisMessage( RadioMessageType messageIdParam , RadioMessageType *responseIdParam )
{
	bool retValueTemp = false;
	for( int i = 0 ; i < RADIO_PROTOCOL_MESSAGE_COUNT ; i++ )
	{
		if( loraMessageArray[i].messageId == messageIdParam )
		{
			if( loraMessageArray[i].haveAnyResponse)
			{
				retValueTemp = true;
				(*responseIdParam) = loraMessageArray[i].responseMessageId;
			}
		}
	}
	return retValueTemp;
}

bool	checkThatMessageHaveResponse( RadioMessageType messageIdParam )
{
	bool retValueTemp = false;
	for(int i = 0 ; i < RADIO_PROTOCOL_MESSAGE_COUNT ; i++ )
	{
		if(loraMessageArray[i].messageId == messageIdParam )
		{
			retValueTemp = loraMessageArray[i].haveAnyResponse;
		}
	}
	return retValueTemp;
}


bool 	checkThatMessageIsRequest( RadioMessageType messageIdParam )
{
	bool retValueTemp = false;
	for ( int i = 0 ; i < RADIO_PROTOCOL_MESSAGE_COUNT ; i++)
	{
		if( loraMessageArray[i].messageId == messageIdParam )
		{
			retValueTemp = loraMessageArray[i].isRequestMessage;
		}
	}
	return retValueTemp;
}

// HANDLE AND ERROR FUNCTIONS //



void handle_DataChannel_Request( void )
{
	uint8_t messageType = ((loraMessageStruct.data[0] & 0x80)>>7);
	uint8_t channelsCount = ((loraMessageStruct.data[0] & 0x7F));
	if(channelsCount > MAX_DATA_CHANNEL_MESSAGE_CHANNEL_COUNT)
	{
		return;
	}
	#ifndef GATEWAY_CODE // STM CODE

	uint8_t channelIDs[channelsCount];

	//kaanbak: burada channelManager'dan alinacak degerler
	if(messageType == 0) // Read Request
	{
		float 		channelValues[channelsCount];
		uint8_t 	channelStates[channelsCount];
		RetVal_t 	channelValueTemp;
		CM_OPERATION_RESULT channelReadingResult;

		for(int i = 0 ; i < channelsCount ; i++)
		{
			channelIDs[i] = loraMessageStruct.data[1+i];
			//channel reading..
			channelReadingResult = CM_Get_Channel_Value_Raw( channels[channelIDs[i]], &channelValueTemp );
			//data packings
			channelStates[i] = channelReadingResult;
			channelValues[i] = channelValueTemp.f32;
		}
		unitNo_t sourceUnitTemp = getSourceUnitFromGenericRoutingHeader(&(loraMessageStruct.routingHeader));
		Radio_SendDataChannelGetValuesResponse( sourceUnitTemp, channelsCount,channelIDs,channelValues,channelStates, 1000, 2);
	}
	else // Write Request
	{
		for(int i = 0 ; i < channelsCount ; i++)
		{
			//not implemented yet...
		}
	}

	#endif // end of #ifndef GATEWAY_CODE

}
//kaanbak, buraya sourceID'yi eklememiz lazim..

void handle_DataChannel_Response( void )
{
	FILE *rowData;
	char rowString[1024];
	printf("DataChannelResponse Handler\n");
	
	unitNo_t sourceUnitNo = getSourceUnitFromGenericRoutingHeader( &(loraMessageStruct.routingHeader ) );
	uint8_t cmdType = ((loraMessageStruct.data[0] & 0x80) >> 7);
	uint8_t channelCount = (loraMessageStruct.data[0] & 0x7F);
	float channelValues[channelCount];
	uint8_t channelStatus[channelCount];
	uint8_t channelIDs[channelCount];
	int16_t lastRssiValue = Radio_GetLastRssi();
	if(cmdType == 0) printf("DataChannel Read Response , Channel Count :%d, Signal RSSI : %d\n",channelCount, lastRssiValue );
	//else printf("DataChannel Write Response , Channel Count :%d\n",channelCount);
	if ( 0 == cmdType )
	{
		for(int i = 0 ; i < channelCount ; i++)
		{
			uint8_t channelID = loraMessageStruct.data[1+i*6];
			float 	channelValue = 0;
			memcpy(&channelValue,&(loraMessageStruct.data[2+i*6]),sizeof(float));
			channelValues[i] = channelValue;
			channelStatus[i] = loraMessageStruct.data[6+i*6];
			channelIDs[i] = channelID;
		}
		#ifdef GATEWAY_CODE
			ChannelManager_WriteNewValuesToFileSystem( sourceUnitNo , channelCount, channelIDs, channelValues, channelStatus , lastRssiValue );
			//ChannelManager_WriteNewValuesToDatabase( sourceUnitNo , channelCount, channelIDs,channelValues,channelStatus , lastRssiValue );
		#endif
	}
	else if ( 1 == cmdType ) /* Write on channels request */
	{

	}
	
}

void handle_DataChannel_RequestError( void )
{
	//kaantodo
	LoRaMessageQueueElement *queueElement; // son gonderdigimiz paket hakkinda info'lari almak icin...
	queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
	//printf("<handle_DataChannel_RequestError> \n");
	if(queueElement != NULL)
	{
		unitNo_t destinationUnitIdTemp = queueElement->destinationUnit; // bize yanit gondermeyen unitenin id'si...queuemiz'dan cekiyoruz cunku henuz cikarmadik queue dan
		uint8_t cmdType = ((queueElement->messageData[0] & 0x80) >> 7);
		uint8_t channelCount = (queueElement->messageData[0] & 0x7F);

		float channelValues[channelCount];
		uint8_t channelStatus[channelCount];
		uint8_t channelIDs[channelCount];

		for ( int i = 0 ; i < channelCount ; i++ )
		{
			channelValues[i] = -1;
			channelStatus[i] = 9;
			channelIDs[i] = queueElement->messageData[1+i];
		}

		printf("<Error on DataChannel Request : > destinationID : %d \n");
		#ifdef GATEWAY_CODE
			ChannelManager_WriteNewValuesToFileSystem( destinationUnitIdTemp ,channelCount, channelIDs, channelValues, channelStatus , -1 );
			//ChannelManager_WriteNewValuesToDatabase( destinationUnitIdTemp ,channelCount, channelIDs, channelValues, channelStatus , -1);
		#endif
	}
}

void handle_DeviceInformation_Request( void )
{

}

void handle_DeviceInformation_Response( void )
{

}

void handle_DeviceInformation_RequestError( void )
{

}

void handle_OperationalCmd_Request( void )
{

}

void handle_OperationalCmd_Response( void )
{

}

void handle_OperationalCmd_RequestError( void )
{

}


void handle_PingMessage( void )
{
	// get sender
	// add response to queue
	uint8_t messageData = 0;
	RadioRoutingHeaderType headerTypeTemp;
	//debug printf("handle_ping_message\n");
	unitNo_t sourceUnitNo 	= getSourceUnitFromGenericRoutingHeader( &(loraMessageStruct.routingHeader ) );
	headerTypeTemp 		 	= loraMessageStruct.routingHeader.routingHeaderType; /* get routing type */
	if( UNICAST_DIRECT_HEADER == headerTypeTemp || UNICAST_ROUTED_HEADER == headerTypeTemp )
	{
		//debug printf("handle_ping_message unicast header\n");
		addNewMessageToQueue(	sourceUnitNo,
								PONG_MESSAGE,
								UNICAST_SERVICE,
								MESSAGE_PRIORITY_1,
								&messageData,
								0, /* message length */
								0, /* timeout */
	 							0, /* retry count */
								true,
								RADIO_MANAGER_RESPONSE
							);

	}
	else if ( MULTICAST_DIRECT_HEADER == headerTypeTemp )
	{
		// not implemented yet...
	}
	else if ( MULTICAST_ROUTED_HEADER == headerTypeTemp )
	{
		// not implemented yet...
	}
	else if ( BROADCAST_HEADER == headerTypeTemp )
	{
		// not implemented yet...
	}
}

void handle_PongMessage( void )
{

}

void handle_PingMessageError( void )
{

}

void handle_DiscoverRequest( void )
{
//kaantodo: rtc modulunu de ortakladiktan sonra burayi da acabiliriz
#ifndef GATEWAY_CODE
	unitNo_t sourceUnitNoTemp = getSourceUnitFromGenericRoutingHeader( &(loraMessageStruct.routingHeader ) );
	unitNo_t ownUnitNoTemp = getDeviceLoRaID( false , THREAD );

	RadioServiceType serviceTypeTemp = findServiceTypeFromRoutingHeader(loraMessageStruct.mainHeader.routingHeaderType);

	if ( UNICAST_SERVICE == serviceTypeTemp )
	{
		// not implemented yet...
	}
	else if ( MULTICAST_SERVICE == serviceTypeTemp )
	{
		// not implemented yet...
	}
	else if ( BROADCAST_SERVICE == serviceTypeTemp )
	{
		// calculate response time...
		// broadcast mesajlarinin toplam waiting suresi 5000 ms'dir. cevrim icerisindeki maksimum cihaz sayisi 200 ile sinirlandirilmistir. unitelerin unit offseti 5000 / 200 = 25 ms dir.
		//kaanbak : burada queue' doluysa ???

		uint16_t responseTimeTemp = (loraMessageStruct.data[2] << 8 ) | ( loraMessageStruct.data[3] ) ;
		uint32_t unitoffsetWaitTime = ownUnitNoTemp * ( responseTimeTemp / MAX_UNIT_NO_IN_NET );
		uint8_t payloadData[8];
		uint8_t payloadDataLength = 0;
		DateTimeStruct_t deviceDateTimeStr;
		DateTimeStruct_t sourceDateTimeStr;
		RTC_GetTime(&(deviceDateTimeStr.time));
		RTC_GetDate(&(deviceDateTimeStr.date));

		payloadData[ payloadDataLength++ ] = 0xFF; /* 0. byte rezerved*/
		#ifndef GATEWAY_CODE
		payloadData[ payloadDataLength++ ] = 15;  /* DeviceType -> EndPoint */
		#else
		payloadData[ payloadDataLength++ ] = 12;  /* DeviceType -> Gateway */
		#endif
		payloadData[ payloadDataLength++ ] = deviceDateTimeStr.time.hour;
		payloadData[ payloadDataLength++ ] = deviceDateTimeStr.time.minute;
		payloadData[ payloadDataLength++ ] = deviceDateTimeStr.time.second;
		payloadData[ payloadDataLength++ ] = deviceDateTimeStr.date.day;
		payloadData[ payloadDataLength++ ] = deviceDateTimeStr.date.month;
		payloadData[ payloadDataLength++ ] = deviceDateTimeStr.date.year;

		if ( loraMessageStruct.data[0] == 0x01)
		{
			RTC_SetTime(loraMessageStruct.data[4],loraMessageStruct.data[5],loraMessageStruct.data[6]);
			RTC_SetDate(loraMessageStruct.data[7],loraMessageStruct.data[8],loraMessageStruct.data[9]);
		}
		Delay_WithMillis( unitoffsetWaitTime, THREAD );

		addNewMessageToQueue(	sourceUnitNoTemp,
								DISCOVER_NEIGHBOUR_RESPONSE,
								UNICAST_SERVICE,
								MESSAGE_PRIORITY_1,
								payloadData,
								payloadDataLength, /* message length */
								0, /* timeout */
								0, /* retry count */
								true,
								RADIO_MANAGER_RESPONSE
							);
	}
	else
	{
		// error...
	}
	#endif
}

void handle_DiscoverResponse( void )
{

}

void handle_EchoMessage_Request( void )
{
	// get sender
	// add response to queue
	uint8_t messageData = 0;
	RadioRoutingHeaderType headerTypeTemp;
#ifdef GATEWAY_CODE
	printf("handle_echo_message\n");
#endif

	unitNo_t sourceUnitNo 	= getSourceUnitFromGenericRoutingHeader( &(loraMessageStruct.routingHeader ) );
	headerTypeTemp 		 	= loraMessageStruct.routingHeader.routingHeaderType; /* get routing type */
	if( UNICAST_DIRECT_HEADER == headerTypeTemp || UNICAST_ROUTED_HEADER == headerTypeTemp )
	{
#ifdef GATEWAY_CODE
		printf("handle_echo_message_response unicast header -> Destination %d \n", sourceUnitNo );
#endif
		addNewMessageToQueue(	sourceUnitNo,
								ECHO_MESSAGE_RESPONSE,
								UNICAST_SERVICE,
								MESSAGE_PRIORITY_1,
								loraMessageStruct.data,
								loraMessageStruct.dataLength, /* message length */
								0, /* timeout */
								0, /* retry count */
								true,
								RADIO_MANAGER_RESPONSE
							);

	}
	else if ( MULTICAST_DIRECT_HEADER == headerTypeTemp )
	{
		// not implemented yet...
	}
	else if ( MULTICAST_ROUTED_HEADER == headerTypeTemp )
	{
		// not implemented yet...
	}
	else if ( BROADCAST_HEADER == headerTypeTemp )
	{
		// not implemented yet...
	}
}
void handle_EchoMessage_Response( void )
{

}
void handle_EchoMessage_RequestError( void )
{

}

void handle_Uncategorized_Unr_Msg( void )
{

}

void handle_Uncategorized_Rel_Msg_Request(void)
{

}
void handle_Uncategorized_Rel_Msg_Response(void)
{

}

void handle_Uncategorized_Rel_Msg_RequestError(void)
{

}

void handle_FileOperations_Request( void )
{
#ifndef GATEWAY_CODE
	Radio_FileOperation_Cmds fileOperationsMsgID = (Radio_FileOperation_Cmds)loraMessageStruct.data[0];
	unitNo_t sourceUnitNoTemp 	= getSourceUnitFromGenericRoutingHeader( &(loraMessageStruct.routingHeader ) );

	loraMessageStruct.data[1] =  (uint8_t)( PackerChunk_s.bytesWritten >> 8 );
	loraMessageStruct.data[2] =  (uint8_t)( PackerChunk_s.bytesWritten & 0xFF );

	memcpy( loraMessageStruct.data + 3, PackerChunk_s.writeDataBuffer, 200 );

	loraMessageStruct.dataLength = 204;
	switch( fileOperationsMsgID )
	{
		case GET_LAST_WRITTEN_ROW_CMD:


			break;
		case GET_LAST_READ_ROW_CMD:
			break;
		default:
			break;
	}
	addNewMessageToQueue(	sourceUnitNoTemp,
							FILE_OPERATIONS_RESPONSE,
							UNICAST_SERVICE,
							MESSAGE_PRIORITY_1,
							loraMessageStruct.data,
							loraMessageStruct.dataLength, /* message length */
							0, /* timeout */
							0, /* retry count */
							true,
							RADIO_MANAGER_RESPONSE
						);

#endif
}

void handle_FileOperations_Response( void )
{

}

void handle_FileOperations_RequestError( void )
{

}
