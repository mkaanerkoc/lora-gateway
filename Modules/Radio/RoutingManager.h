/*
 * RoutingManager.h
 *
 *  Created on: Jan 21, 2018
 *      Author: Kaan
 */

#ifndef RADIO_ROUTINGMANAGER_H_
#define RADIO_ROUTINGMANAGER_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "RTCManager.h"
#include "RadioManager_TypesConstants.h"
#include "Common_TypeDefs.h"
#include "DeviceManager.h"
#include "EEPROM_MemoryMap.h"

/*function prototypes */

#define MAX_UNIT_NO_IN_NET 									200
#define MAX_NEIGHBOURS_LIST_SIZE 							10
#define MAX_STATIC_ROUTING_TABLE_SIZE 						10
#define MAX_DYNAMIC_ROUTING_TABLE_SIZE 						10
#define NEIGHBOUR_ITEM_BYTE_SIZE_IN_CMD						7

typedef enum NeighbourState
{
	EMPTY_SLOT_NEIGHBOUR = 0x00,
	FULL_SLOT_NEIGHBOUR 	= 0x01
}NeighbourState;

typedef enum sRoutingTableElementState
{
	EMPTY_SLOT_S_ROUTING = 0x00,
	FULL_SLOT_S_ROUTING 	= 0x01
}sRoutingTableElementState;

typedef enum RoutingManagerReturnType
{
	ROUTING_FAILED,
	ROUTING_INVALID_PARAM,
	ROUTING_CANNOT_FOUND_NEXT_HOP,
	ROUTING_FOUND_IN_NEIGHBOURS,
	ROUTING_FOUND_IN_STATIC_TABLE,
	ROUTING_FOUND_IN_DYNAMIC_TABLE

}RoutingManagerReturnType;

typedef struct NeighbourUnitStruct
{
	unitNo_t LoRaID;
	uint8_t degree;
	int16_t linkRssi;
	TimeStruct_t lastPackageTime;
	NeighbourState  state;
}NeighbourUnitStruct;

/*Static Routing Table Elements */
// size : 3 byte
typedef struct sRoutingTableElement
{
	unitNo_t fromUnit;
	unitNo_t destinationUnit;
	unitNo_t toUnit;
	//sRoutingTableElementState state;
	//kaanbak : simdilik kapattim ileri de tum static routing table'ini okumak yerine bu state'e gore yuklemek / uzak komutaya gondermek / almak daha dogru olabilir
}sRoutingTableElement;

typedef struct dRoutingTableElement
{
	unitNo_t fromUnit;
	unitNo_t destinationUnit;
	unitNo_t toUnit;
}dRoutingTableElement;



bool								InitializeRoutingManager(void);


NeighbourUnitStruct* 				Routing_GetNeighbour( unitNo_t LoRaID );
uint8_t 							Routing_GetNeighboursListFullSlotsCount( void);
uint8_t 							Routing_GetNeighboursListEmptySlotsCount( void);
RoutingManagerReturnType			Routing_CalculateUnicastRoutingHeader ( unitNo_t destinationID,  LoRaGenericMessageStruct *genericMessageHeader );
RoutingManagerReturnType			Routing_CalculateMulticastSharingPlan ( unitNo_t *destinations, uint8_t destinationCount );

RoutingManagerReturnType			Routing_RouteReceivedPackage( LoRaGenericMessageStruct *loraMsg);
bool 								Routing_UpdateNeighbourList( LoRaGenericRoutingHeader *genericHeader,int16_t rssi); // kaanbak PARAMLAR Belirlenecek
bool								Routing_UpdateDynamicRoutingTable(); // kaanbak PARAMLAR Belirlenecek
bool								Routing_ProcessRoutingTableInformation(UnicastHeader unicastHeader,int16_t rssi); //bu fonksiyon diger unitelerden bize gelen routing table bilgisi ile kendi komsularimizi guncellemek icin kullanilan fonksiyon


/*privates */
bool								Routing_LoadStaticRoutingTable(Context_Enum context);

unitNo_t 							getDestinationUnitFromGenericRoutingHeader( LoRaGenericRoutingHeader *genericHeaderParam );
unitNo_t 							getFromUnitInfoFromGenericRoutingHeader( LoRaGenericRoutingHeader *genericHeader );
unitNo_t							getSourceUnitFromGenericRoutingHeader( LoRaGenericRoutingHeader *genericHeaderParam );
#endif /* RADIO_ROUTINGMANAGER_H_ */
