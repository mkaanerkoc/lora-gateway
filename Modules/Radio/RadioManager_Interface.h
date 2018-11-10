/*
 * RadioManager_Interface.h
 *
 *  Created on: Jan 27, 2018
 *      Author: Kaan
 */

#ifndef RADIO_RADIOMANAGER_INTERFACE_H_
#define RADIO_RADIOMANAGER_INTERFACE_H_

#include "stdbool.h"
#include "RadioManager_TypesConstants.h"
#include "Radio_Services.h"
#include "RadioManager.h"

#define RADIO_PROTOCOL_MESSAGE_COUNT 15


typedef void (*messageCallback)(void);


typedef struct RadioMessageConfiguration_s
{
	RadioMessageType 	messageId;
	bool 				haveAnyResponse;
	bool				isRequestMessage;
	RadioMessageType 	responseMessageId;
	uint16_t			responseTimeoutInMillisDefault;
	uint8_t 			retryCountDefault;
	void 				(*receivedMessageCallback)(void);
	void				(*errorCallback)(void);
}RadioMessageConfiguration_s;


//kaanbak:
void handle_DataChannel_Request ( void );
void handle_DataChannel_Response( void );
void handle_DataChannel_RequestError( void ); /* request'e yanit gelmediginde cagirilacak fonksiyon */

void handle_DeviceInformation_Request(void);
void handle_DeviceInformation_Response(void);
void handle_DeviceInformation_RequestError(void);

void handle_OperationalCmd_Request(void);
void handle_OperationalCmd_Response(void);
void handle_OperationalCmd_RequestError(void);

void handle_PingMessage( void );
void handle_PongMessage( void );
void handle_PingMessageError( void ); /* ping mesajina yanit gelmediginde cagirilacak fonksiyon */

void handle_DiscoverRequest( void );
void handle_DiscoverResponse( void );

void handle_EchoMessage_Request( void );
void handle_EchoMessage_Response( void );
void handle_EchoMessage_RequestError( void ); /* echo mesajina karsidan yanit gelmediginde cagirilacak fonksiyon */

void handle_Uncategorized_Unr_Msg( void );

void handle_Uncategorized_Rel_Msg_Request(void);
void handle_Uncategorized_Rel_Msg_Response(void);
void handle_Uncategorized_Rel_Msg_RequestError(void);

void handle_FileOperations_Request( void );
void handle_FileOperations_Response( void );
void handle_FileOperations_RequestError( void );



static RadioMessageConfiguration_s  loraMessageArray[ RADIO_PROTOCOL_MESSAGE_COUNT ] = {

		{ DATA_CHANNEL_REQUEST			, true , true, DATA_CHANNEL_RESPONSE			,	3000,	5 , &handle_DataChannel_Request , &handle_DataChannel_RequestError },
		{ DATA_CHANNEL_RESPONSE			, false, false, 0								, 	0	,	0 , &handle_DataChannel_Response, NULL },
	
		{ DEVICE_INFORMATION_REQUEST 	, true , true, DEVICE_INFORMATION_RESPONSE		,  	3000,	5 , &handle_DeviceInformation_Request , &handle_DeviceInformation_RequestError},
		{ DEVICE_INFORMATION_RESPONSE	, false, false, 0								,  	0	,	0 , &handle_DeviceInformation_Response , NULL },
	
		{ OPERATIONAL_REQUEST			, true , true, OPERATIONAL_RESPONSE				, 	3000,	5 , &handle_OperationalCmd_Request , &handle_OperationalCmd_RequestError },
		{ OPERATIONAL_RESPONSE			, false, false,0								,	0	, 	0 , &handle_OperationalCmd_Response, NULL },
	
		{ PING_MESSAGE					, true, true,PONG_MESSAGE						,	3000, 	5 , &handle_PingMessage , &handle_PingMessageError },
		{ PONG_MESSAGE					, false,false, 0								,	0	, 	0 , &handle_PongMessage , NULL},
	
		{ DISCOVER_NEIGHBOUR_REQUEST	, true, true, DISCOVER_NEIGHBOUR_RESPONSE		,	5000, 	0 , &handle_DiscoverRequest , NULL },
		{ DISCOVER_NEIGHBOUR_RESPONSE	, false, false,0								,	0	, 	0 , &handle_DiscoverResponse, NULL },
	
	
		{ ECHO_MESSAGE_REQUEST			, true, true, ECHO_MESSAGE_RESPONSE				,	3000, 	5 , &handle_EchoMessage_Request , &handle_EchoMessage_RequestError },
		{ ECHO_MESSAGE_RESPONSE			, false, false, 0								,	0	, 	0 , &handle_EchoMessage_Response, NULL},

		{ UNCATEGORIZED_UNREL_MSG_REQUEST, false,true, 0								,	0	, 	0 ,	&handle_Uncategorized_Unr_Msg , NULL },

		{ UNCATEGORIZED_REL_MSG_REQUEST	, true, true, UNCATEGORIZED_REL_MSG_RESPONSE	,	3000, 	5 , &handle_EchoMessage_Request , &handle_EchoMessage_RequestError },
		{ UNCATEGORIZED_REL_MSG_RESPONSE, false,false, 0								,	0	, 	0 , &handle_EchoMessage_Response, NULL},

		{ FILE_OPERATIONS_REQUEST		, true, true, FILE_OPERATIONS_RESPONSE			, 3000  ,   5 , &handle_FileOperations_Request, &handle_FileOperations_RequestError },
		{ FILE_OPERATIONS_RESPONSE		, false, false , 0								, 0		,	0 , &handle_FileOperations_Response, NULL }
};



/* public functions */
bool	InitializeRadioInterface( void );
bool 	addNewInterface( 	RadioMessageType messageId, bool haveAnyResponse, RadioMessageType responseMessageId,
							uint16_t responseTimeoutInMillisDefault, uint8_t retryCountDefault, messageCallback successCallback);

bool 	handleReceivedMessage( 	bool areWeReceivedPackageWeAreWaitingParam , bool isItRequestPackageParam, 	RadioMessageType messageIdParam );
bool 	handleErrorMessage(		bool areWeReceivedPackageWeAreWaitingParam,  	RadioMessageType messageIdParam );

bool	checkThatMessageHaveResponse( RadioMessageType messageIdParam );
bool 	checkThatMessageIsRequest( RadioMessageType messageIdParam );
bool 	getResponseMessageIDofThisMessage( RadioMessageType messageIdParam , RadioMessageType *responseIdParam );


#endif /* RADIO_RADIOMANAGER_INTERFACE_H_ */
