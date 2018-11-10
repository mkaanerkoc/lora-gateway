#ifndef ETHERNET_CONTROL_H
#define ETHERNET_CONTROL_H

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mongoose.h>
#include "Common_TypeDefs.h"

#include "EthernetControl_Interface.h"
#include "EthernetControl_Handler.h"


// Defines
#define 	ETHERNET_PACKAGE_HEADER_LEN 6
//Globals
pthread_t 		ethControlTask;
struct 			mg_mgr 	ethernetManager;
struct 			mg_connection *lastConnectedClient;
//struct 			

//Function Prototypes 

void 			Initialize_EthernetControl(void);
void 			Start_EthernetControl(void);
void 			EthernetControl_SendCommand(uint16_t msgID,uint8_t *data, uint16_t length, struct mg_connection *nc);
void			EthernetControl_Broadcast(uint16_t msgID,uint8_t *data, uint16_t length );
static void 	ethernetEventHandler( struct mg_connection *nc, int ev, void *p);
static void     ethernetBroadcastEventHandler( struct mg_connection *nc, int ev, void *p);
void 			ethernetReceivedMessageHandle(uint16_t messageType,uint8_t *messageData,uint16_t messageLength,struct mg_connection *nc);


//Task Prototypes
void 			*EthernetControl_Task(void *arg);

#endif