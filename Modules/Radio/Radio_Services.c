/*
 * Radio_Services.c
 *
 *  Created on: Feb 13, 2018
 *      Author: Kaan
 */


#include "Radio_Services.h"

void Radio_SendDataChannelSetValuesRequest(	unitNo_t deviceID,
											uint8_t channelCount, 
											uint8_t *channelIDs,
											float *channelValues,
											uint16_t messageTimeout,
											uint8_t messageRetry
											)
{

	if(channelCount > MAX_DATA_CHANNEL_MESSAGE_CHANNEL_COUNT)
	{
		return;
	}
	uint8_t msgDataTemp[1 + channelCount+ channelCount*sizeof(float)];
	msgDataTemp[0] = 0x00;
	msgDataTemp[0] = msgDataTemp[0] | channelCount;
	msgDataTemp[0] = msgDataTemp[0] | 0x80; // set MSB bit to High -> Write
	for(int i = 0 ; i < channelCount ; i++)
	{
		//pack datas
		msgDataTemp[1+i*5] = channelIDs[i];
		memcpy(&msgDataTemp[2+i*5],&channelValues[i],sizeof(float));
	}
	addNewMessageToQueue(	deviceID,
							DATA_CHANNEL_REQUEST,
							UNICAST_SERVICE,
							MESSAGE_PRIORITY_1,
							msgDataTemp,
							(1 + channelCount+ channelCount*sizeof(float)),
							messageTimeout,
							messageRetry,
							true,
							CHANNEL_MANAGER);
}

void Radio_SendDataChannelGetValuesRequest( unitNo_t deviceID, 
											uint8_t channelCount,
											uint8_t *channelIDs,
											uint16_t messageTimeout,
											uint8_t messageRetry
											)
{
	if(channelCount > MAX_DATA_CHANNEL_MESSAGE_CHANNEL_COUNT)
	{
		return;
	}
	uint8_t msgDataTemp[1+ channelCount];
	msgDataTemp[0] = 0;
	msgDataTemp[0] = msgDataTemp[0] + channelCount;
	msgDataTemp[0] = msgDataTemp[0] & 0x7F; // set MSB bit to Low  -> Read
	for(int i = 0 ; i < channelCount ; i++)
	{
		msgDataTemp[1+i] = channelIDs[i];
	}
	addNewMessageToQueue(	deviceID, DATA_CHANNEL_REQUEST,UNICAST_SERVICE,MESSAGE_PRIORITY_1,
							msgDataTemp,(1 + channelCount),messageTimeout,messageRetry,true,CHANNEL_MANAGER);
}

void Radio_SendDataChannelSetValuesResponse(unitNo_t deviceID,uint8_t channelCount,uint8_t *channelDs,uint8_t *results,uint16_t messageTimeout,uint8_t messageRetry)
{
	if(channelCount > MAX_DATA_CHANNEL_MESSAGE_CHANNEL_COUNT)
	{
		return;
	}
	uint8_t msgDataTemp[channelCount*sizeof(float)+1];
	msgDataTemp[0] = msgDataTemp[0] | channelCount;
	msgDataTemp[0] = msgDataTemp[0] | 0x80; // set MSB bit to High -> Write
	for(int i = 0 ; i < channelCount ; i++)
	{
		// pack data

		msgDataTemp[1+ i * 2] = channelDs[i];
		msgDataTemp[2 +i * 2] = results[i];
	}


}
void Radio_SendDataChannelGetValuesResponse(	unitNo_t 	deviceID,
												uint8_t 	channelCount,
												uint8_t 	*channelIDs,
												float 		*channelValues,
												uint8_t 	*channelStates,
												uint16_t 	messageTimeout,
												uint8_t 	messageRetry
											)
{

	if(channelCount > MAX_DATA_CHANNEL_MESSAGE_CHANNEL_COUNT)
	{
		return;
	}

	uint8_t msgDataTemp[ channelCount * ( sizeof(float) + 2) + 1 ];

	memset(msgDataTemp,0,sizeof(msgDataTemp));

	msgDataTemp[0] = 0x00;
	msgDataTemp[0] = msgDataTemp[0] | channelCount;
	msgDataTemp[0] = msgDataTemp[0] & 0x7F; // set MSB bit to Low  -> Read

	for(int i = 0 ; i < channelCount  ; i++)
	{
		msgDataTemp[1 + i * 6] = channelIDs[i]; 								// channel ID
		memcpy( &msgDataTemp[2 + i * 6], &(channelValues[i]), sizeof(float) ); // channel Data
		msgDataTemp[3 + i * 6 ] = channelStates[i];								// channel State
	}

	addNewMessageToQueue(	deviceID,
							DATA_CHANNEL_RESPONSE,
							UNICAST_SERVICE,
							MESSAGE_PRIORITY_1,
							msgDataTemp,
							sizeof(msgDataTemp),
							messageTimeout,
							messageRetry,
							true,
							CHANNEL_MANAGER);
}
