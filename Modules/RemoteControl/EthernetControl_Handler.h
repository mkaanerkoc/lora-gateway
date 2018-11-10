#ifndef ETHERNET_CONTROL_HANDLER_H
#define ETHERNET_CONTROL_HANDLER_H

#include "Common_TypeDefs.h"
#include "EthernetControl.h"
#include "DataChannelManager.h"
#include "RadioManager.h"
#include "mongoose.h"


void handleDeviceUniqueID_Set_ETHERNET(uint8_t *msgData,uint16_t msgLength,struct mg_connection *nc, Context_Enum contextParam );
void handleDeviceUniqueID_Get_ETHERNET(struct mg_connection *nc,Context_Enum context);

void handleDeviceLoRaID_Set_ETHERNET(uint8_t *msgData, uint16_t msgLength, struct mg_connection *nc, Context_Enum contextParam );
void handleDeviceLoRaID_Get_ETHERNET(struct mg_connection *nc,Context_Enum contextParam );

void handleDeviceLoRaGatewayID_Set_ETHERNET(uint8_t *msgData, uint16_t msgLength ,struct mg_connection *nc, Context_Enum contextParam );
void handleDeviceLoRaGatewayID_Get_ETHERNET(struct mg_connection *nc, Context_Enum context);

void handleLoRaConfiguration_Set_ETHERNET(uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );
void handleLoRaConfiguration_Get_ETHERNET(struct mg_connection *nc,Context_Enum context);


void handleLoRaEchoMonitor_Set_ETHERNET(uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );
void handleLoRaEchoMonitor_Get_ETHERNET(uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );


void handleLoRaSendPackage_ETHERNET(uint8_t *msgData, uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );



void handleRadioQueue_Get_ETHERNET(uint8_t *msgData, uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );


void handleLoRa_StaticRoutingTable_Set_ETHERNET( uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );
void handleLoRa_StaticRoutingTable_Get_ETHERNET( struct mg_connection *nc, Context_Enum contextParam );

void handle_DataChannels_All_Set_ETHERNET( uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );
void handle_DataChannels_All_Get_ETHERNET( struct mg_connection *nc, Context_Enum contextParam );


void handle_DataChannels_Set_ETHERNET( uint8_t *msgData,uint16_t msgLength, struct mg_connection *nc,Context_Enum contextParam );
void handle_DataChannels_Get_ETHERNET( struct mg_connection *nc, Context_Enum contextParam );

#endif