// author : mkerkoc
// date : 	08/02/18

#ifndef RADIO_QUEUE_MANAGER_H
#define RADIO_QUEUE_MANAGER_H


/* Includes */
#include "stdint.h"
#include "RadioManager.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"


/* Defines */
#ifndef GATEWAY_CODE
#define LORA_MESSAGE_QUEUE_SIZE 	32
#else 
#define LORA_MESSAGE_QUEUE_SIZE 	128
#endif


#define LORA_MESSAGE_DATA_SIZE 		240


/* Enums */
typedef enum RadioMessageState
{
	MESSAGE_EMPTY,
	MESSAGE_WAITING_FOR_SEND,
	MESSAGE_TRANSMITTING,
	MESSAGE_TRANSMITTED,
	MESSAGE_WAITING_ACK,
	MESSAGE_ACK_RECEIVED
}RadioMessageState;

typedef enum RadioMessagePriority
{
	MESSAGE_PRIORITY_0,
	MESSAGE_PRIORITY_1,
	MESSAGE_PRIORITY_2,
	MESSAGE_PRIORITY_3,
	MESSAGE_PRIORITY_4
}RadioMessagePriority;

typedef enum MessageQueueStepReason
{
	FIRST_MESSAGE_ADDED, /* queue'ya ilk mesaj eklendiginde cagirilir..*/
	TX_DONE, /* yanit bekledigimiz bir mesaj gonderildikten sonra cagirilir..*/
	RX_DONE,
	RESPONSE_RECEIVED, /* bekledigimiz yanit gelince cagirilir..*/
	RESPONSE_TIMEOUT, /* bekledigimiz yanit timeout suresi icerisinde gelmeyince cagirilir*/
	RESPONSE_FAILED, /* bekledigimiz yanit tum denemelere ragmen gelmeyince cagirilir*/
	TX_TIMEOUT,

	SEND_FAILED
}MessageQueueStepReason;

/* Structs */
typedef struct LoRaMessageQueueElement
{
	LoRaMessageMainHeader 		mainHeader;
	uint8_t						destinationCount;
	unitNo_t					destinationUnit;
	RadioMessageState 			state;
	RadioMessagePriority 		priority;
	bool						haveAnyResponse;
	uint8_t						messageData[LORA_MESSAGE_DATA_SIZE];
	uint8_t						messageLength;
	RadioServiceType			serviceType;
	uint16_t 					timeout;
	uint8_t						retryCount;
	uint8_t						remainingRetryCount;
	uint16_t					messageID;
	time_t						timeStruct;
	RadioMessageSourceType  	sourceType;
	#ifdef GATEWAY_CODE
	struct mg_connection 		*nc;
	#endif
}LoRaMessageQueueElement;

/* Globals */
int 						loraMessageQueueHeadIndex;
int							loraMessageQueueLastIndex;
uint8_t 					loraMessageCountInQueue;
LoRaMessageQueueElement 	loraMessageQueue[LORA_MESSAGE_QUEUE_SIZE];


/* Function Prototypes */
bool						Initialize_LoRaMessageQueue(void);
bool						addNewMessageToQueue( 	unitNo_t destination, 
													RadioMessageType messageType, 
													RadioServiceType serviceType, 
													RadioMessagePriority priority,
													uint8_t *data,
													uint8_t length,
													uint16_t timeout,
													uint8_t retryCount,
													bool allowOverwrite, 
													RadioMessageSourceType  
													sourceTypeParam
												);
bool						clearAllMessageQueue(void);
bool						clearMessageFromQueueByMessageID(uint16_t messageID);
bool						clearMessageFromQueueByIndex(uint8_t index);
bool						clearLastMessageOnQueue(void);
bool 						removeHeadElementInQueue(void);
bool						stepMessageQueue(MessageQueueStepReason reason);
LoRaMessageQueueElement* 	getMessageQueueElementByMessageID(uint16_t messageID);
LoRaMessageQueueElement* 	getMessageQueueElementByIndex(uint8_t index);

#endif