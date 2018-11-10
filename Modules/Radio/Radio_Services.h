/*
 * Radio_Services.h
 *
 *  Created on: Feb 13, 2018
 *      Author: Kaan
 */

#ifndef RADIO_RADIO_SERVICES_H_
#define RADIO_RADIO_SERVICES_H_

/* Includes */
#include "Radio_QueueManager.h"

/* Defines */
#define MAX_DATA_CHANNEL_MESSAGE_CHANNEL_COUNT 25

/* Message Types */
void Radio_SendDataChannelSetValuesRequest(unitNo_t deviceID,uint8_t channelCount, uint8_t *channelIDs,float *channelValues,  uint16_t messageTimeout,uint8_t messageRetry);
void Radio_SendDataChannelGetValuesRequest(unitNo_t deviceID,uint8_t channelCount,uint8_t *channelIDs,uint16_t messageTimeout,uint8_t messageRetry);

void Radio_SendDataChannelSetValuesResponse(unitNo_t deviceID,uint8_t channelCount,uint8_t *channelDs,uint8_t *results,uint16_t messageTimeout,uint8_t messageRetry);
void Radio_SendDataChannelGetValuesResponse(unitNo_t deviceID,uint8_t channelCount,uint8_t *channelIDs,float *channelValues, uint8_t *channelStates, uint16_t messageTimeout,uint8_t messageRetry);




#endif /* RADIO_RADIO_SERVICES_H_ */
