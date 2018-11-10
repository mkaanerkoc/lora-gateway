#include "EthernetControl.h"

//Constants
const char *port1 = "1234";
uint8_t connectedClientCount;

void 	Initialize_EthernetControl(void)
{
	connectedClientCount = 0;
	mg_mgr_init(&ethernetManager, NULL);
	mg_bind(&ethernetManager, port1, ethernetEventHandler);
	pthread_create(&ethControlTask,NULL,EthernetControl_Task,NULL);
}

void 	Start_EthernetControl(void)
{
  	pthread_join(ethControlTask,NULL);
}

void	*EthernetControl_Task(void *arg)
{
  printf("<EthernetControl> Task Created ! \n");
  while(1)
  {
    mg_mgr_poll(&ethernetManager, 1000);
  }
  mg_mgr_free(&ethernetManager);
}

static void ethernetEventHandler(struct mg_connection *nc, int ev, void *p)
{
  struct mbuf *io = &nc->recv_mbuf;
  lastConnectedClient = nc;
  uint16_t messageType = 0;
  uint16_t messageLength = 0;
  (void) p;
  switch (ev)
  {
    case MG_EV_ACCEPT:
  		printf("MG_EV_ACCEPT \n");
  		connectedClientCount++;
    	Radio_SetEchoSettings(2, true);
      	break;
    case MG_EV_CLOSE:
    	connectedClientCount--;
    	if(connectedClientCount == 0)
    	{
    		Radio_SetEchoSettings(2, false);
    	}
      	printf("MG_EV_CLOSE\n");
      	break;
    case MG_EV_RECV:
    	messageType 	= (io->buf[2]<<8)|(io->buf[3]);
    	messageLength 	= (io->buf[4]<<8)|(io->buf[5]);
    	printf("Received Msg : Msg Type : %d , Msg Length : %d, Rx Length : %d\n",messageType,messageLength,io->len);
		ethernetReceivedMessageHandle(messageType,io->buf,io->len,nc);
		mbuf_remove(io, io->len);       // Discard message from recv buffer
		break;
    default:
		break;
  }
}

static void ethernetBroadcastEventHandler(struct mg_connection *nc, int ev, void *ev_data)
{
	(void) ev;
	char s[32];
	uint8_t *ev_data_temp = (uint8_t*)ev_data;
	uint16_t tempLength = (ev_data_temp[0] << 8 ) | ( ev_data_temp[1]);
	struct mg_connection *c;
	for (c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c))
	{
		if (c == nc) continue;

		mg_send(c, ev_data_temp + 2 , (tempLength - 2 ));

	}
}

void ethernetReceivedMessageHandle(uint16_t messageType,uint8_t *messageData,uint16_t messageLength,struct mg_connection *nc)
{
	switch(messageType)
	{
		case INCOMING_CMD_DEVICE_UNIQUE_ID_SET_REQUEST:
			handleDeviceUniqueID_Set_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_DEVICE_UNIQUE_ID_GET_REQUEST:
			handleDeviceUniqueID_Get_ETHERNET(nc,THREAD);
			break;
		case INCOMING_CMD_DEVICE_LORA_ID_SET_REQUEST:
			handleDeviceLoRaID_Set_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_DEVICE_LORA_ID_GET_REQUEST:
			handleDeviceLoRaID_Get_ETHERNET(nc,THREAD);
			break;
		case INCOMING_CMD_DEVICE_LORA_GATEWAY_ID_SET_REQUEST:
			handleDeviceLoRaGatewayID_Set_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_DEVICE_LORA_GATEWAY_ID_GET_REQUEST:
			handleDeviceLoRaGatewayID_Get_ETHERNET(nc,THREAD);
			break;
		case INCOMING_CMD_LORA_CODING_RATE_SET_REQUEST:
			break;
		case INCOMING_CMD_LORA_CODING_RATE_GET_REQUEST:
			break;
		case INCOMING_CMD_LORA_BANDWIDTH_SET_REQUEST:
			break;
		case INCOMING_CMD_LORA_BANDWIDTH_GET_REQUEST:
			break;
		case INCOMING_CMD_LORA_SPREADING_FACTOR_SET_REQUEST:
			break;
		case INCOMING_CMD_LORA_SPREADING_FACTOR_GET_REQUEST:
			break;
		case INCOMING_CMD_LORA_FREQUENCY_SET_REQUEST:
			break;
		case INCOMING_CMD_LORA_FREQUENCY_GET_REQUEST:
			break;
		case INCOMING_CMD_LORA_TX_POWER_SET_REQUEST:
			break;
		case INCOMING_CMD_LORA_TX_POWER_GET_REQUEST:
			break;
		case INCOMING_CMD_LORA_CONFIG_SET_REQUEST:
			handleLoRaConfiguration_Set_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_LORA_CONFIG_GET_REQUEST:
			handleLoRaConfiguration_Get_ETHERNET(nc,THREAD);
			break;
		case INCOMING_CMD_LORA_STATIC_ROUTING_TABLE_SET_REQUEST:
			handleLoRa_StaticRoutingTable_Set_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_LORA_STATIC_ROUTING_TABLE_GET_REQUEST:
			handleLoRa_StaticRoutingTable_Get_ETHERNET( nc, THREAD );
			break;
		case INCOMING_CMD_LORA_ECHO_MONITOR_SET_REQUEST:
			handleLoRaEchoMonitor_Set_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_LORA_ECHO_MONITOR_GET_REQUEST:
			handleLoRaEchoMonitor_Get_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_LORA_SEND_PACKAGE:
			handleLoRaSendPackage_ETHERNET(messageData+ETHERNET_PACKAGE_HEADER_LEN,messageLength,nc,THREAD);
			break;
		case INCOMING_CMD_DATA_CHANNELS_ALL_SET_REQUEST:
			handle_DataChannels_All_Set_ETHERNET ( messageData + ETHERNET_PACKAGE_HEADER_LEN, messageLength ,nc , THREAD );
			break;
		default:
			printf("<ethernetReceivedMessageHandle> messageType : %d not implemented yet \n",messageType);
			break;
	}
}

void EthernetControl_SendCommand(uint16_t msgID,uint8_t *data, uint16_t length, struct mg_connection *nc)
{
	uint8_t outputBuffer[length+8];
	outputBuffer[0] = COMMAND_STARTER_1;
	outputBuffer[1] = (uint8_t)(msgID >> 8);
	outputBuffer[2] = (uint8_t)(msgID & 0xFF);
	outputBuffer[3] = (uint8_t)(length >> 8);
	outputBuffer[4] = (uint8_t)(length & 0xFF);
	memcpy(outputBuffer+5,data,length);
	//kaanbak , buraya CRC hesaplanmasi gelecek...
	outputBuffer[length+5] = 0x23;
	outputBuffer[length+6] = 0x24;
	outputBuffer[length+7] = COMMAND_FINISHER_1;
	mg_send(nc,outputBuffer,length+8);
}

void EthernetControl_Broadcast(uint16_t msgID,uint8_t *data, uint16_t length )
{
	uint8_t outputBuffer[length+10];
	uint16_t tempLength = length + 10;
	outputBuffer[0] = (uint8_t)(tempLength >> 8 );
	outputBuffer[1] = (uint8_t)(tempLength & 0xFF );
	outputBuffer[2] = COMMAND_STARTER_1;
	outputBuffer[3] = (uint8_t)(msgID >> 8);
	outputBuffer[4] = (uint8_t)(msgID & 0xFF);
	outputBuffer[5] = (uint8_t)(length >> 8);
	outputBuffer[6] = (uint8_t)(length & 0xFF);
	memcpy(outputBuffer+7,data,length);
	//kaanbak , buraya CRC hesaplanmasi gelecek...
	outputBuffer[length+7] = 0x23;
	outputBuffer[length+8] = 0x24;
	outputBuffer[length+9] = COMMAND_FINISHER_1;

	mg_broadcast(&ethernetManager,ethernetBroadcastEventHandler, outputBuffer, tempLength );
}
