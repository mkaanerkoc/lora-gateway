/*
 * RadioManager.h
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#ifndef RADIO_RADIOMANAGER_H_
#define RADIO_RADIOMANAGER_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "Common_TypeDefs.h"
#include "RadioManager_TypesConstants.h"
#include "SX1272Driver.h"
#include "Radio_QueueManager.h"
#include "RoutingManager.h"
#include "DeviceManager.h"
#include "RoutingManager.h"
#include "RadioManager_Interface.h"



#ifdef GATEWAY_CODE

	#include "TimerModule.h"
	#include <pthread.h>
	#include "mongoose.h"
	#include "EthernetControl.h"
	#include "EthernetControl_Interface.h"

	#define 	portTICK_PERIOD_MS 		1000 / 1000	

#else
	#include "DSP_Common_Types.h"
	#include "Channel_Manager.h"
	#include "SerialControl_Interface.h"
#endif

/*Defines */

#define RSSI_OFFSET                      -139

#define LORA_ISR_HANDLER_TASK_PRIORITY 	2
#define LORA_PROCESS_TIME_IN_MILLIS 	25


#ifndef GATEWAY_CODE // STM_CODE
				
	/* Globals */
	TaskHandle_t 						radioInterruptTaskHandler;
	SemaphoreHandle_t 					radioInterrupt_0_Semaphore;
	SemaphoreHandle_t 					radioInterrupt_1_Semaphore;
	SemaphoreHandle_t 					radioInterrupt_2_Semaphore;
	SemaphoreHandle_t 					radioInterrupt_3_Semaphore;

#else // GATEWAY_CODE
	pthread_t 							sx1272DriverPinReadTask;
	pthread_t 							loraEventHandlerTaskHandler;
	SoftwareTimerStruct 				radioTimeoutTimer,txTimeoutTimer;
#endif



RHMode 			_mode;


volatile uint8_t    				_bufLen; /// Number of octets in the buffer
uint8_t             				_buf[RH_RF92_MAX_PAYLOAD_LEN]; /// The receiver/transmitter buffer
volatile bool       				_rxBufValid; /// True when there is a valid message in the buffer
bool                				_usingHFport; // True if we are using the HF port (779.0 MHz and above)
int8_t              				_lastSNR; // Last measured SNR, dB
//int16_t 							_lastRssi;

RadioManagerStates					radioManagerState;
LoRaGenericMessageStruct			loraMessageStruct;
/*Task Functions */
void 								vRadioInterruptHandlerTask( void * pvParameters );



/*UserLevel Functions */

bool								InitializeRadioManager(void);

bool 								Radio_Begin(Context_Enum context);
void								Radio_WriteFIFO(uint8_t *dataPtrParam, uint16_t dataLengthParam , Context_Enum contextParam );



/* Radio Services  Functions */
bool								Radio_Send_Channels_Message( unitNo_t destination,uint8_t getChannelCounts,uint8_t *getChannelsIDs,uint8_t setChannelsCounts,uint8_t *setChannelsIDs,uint8_t *setChannelValues,uint16_t timeout,uint8_t retryCount);


uint8_t	 							Radio_Unicast_Send( unitNo_t destinationParam, RadioMessageType messageTypeParam, uint8_t *dataPtrParam, uint8_t dataLengthParam, uint16_t timeoutParam, Context_Enum contextParam);

bool 								Radio_Multicast_Send(const uint8_t *dataPtrParam, uint8_t 	dataLengthParam, unitNo_t *destinationsListParam, uint8_t destinationCountParam, RadioMessageType messageTypeParam, Context_Enum contextParam );

bool 								Radio_BroadCast_Send( const uint8_t *dataPtrParam, uint8_t 	dataLengthParam, RadioMessageType messageTypeParam, Context_Enum contextParam );

bool 								Radio_Send(const uint8_t *data, uint8_t length, Context_Enum context);

uint8_t								Radio_Send_Package( unitNo_t destination, RadioMessageType messageType, RadioServiceType serviceType,uint8_t *data,uint8_t length,uint32_t timeout , Context_Enum contextParam );


/* Radio Event Handler Functions */
void								Radio_Handle_TX_Done_Event(uint8_t irq_flags);
void								Radio_Handle_RX_Done_Event(uint8_t irq_flags);
void 								Radio_Handle_CAD_Done_Event(uint8_t irq_flags);
void 								Radio_Handle_RX_Timeout_Event(uint8_t irq_flags); // lora'nin rx timeout'u, bunu kullanmiyoruz pek


void								Radio_Handle_Payload_CRC_Error(uint8_t irq_flags);

void    							Radio_Handle_Response_Timeout_Event(void);


/*Timer Functions */
void 								Radio_Start_Response_Timer (uint32_t periodInMillisParam, Context_Enum contextParam);
void								Radio_Stop_Response_Timer( Context_Enum contextParam );
void 								Radio_SetResponseTimer_Timeout ( uint32_t periodInMillisParam,Context_Enum contextParam);

void								Radio_Start_TxTimeout_Timer(uint32_t periodInMillisParam, Context_Enum contextParam);
void 								Radio_Stop_TxTimeout_Timer( Context_Enum contextParam );
void 								Radio_Set_TxTimeout_Timer_Timeout( uint32_t periodInMillisParam, Context_Enum contextParam);

#ifndef GATEWAY_CODE

void 								vLoRaTimeoutTimerCallback( TimerHandle_t xTimer );
void								Radio_Handle_TX_Timeout_Callback( TimerHandle_t xTimer  ); /* lora modeminin tx interrupt'ini alamazsak bu callback cagirilacak*/

#else // GATEWAY_CODE

static void 						txTimeoutHandler(int sig, siginfo_t *si, void *uc);
static void 						responseTimeoutHandler(int sig, siginfo_t *si, void *uc);

#endif

/* Radio Processes Functions */

bool 								Radio_ReceiveIncomingPackage(void);
bool 								Radio_ParseIncomingPackage(uint8_t *data,uint8_t length);
RadioRoutingState					Radio_GetRouterState( void );
uint8_t 							Radio_ProcessIncomingPackage();

int									Radio_CalculateRSSI(int snr,int rssi);
void								Radio_EchoReceivedPackage(uint8_t *data,uint8_t length,int16_t rssi);


void 								Radio_EnableTCXO(Context_Enum context);
float 								Radio_GetFrequencyError(Context_Enum context);
bool 								Radio_CheckIsChannelActive(Context_Enum context);
int  	 							Radio_GetLastSNR(void);


void 								Radio_SetPromiscuous(bool promiscuous);

void								Radio_SetEchoSettings(Radio_EchoDevice echoId,bool enabled);
bool								Radio_GetEchoSettings(Radio_EchoDevice echoId);

int16_t 							Radio_GetLastRssi();

RadioServiceType 					findServiceTypeFromRoutingHeader(RadioRoutingHeaderType routingHeaderTypeParam  );
void 								Delay_WithMillis( uint32_t millisParam, Context_Enum contextParam );


#endif /* RADIO_RADIOMANAGER_H_ */
