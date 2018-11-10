/*
 * RoutingManager.c
 *
 *  Created on: Jan 21, 2018
 *      Author: Kaan
 */


#include "RoutingManager.h"


NeighbourUnitStruct 			neighbourList[MAX_NEIGHBOURS_LIST_SIZE];
sRoutingTableElement 			staticRoutingTable[MAX_STATIC_ROUTING_TABLE_SIZE];
dRoutingTableElement 			dynamicRoutingTable[MAX_DYNAMIC_ROUTING_TABLE_SIZE];

bool	InitializeRoutingManager(void)
{
	Routing_LoadStaticRoutingTable(NON_THREAD);
	return true;
}

NeighbourUnitStruct* 			Routing_GetNeighbour(unitNo_t LoRaID)
{
	if(LoRaID > MAX_NEIGHBOURS_LIST_SIZE)
	{
		return NULL;
	}
	return &(neighbourList[LoRaID]);
}



RoutingManagerReturnType Routing_CalculateUnicastRoutingHeader(unitNo_t destinationUnitNoParam,  LoRaGenericMessageStruct *genericMessageHeader )
{
	RoutingManagerReturnType retVal = ROUTING_FAILED;

	int i=0;
	//param check
	if( destinationUnitNoParam > MAX_UNIT_NO_IN_NET )
	{
		retVal = ROUTING_INVALID_PARAM;
		return retVal;
	}

	// once elimizdeki komsularimizin listesine bakiyoruz...
	for(i = 0 ; i < MAX_NEIGHBOURS_LIST_SIZE ; i++)
	{
		if( destinationUnitNoParam == neighbourList[i].LoRaID && neighbourList[i].degree == 1) // bingo we find in 1st degree neighbors..
		{
			genericMessageHeader->routingHeader.routingHeaderType = UNICAST_DIRECT_HEADER;
			genericMessageHeader->routingHeader.unicastDirectHeader.sourceUnit = getDeviceLoRaID(false, NON_THREAD); // eeprom'dan okumayacagimiz icin , context parametresinin ne olacagi onemli degil...
			genericMessageHeader->routingHeader.unicastDirectHeader.destinationUnit = destinationUnitNoParam;
			return ROUTING_FOUND_IN_NEIGHBOURS;
		}
	}
	// 1. derece komsular arasinda bulamadik... static yonlendirme tablosuna bakiyoruz...
	for(i = 0 ; i < MAX_STATIC_ROUTING_TABLE_SIZE ; i++)
	{
		if( destinationUnitNoParam == staticRoutingTable[i].destinationUnit)
		{
			//if(staticRoutingTable[i].fromUnit == 0)
			//{

			//}
			//kaanbak -> suan icin from unit bilgisinde sinirlayicilik kullanilmiyor.. hatta sacma sapan bi kullanimi oldu suan... ileride routing bilgisine bir flag eklenerek routingType belirlenebilir
			// mesala ALLOW_EVERYONE flag'indeyse yonlendirme de kimden geldigi onemli olmaz gibi.. dusunmek lazim uzerinde...
			if(staticRoutingTable[i].toUnit == staticRoutingTable[i].destinationUnit)
			{
				genericMessageHeader->routingHeader.routingHeaderType = UNICAST_DIRECT_HEADER;
				genericMessageHeader->routingHeader.unicastDirectHeader.sourceUnit = getDeviceLoRaID(false, NON_THREAD); // eeprom'dan okumayacagimiz icin , context parametresinin ne olacagi onemli degil...
				genericMessageHeader->routingHeader.unicastDirectHeader.destinationUnit = destinationUnitNoParam;
			}
			else
			{
				genericMessageHeader->routingHeader.routingHeaderType = UNICAST_ROUTED_HEADER;
				genericMessageHeader->routingHeader.unicastRoutedHeader.sourceUnit 			= getDeviceLoRaID(false, NON_THREAD); // eeprom'dan okumayacagimiz icin , context parametresinin ne olacagi onemli degil...
				genericMessageHeader->routingHeader.unicastRoutedHeader.destinationUnit 	= destinationUnitNoParam;
				genericMessageHeader->routingHeader.unicastRoutedHeader.fromUnit 			= getDeviceLoRaID(false, NON_THREAD); // eeprom'dan okumayacagimiz icin , context parametresinin ne olacagi onemli degil...
				genericMessageHeader->routingHeader.unicastRoutedHeader.toUnit 				= staticRoutingTable[i].toUnit;
				genericMessageHeader->routingHeader.unicastRoutedHeader.hopIndex 			= 0; // paket source'dan ilk kez cikiyor,,, hopIndex = 0
			}

			return ROUTING_FOUND_IN_STATIC_TABLE;
		}
	}

	// static routing table'da da  bulamadik... dynamic yonlendirme tablosuna bakiyoruz...
	//kaanbak.. ileride dynamic yada static'den sadece bir tanesi kullanilmali.. suan ki implementasyonda static ve dinamik birlikte kullaniliyor ve statik'in onceligi
	//dynamic'e gore daha yuksek.. once static'e bakiyoruz dolayisi ile..
	for(i = 0 ; i < MAX_DYNAMIC_ROUTING_TABLE_SIZE ; i++)
	{
		if( destinationUnitNoParam == dynamicRoutingTable[i].destinationUnit)
		{

			//if(staticRoutingTable[i].fromUnit == 0)
			//{

			//}
			//kaanbak -> suan icin from unit bilgisinde sinirlayicilik kullanilmiyor.. hatta sacma sapan bi kullanimi oldu suan... ileride routing bilgisine bir flag eklenerek routingType belirlenebilir
			// mesala ALLOW_EVERYONE flag'indeyse yonlendirmede kimden geldigi onemli olmaz gibi.. dusunmek lazim uzerinde...
			genericMessageHeader->routingHeader.routingHeaderType 						= UNICAST_ROUTED_HEADER;
			genericMessageHeader->routingHeader.unicastRoutedHeader.sourceUnit 			= getDeviceLoRaID(false, NON_THREAD); // eeprom'dan okumayacagimiz icin , context parametresinin ne olacagi onemli degil...

			genericMessageHeader->routingHeader.unicastRoutedHeader.fromUnit 			= getDeviceLoRaID(false, NON_THREAD); // eeprom'dan okumayacagimiz icin , context parametresinin ne olacagi onemli degil...
			genericMessageHeader->routingHeader.unicastRoutedHeader.destinationUnit 	= destinationUnitNoParam;
			genericMessageHeader->routingHeader.unicastRoutedHeader.toUnit 				= dynamicRoutingTable[i].toUnit; //  // 1. dereceden komsuya paket cikariyoruz, to ve destination ID leri ayni olmak zorunda... aktarici kullanmayacagiz..
			genericMessageHeader->routingHeader.unicastRoutedHeader.hopIndex 			= 0; // paket source'dan ilk kez cikiyor,,, hopIndex = 0
			return ROUTING_FOUND_IN_DYNAMIC_TABLE;
		}
	}

	genericMessageHeader->routingHeader.routingHeaderType = UNICAST_DIRECT_HEADER;
	genericMessageHeader->routingHeader.unicastDirectHeader.sourceUnit = getDeviceLoRaID(false, NON_THREAD); // eeprom'dan okumayacagimiz icin , context parametresinin ne olacagi onemli degil...
	genericMessageHeader->routingHeader.unicastDirectHeader.destinationUnit = destinationUnitNoParam;
	//kaanbak ->  destination'u hic bi yerde bulamadik aq... sisteme ilk kez paket gondericek olabiliriz.. boyle bi durumda nasil yontem izlenilmeli uzerinde dusunulecek...
	return ROUTING_CANNOT_FOUND_NEXT_HOP;
}


RoutingManagerReturnType			Routing_RouteReceivedPackage(LoRaGenericMessageStruct *loraMsg)
{
	RoutingManagerReturnType retValueTemp = ROUTING_INVALID_PARAM;

	unitNo_t destinationUnitTemp = getDestinationUnitFromGenericRoutingHeader(&(loraMsg->routingHeader));
	//onay vermek icin kullanilacak..
	unitNo_t fromUnitTemp = getFromUnitInfoFromGenericRoutingHeader(&(loraMsg->routingHeader));

	RoutingManagerReturnType routingResultTemp = Routing_CalculateUnicastRoutingHeader(destinationUnitTemp, loraMsg);
	if ( routingResultTemp == ROUTING_FOUND_IN_STATIC_TABLE )
	{

	}

	/* burada onay atilabilir */

	addNewMessageToQueue( 	destinationUnitTemp,
							loraMsg->mainHeader.messageType,
							UNICAST_SERVICE,
							0
							,
							loraMsg->data,
							loraMsg->dataLength,
							500,
							3,
							true,
							ROUTING_OPERATION );
	return retValueTemp;
}

/*brief*/
/* bu fonksiyon havadan gelen paket icerisinden UnicastHeader' bilgisini kullanarak neighbourList ve dynamicRoutingTable'i gunceller..*/
bool			Routing_UpdateNeighbourList(LoRaGenericRoutingHeader *genericHeader,int16_t  rssi)
{
	TimeStruct_t now;
	RTC_GetTime(&now);
	TimeStruct_t timeStr; // time struct to be used in later
	unitNo_t fromUnitTemp = getFromUnitInfoFromGenericRoutingHeader( genericHeader );
	//search in neighborList if its exist
	for(int i = 0 ; i < MAX_NEIGHBOURS_LIST_SIZE ; i++)
	{
		NeighbourUnitStruct *nuStr = Routing_GetNeighbour(i);
		if(nuStr->LoRaID == fromUnitTemp )
		{
			// bingo ! we found this dude in our neighbourList but we should be sure on degree level first-todo
			// update rssi level & lastPackage rx time
			RTC_GetTime(&timeStr);
			nuStr->degree = 1; // 1 cunki direkt from olarak kaydettik...
			nuStr->linkRssi = rssi;
			memcpy(&(nuStr->lastPackageTime),&timeStr,sizeof(TimeStruct_t));
			nuStr->state = FULL_SLOT_NEIGHBOUR;
			return true;
		}
	}
	//buraya gelebilmissek daha once boyle bir komsumuzun olmadigini ogrenmis oluyoruz... ilk bos slot'a eklenecek
	for(int i = 0 ;  i < MAX_NEIGHBOURS_LIST_SIZE ; i++)
	{
		NeighbourUnitStruct *nuStr = Routing_GetNeighbour(i);
		if(nuStr->state == EMPTY_SLOT_NEIGHBOUR)
		{
			RTC_GetTime(&timeStr);
			nuStr->LoRaID = fromUnitTemp;
			nuStr->degree = 1; // 1 cunki direkt from olarak kaydettik...
			nuStr->linkRssi = rssi;
			memcpy(&(nuStr->lastPackageTime),&timeStr,sizeof(TimeStruct_t));
			nuStr->state = FULL_SLOT_NEIGHBOUR;
			return true;
		}
	}
	// buraya kadar gelebilmissek , kayit edecegimiz unite , mevcut komsu listesinde yok demek ve komsu listesinde bos slot yer yok demek... NEIGHBOUR_TIMEOUT suresinden fazla suredir paket alinamayan ilk komsuyu kaldirarak yerine bu uniteyi koyuyoruz
	for(int i = 0 ; i <MAX_NEIGHBOURS_LIST_SIZE ; i++)
	{
		NeighbourUnitStruct *nuStr = Routing_GetNeighbour(i);
	}
}

bool	Routing_UpdateDynamicRoutingTable()
{
	return true;
}

uint8_t 	Routing_GetNeighboursListFullSlotsCount(void)
{
	uint8_t retval = 0x00;
	for(int i = 0 ; i < MAX_NEIGHBOURS_LIST_SIZE ; i++)
	{
		if( FULL_SLOT_NEIGHBOUR  == neighbourList[i].state )
		{
			retval +=1;
		}
	}
	return retval;
}

uint8_t 	Routing_GetNeighboursListEmptySlotsCount(void)
{
	uint8_t retval = 0x00;
	for(int i = 0 ; i < MAX_NEIGHBOURS_LIST_SIZE ; i++)
	{
		if(FULL_SLOT_NEIGHBOUR  == neighbourList[i].state )
		{
			retval +=1;
		}
	}
	return retval;
}


bool	Routing_ProcessRoutingTableInformation(UnicastHeader unicastHeader,int16_t rssi)
{
	return false;
}

bool	Routing_LoadStaticRoutingTable(Context_Enum context)
{
	bool retValueTemp = true;
	uint8_t	 			routingTableData[sizeof(sRoutingTableElement)*MAX_STATIC_ROUTING_TABLE_SIZE];
	uint16_t 			messageLength = sizeof(sRoutingTableElement)*MAX_STATIC_ROUTING_TABLE_SIZE;

	EEPROM_ReadBulk(EEPROM_1, E2PROM_LORA_ROUTING_MAP_INDEX, routingTableData, messageLength, context);
	
	for(int i = 0 ; i < MAX_STATIC_ROUTING_TABLE_SIZE ; i++)
	{
		staticRoutingTable[i].fromUnit 			= (routingTableData[i 	  *	sizeof(sRoutingTableElement)] );
		staticRoutingTable[i].destinationUnit 	= (routingTableData[1 + i *	sizeof(sRoutingTableElement)] );
		staticRoutingTable[i].toUnit 			= (routingTableData[2 + i * sizeof(sRoutingTableElement)] );
	}
	return retValueTemp;
}

unitNo_t 	getFromUnitInfoFromGenericRoutingHeader( LoRaGenericRoutingHeader *genericHeaderParam )
{
	unitNo_t retValueTemp = 0x00;
	switch(genericHeaderParam->routingHeaderType)
	{
		case UNICAST_DIRECT_HEADER:
			retValueTemp = genericHeaderParam->unicastDirectHeader.sourceUnit;
			break;
		case UNICAST_ROUTED_HEADER:
			retValueTemp = genericHeaderParam->unicastRoutedHeader.fromUnit;
			break;
		case MULTICAST_DIRECT_HEADER:
			break;
		case MULTICAST_ROUTED_HEADER:
			break;
		case BROADCAST_HEADER:
			break;
		default:
			retValueTemp = 0;
			break;
	}

	return retValueTemp;
}

unitNo_t	getSourceUnitFromGenericRoutingHeader( LoRaGenericRoutingHeader *genericHeaderParam )
{
	unitNo_t retValueTemp = 0x00;
	switch(genericHeaderParam->routingHeaderType)
	{
		case UNICAST_DIRECT_HEADER:
			retValueTemp = genericHeaderParam->unicastDirectHeader.sourceUnit;
			break;
		case UNICAST_ROUTED_HEADER:
			retValueTemp = genericHeaderParam->unicastRoutedHeader.sourceUnit;
			break;
		case MULTICAST_DIRECT_HEADER:
			retValueTemp = genericHeaderParam->multicastDirectHeader.sourceUnit;
			break;
		case MULTICAST_ROUTED_HEADER:
			retValueTemp = genericHeaderParam->multicastRoutedHeader.sourceUnit;
			break;
		case BROADCAST_HEADER:
			retValueTemp = genericHeaderParam->broadcastHeader.sourceUnit;
			break;
		default:
			retValueTemp = 0;
			break;
	}
	return retValueTemp;
}

unitNo_t getDestinationUnitFromGenericRoutingHeader( LoRaGenericRoutingHeader *genericHeaderParam )
{
	unitNo_t retValueTemp = 0x00;
	switch(genericHeaderParam->routingHeaderType)
	{
		case UNICAST_DIRECT_HEADER:
			retValueTemp = genericHeaderParam->unicastDirectHeader.destinationUnit;
			break;
		case UNICAST_ROUTED_HEADER:
			retValueTemp = genericHeaderParam->unicastRoutedHeader.destinationUnit;
			break;
		case MULTICAST_DIRECT_HEADER:
		case MULTICAST_ROUTED_HEADER:
		case BROADCAST_HEADER:
		default:
			retValueTemp = INVALID_DEVICE_NO;
			break;
	}
	return retValueTemp;
}

bool	areWeDestinationOfThisPackage( LoRaGenericRoutingHeader *genericHeaderParam )
{
	bool retValueTemp = false;
	unitNo_t ownUnitNoTemp = getDeviceLoRaID( false, THREAD );

	switch( genericHeaderParam->routingHeaderType )
	{
		case	BROADCAST_HEADER:
			retValueTemp = true;
			break;
		case 	UNICAST_DIRECT_HEADER:
			retValueTemp = ( ownUnitNoTemp == genericHeaderParam->unicastDirectHeader.destinationUnit );
			break;
		case 	UNICAST_ROUTED_HEADER:
			retValueTemp = ( ownUnitNoTemp == genericHeaderParam->unicastRoutedHeader.destinationUnit );
			break;
		case 	MULTICAST_DIRECT_HEADER:

			break;
		case 	MULTICAST_ROUTED_HEADER:

			break;
		default:
			break;
	}
	return retValueTemp;
}

bool	areWeRouterOfThisPackage( LoRaGenericRoutingHeader *genericHeaderParam )
{
	bool retValueTemp = false;
	unitNo_t ownUnitNoTemp = getDeviceLoRaID( false, THREAD );

	switch( genericHeaderParam->routingHeaderType )
		{
			case	BROADCAST_HEADER:
			case 	UNICAST_DIRECT_HEADER:
			case 	MULTICAST_DIRECT_HEADER:
				retValueTemp = false;
				break;
			case 	UNICAST_ROUTED_HEADER:
				retValueTemp = ( ownUnitNoTemp == genericHeaderParam->unicastRoutedHeader.destinationUnit );
				break;
			case 	MULTICAST_ROUTED_HEADER:

				break;
			default:
				break;
		}
	return retValueTemp;
}
