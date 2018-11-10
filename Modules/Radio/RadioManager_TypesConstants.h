/*
 * RadioManager_TypesConstants.h
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#ifndef RADIO_RADIOMANAGER_TYPESCONSTANTS_H_
#define RADIO_RADIOMANAGER_TYPESCONSTANTS_H_


#include "stdint.h"
#include "Common_TypeDefs.h"


/*Defines */
#define RH_RF92_FIFO_SIZE 255
#define INVALID_DEVICE_NO	 254

#define MAX_UNIT_NO_IN_NET 200

// This is the maximum number of bytes that can be carried by the LORA.
// We use some for headers, keeping fewer for RadioHead messages
#define RH_RF92_MAX_PAYLOAD_LEN RH_RF92_FIFO_SIZE

// The length of the headers we add.
// The headers are inside the LORA's payload
#define RH_RF92_HEADER_LEN 4 // kaanbak burasi degisecek

#define MINIMUM_LORA_PACKAGE_SIZE 3 // kaanbak-> bu degeri gotumden salladim ilerleyen zamanlarda guncellenecek...
/*LoRa Unicat Message Header struct = [ 1 byte ServiceType + 9 byte UnicastHeader  + 8 byte messageHeader ] */
/*LoRa Multicast Message Header struct = [ 1 byte Service Type ... + x byte Multicast Header , + 8 byte messageHeader ] ... kaanbak !! */

#ifndef RH_RF92_MAX_MESSAGE_LEN
	#define RH_RF92_MAX_MESSAGE_LEN (RH_RF92_MAX_PAYLOAD_LEN - RH_RF92_HEADER_LEN)
#endif




/*Enums*/
typedef enum
{
	RHModeInitialising = 0, ///< Transport is initialising. Initial default value until init() is called..
	RHModeSleep,            ///< Transport hardware is in low power sleep mode (if supported)
	RHModeIdle,             ///< Transport is idle.
	RHModeTx,               ///< Transport is in the process of transmitting a message.
	RHModeRx,               ///< Transport is in the process of receiving a message.
	RHModeCad               ///< Transport is in the process of detecting channel activity (if supported)
} RHMode;

/*BandWidth */
typedef enum
{
	BW_125Khz = 0x00,
	BW_250Khz = 0x01,
	BW_500Khz = 0x02
}RadioBW_Enum;

/*Coding Rate */
typedef enum
{
	CR_4_5 = 0x01,
	CR_4_6 = 0x02,
	CR_4_7 = 0x03,
	CR_4_8 = 0x04
}RadioCR_Enum;

/*Spreading Factor*/
typedef enum
{
	SF_64C_Symbol 		= 0x06,
	SF_128C_Symbol 	= 0x07,
	SF_256C_Symbol 	= 0x08,
	SF_512C_Symbol 	= 0x09,
	SF_1024C_Symbol	= 0x0A,
	SF_2048C_Symbol 	= 0x0B,
	SF_4096C_Symbol 	= 0x0C
}RadioSF_Enum;


typedef enum RadioMessageType // 5 bit
{
	//Requests
	DATA_CHANNEL_REQUEST 			=0,
	DEVICE_INFORMATION_REQUEST		=1,
	OPERATIONAL_REQUEST				=2,
	PING_MESSAGE					=3, // request yada response bilgisi yok mesajla birlikte islenecek
	//Responses
	DATA_CHANNEL_RESPONSE			=4,
	DEVICE_INFORMATION_RESPONSE		=5,
	OPERATIONAL_RESPONSE			=6,
	PONG_MESSAGE					=7,

	DISCOVER_NEIGHBOUR_REQUEST		= 8,
	DISCOVER_NEIGHBOUR_RESPONSE		= 9,

	UNCATEGORIZED_UNREL_MSG_REQUEST = 10,

	UNCATEGORIZED_REL_MSG_REQUEST  = 11,
	UNCATEGORIZED_REL_MSG_RESPONSE = 12,

	ECHO_MESSAGE_REQUEST = 13,
	ECHO_MESSAGE_RESPONSE = 14,

	FILE_OPERATIONS_REQUEST = 15,
	FILE_OPERATIONS_RESPONSE = 16,

	RADIO_MESSAGE_TYPE_COUNT = 17
}RadioMessageType;


typedef enum RadioServiceType
{
	UNICAST_SERVICE 			= 0x00,
	MULTICAST_SERVICE 			= 0x01,
	BROADCAST_SERVICE 			= 0x02,
	RADIO_SERVICE_TYPE_COUNT 	= 0x03
}RadioServiceType;

typedef enum RadioRoutingHeaderType
{
	UNICAST_DIRECT_HEADER		= 0x00,
	UNICAST_ROUTED_HEADER		= 0x01,
	MULTICAST_DIRECT_HEADER		= 0x02,
	MULTICAST_ROUTED_HEADER		= 0x03,
	BROADCAST_HEADER			= 0x04,
	ROUTING_HEADER_TYPES_COUNT	= 0x05
}RadioRoutingHeaderType;

typedef enum RadioMessageSourceType
{
	FROM_BLUETOOTH	 	= 0x00,
	FROM_SERIAL			= 0x01,
	FROM_ETHERNET 		= 0x02,
	CHANNEL_MANAGER		= 0x03,
	RADIO_MANAGER_RESPONSE = 0x04,
	ROUTING_OPERATION  = 0x05
}RadioMessageSourceType;


typedef enum RadioRoutingState
{
	NOT_SET_ROUTING_INFO,
	ROUTER_OF_PACKAGE,
	DESTINATION_OF_THIS_PACKAGE,
	WRONG_ROUTED_MESSAGE,
	INDEPENDENT_WITH_PACKAGE
}RadioRoutingState;

typedef struct UnicastHeader
{
	unitNo_t sourceUnit;
	unitNo_t fromUnit;
	unitNo_t destinationUnit;
	unitNo_t toUnit;
	uint8_t hopIndex;
}UnicastHeader;


typedef struct LoRaMessageMainHeader
{
	RadioMessageType 			messageType;
	RadioRoutingHeaderType 		routingHeaderType;
}LoRaMessageMainHeader;


/* Routing Headers */

typedef struct UnicastDirectMsgRoutingHeader_s
{
	unitNo_t 	sourceUnit;
	unitNo_t	destinationUnit;
}UnicastDirectMsgRoutingHeader_s;

typedef struct UnicastDirectMsgHeader_s
{
	RadioMessageType messageType;
	RadioRoutingHeaderType routingHeaderType;
	UnicastDirectMsgRoutingHeader_s	routingHeader;
}UnicastDirectMsgHeader_s;


typedef struct UnicastRoutedMsgRoutingHeader_s
{

	unitNo_t 	fromUnit;
	unitNo_t	toUnit;
	unitNo_t 	sourceUnit;
	unitNo_t 	destinationUnit;
	uint8_t 	hopIndex;

}UnicastRoutedMsgRoutingHeader_s;

typedef struct UnicastRoutedMsgHeader_s
{
	RadioMessageType 	messageType;
	RadioRoutingHeaderType routingHeaderType;
	UnicastRoutedMsgRoutingHeader_s	routingHeader;

}UnicastRoutedMsgHeader_s;

typedef struct MulticastDirectMsgRoutingHeader_s
{
	unitNo_t	sourceUnit;
	uint8_t 	destinationUnitCount;

}MulticastDirectMsgRoutingHeader_s;


typedef struct MulticastRoutedMsgRoutingHeader_s
{
	unitNo_t	sourceUnit;
	uint8_t destinationUnitCount;

}MulticastRoutedMsgRoutingHeader_s;


typedef struct BroadcastMsgRoutingHeader_s
{
	unitNo_t	sourceUnit;
}BroadcastMsgRoutingHeader_s;




typedef struct LoRaGenericRoutingHeader
{
	RadioRoutingHeaderType				routingHeaderType;
	UnicastDirectMsgRoutingHeader_s 	unicastDirectHeader;
	UnicastRoutedMsgRoutingHeader_s 	unicastRoutedHeader;
	MulticastDirectMsgRoutingHeader_s	multicastDirectHeader;
	MulticastRoutedMsgRoutingHeader_s	multicastRoutedHeader;
	BroadcastMsgRoutingHeader_s			broadcastHeader;
}LoRaGenericRoutingHeader;

typedef struct LoRaGenericMessageStruct
{
	LoRaMessageMainHeader 		mainHeader;
	LoRaGenericRoutingHeader 	routingHeader;
	RadioServiceType			serviceType;
	uint8_t						data[RH_RF92_FIFO_SIZE];
	uint8_t     				dataLength;
	int16_t						SNR;
	int							RSSI;
}LoRaGenericMessageStruct;



typedef enum RadioManagerStates
{
	RADIO_TRANSMITTING,
	RADIO_WAITING_ACK,
	RADIO_NOT_INITIALIZED,
	RADIO_IDLE
}RadioManagerStates;


typedef enum Radio_EchoDevice
{
	SERIAL_DEVICE,
	BLUETOOTH_DEVICE,
	ETHERNET_CLIENT_DEVICE
}Radio_EchoDevice;


#endif /* RADIO_RADIOMANAGER_TYPESCONSTANTS_H_ */
