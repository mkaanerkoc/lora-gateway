/*
 * RadioManager.c
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#include "RadioManager.h"

/* private variables */
volatile bool 			_echoToSerialPort;
volatile bool			_echoToBluetooth;
volatile bool			_echoToEthernetClient;

volatile 	uint8_t lastReadIRQFlags;


#ifndef GATEWAY_CODE

TimerHandle_t 						radioResponseTimerHandler;
TimerHandle_t						sx1272TxTimeOutTimerHandler;

SemaphoreHandle_t 					radioInterrupt_0_Semaphore;
SemaphoreHandle_t 					radioInterrupt_1_Semaphore;
SemaphoreHandle_t 					radioInterrupt_2_Semaphore;
SemaphoreHandle_t 					radioInterrupt_3_Semaphore;

uint8_t 		responseTimeoutSemaphoreVar;
uint8_t			txTimeoutSemaphoreVar;

#else

/* Task Function Prototypes */
void 	*sx1272_Poll_Dio_Pin_Task();
void 	*loraEventHandlerTask();

pthread_mutex_t loraHandlerTaskMutex;
/* Private Function Prototypes */
static void txTimeoutHandler(int sig, siginfo_t *si, void *uc);
static void rxTimeoutHandler(int sig, siginfo_t *si, void *uc);

#endif


/* Global Functions */
bool InitializeRadioManager(void)
{


	/*BSP Driver Initializations */
	InitializeSX1272Driver();
	Initialize_LoRaMessageQueue();
	InitializeRoutingManager();

	radioManagerState = RADIO_IDLE;


	#ifndef GATEWAY_CODE
		BaseType_t xReturned;

		txTimeoutSemaphoreVar = 0;
		responseTimeoutSemaphoreVar = 0;

		/*Semaphore Initializations */
		radioInterrupt_0_Semaphore = xSemaphoreCreateBinary();
		radioInterrupt_1_Semaphore = xSemaphoreCreateBinary();
		radioInterrupt_2_Semaphore = xSemaphoreCreateBinary();
		radioInterrupt_3_Semaphore = xSemaphoreCreateBinary();
		/*Task Initializations */

		radioInterruptTaskHandler = NULL;
		xReturned = xTaskCreate( vRadioInterruptHandlerTask,  (const char* const) "LORA_ISR_TASK",  256,  NULL,   LORA_ISR_HANDLER_TASK_PRIORITY, &radioInterruptTaskHandler );

		radioResponseTimerHandler 		= xTimerCreate("LORA_PKG_TIMER",	(TickType_t)1000,	pdFALSE,	( void * ) 0,	vLoRaTimeoutTimerCallback);
		sx1272TxTimeOutTimerHandler 	= xTimerCreate("LORA_TX_TIMER",				(TickType_t)500, 	pdFALSE, 	( void * ) 0,   Radio_Handle_TX_Timeout_Callback );

		radioManagerState = RADIO_IDLE;
		if( xReturned != pdPASS )
		{
			// The task was created.  Use the task's handle to delete the task. //
			vTaskDelete( radioInterruptTaskHandler );
			return false;
		}
		else
		{
			return true;
		}
	#else // GATEWAY_CODE

		pthread_create(&sx1272DriverPinReadTask,NULL,sx1272_Poll_Dio_Pin_Task,NULL);
		pthread_create(&loraEventHandlerTaskHandler,NULL,loraEventHandlerTask,NULL);

	#endif

}

bool  Radio_Begin(Context_Enum context)
{
    uint8_t dummyReadRegister;
    dummyReadRegister = SX1272_ReadRegister(RH_RF92_REG_42_VERSION,context);
    if (dummyReadRegister == 00 || dummyReadRegister == 0xff)
    {
    	return false;
    }
	// Set sleep mode, so we can also set LORA mode:
	SX1272_WriteRegister(RH_RF92_REG_01_OP_MODE, (RH_RF92_MODE_SLEEP | RH_RF92_LONG_RANGE_MODE),context);

	Delay_WithMillis(20, context);


	// Check we are in sleep mode, with LORA set
	dummyReadRegister = SX1272_ReadRegister(RH_RF92_REG_01_OP_MODE,context);
	if (dummyReadRegister != (RH_RF92_MODE_SLEEP | RH_RF92_LONG_RANGE_MODE))
	{
		return false; // No device present?
	}

	// kaanbak, gpio interrupt ayarlamalari..

	// Set up FIFO
	// We configure so that we can use the entire 256 byte FIFO for either receive or transmit, but not both at the same time
	SX1272_WriteRegister(RH_RF92_REG_0E_FIFO_TX_BASE_ADDR, 0, context);
	SX1272_WriteRegister(RH_RF92_REG_0F_FIFO_RX_BASE_ADDR, 0, context);


	SX1272_SetMode(RHModeIdle,context);
	SX1272_SetBandwidth(BW_125Khz,context);
	SX1272_SetCodingRate(CR_4_5,context);
	SX1272_SetSpreadingFactor(SF_128C_Symbol,context);

	SX1272_SetPreambleLength(8,context); // Default is 8
	SX1272_SetFrequency(868.0,   context);	    // An innocuous ISM frequency, same as RF22's
	SX1272_SetTxPower(23,true,  context);	    // Lowish power
	
	return true;
}

void Radio_WriteFIFO( uint8_t *dataPtrParam, uint16_t dataLengthParam , Context_Enum contextParam )
{
	// FIFO Pointer'ini resetliyoruz...
	SX1272_WriteRegister(RH_RF92_REG_0D_FIFO_ADDR_PTR, 0, contextParam);

	//MainHeader'i ilk byte'a yaziyoruz...
	SX1272_WriteRegisters(RH_RF92_REG_00_FIFO, dataPtrParam ,dataLengthParam ,contextParam);
	SX1272_WriteRegister(RH_RF92_REG_22_PAYLOAD_LENGTH, ( dataLengthParam ),contextParam);
}


uint8_t 	Radio_Unicast_Send(		unitNo_t 			destinationParam,
									RadioMessageType 	messageTypeParam,
									uint8_t 			*dataPtrParam,
									uint8_t 			dataLengthParam,
									uint16_t 			timeoutParam,
									Context_Enum		contextParam )
{

	// not implemented yet..

	return 0x00;
}


bool Radio_Multicast_Send(		const uint8_t 		*dataPtrParam,
								uint8_t 			dataLengthParam,
								unitNo_t 			*destinationsListParam,
								uint8_t 			destinationCountParam,
								RadioMessageType 	messageTypeParam,
								Context_Enum 		contextParam )
{

	return false;

}

bool 		Radio_BroadCast_Send( const uint8_t *dataPtrParam, uint8_t 	dataLengthParam, RadioMessageType messageTypeParam, Context_Enum contextParam )
{

	return false;
}


uint8_t		Radio_Send_Package(		unitNo_t 			destinationUnitParam,
									RadioMessageType 	messageTypeParam,
									RadioServiceType 	serviceTypeParam,
									uint8_t 			*dataPtrParam,
									uint8_t 			dataLengthParam,
									uint32_t 			timeoutInMillisParam,
									Context_Enum		contextParam )
{

	unitNo_t ownUnitId = getDeviceLoRaID( false , THREAD );
	uint8_t mainHeaderByte; // main header byte

	uint8_t 	dataPtrTemp[RH_RF92_FIFO_SIZE];
	uint16_t 	dataLengthTemp = 0x00; // used to byte counts...


	mainHeaderByte 	= 0x00; // initialization of header byte

	// Radio State Check
	if(radioManagerState == RADIO_WAITING_ACK)
	{
		//printf("<RadioManager> Radio_Send_Package -> Radio in RADIO_WAITING_ACK state !! could not send : \n");
		return 0x01;
	}
	if(radioManagerState == RADIO_TRANSMITTING)
	{
		//printf("<RadioManager> Radio_Send_Package -> Radio in RADIO_TRANSMITTING state !! could not send : \n");
		return 0x02;
	}

	// Clear IRQ Flags
	SX1272_ClearInterruptRegisters( contextParam );

	//waitPacketSent(); //kaanbak -> bu fonksiyonlari da implement etmek lazim... Make sure we dont interrupt an outgoing message
	SX1272_SetMode( RHModeIdle, contextParam);

	if ( serviceTypeParam  ==  UNICAST_SERVICE)
	{

		RoutingManagerReturnType routingResultTemp = Routing_CalculateUnicastRoutingHeader( destinationUnitParam, &loraMessageStruct );

		mainHeaderByte 		= mainHeaderByte | ( ( messageTypeParam & 0x1F ) << 3 ); 								// message type -> 5 bit
		mainHeaderByte 		= mainHeaderByte | ( ( loraMessageStruct.routingHeader.routingHeaderType & 0x07 )  ); 	// routing header type -> 3 bit

		dataPtrTemp[dataLengthTemp] =  mainHeaderByte; // 0.Byte
		dataLengthTemp = dataLengthTemp + 1;

		if( routingResultTemp == ROUTING_FAILED || 	routingResultTemp == ROUTING_INVALID_PARAM )
		{
			return 0x04; // Error on calculating routing
		}

		//Write Routing Header
		if( UNICAST_DIRECT_HEADER == loraMessageStruct.routingHeader.routingHeaderType )
		{
			dataPtrTemp[dataLengthTemp] = loraMessageStruct.routingHeader.unicastDirectHeader.sourceUnit; // 1. Byte -- source
			dataLengthTemp = dataLengthTemp + 1;
			dataPtrTemp[dataLengthTemp] = loraMessageStruct.routingHeader.unicastDirectHeader.destinationUnit; // 2. Byte -- destination
			dataLengthTemp = dataLengthTemp + 1;
		}
		else if( UNICAST_ROUTED_HEADER == loraMessageStruct.routingHeader.routingHeaderType )
		{
			dataPtrTemp[dataLengthTemp] = loraMessageStruct.routingHeader.unicastRoutedHeader.sourceUnit; // 1. Byte -- sourceUnit
			dataLengthTemp = dataLengthTemp + 1;
			dataPtrTemp[dataLengthTemp] = loraMessageStruct.routingHeader.unicastRoutedHeader.destinationUnit; // 2. Byte -- destinationUnit
			dataLengthTemp = dataLengthTemp + 1;
			dataPtrTemp[dataLengthTemp] = loraMessageStruct.routingHeader.unicastRoutedHeader.toUnit; // 3. Byte -- toUnit
			dataLengthTemp = dataLengthTemp + 1;
			dataPtrTemp[dataLengthTemp] = loraMessageStruct.routingHeader.unicastRoutedHeader.fromUnit; // 4. Byte -- fromUnit
			dataLengthTemp = dataLengthTemp + 1;
			dataPtrTemp[dataLengthTemp] = loraMessageStruct.routingHeader.unicastRoutedHeader.hopIndex; // 5. Byte -- hopIndex
			dataLengthTemp = dataLengthTemp + 1;
		}
		// params check
		if ( dataLengthParam > RH_RF92_FIFO_SIZE - dataLengthTemp)
		{
			//printf("<RadioManager> Radio_Send_Package -> length param is INVALID !!  %d: \n",length);
			return 0x03;
		}

		//Write Data
		memcpy(dataPtrTemp + dataLengthTemp, dataPtrParam  , dataLengthParam  );
		dataLengthTemp += dataLengthParam;

	}
	else if( serviceTypeParam == MULTICAST_SERVICE )
	{
		// not implemented yet...

	}
	else if( serviceTypeParam == BROADCAST_SERVICE )
	{
		mainHeaderByte 		= mainHeaderByte | ( ( messageTypeParam & 0x1F ) << 3 ); 	// message type -> 5 bit
		mainHeaderByte 		= mainHeaderByte | ( ( BROADCAST_HEADER & 0x07 )  ); 	// routing header type -> 3 bit

		dataPtrTemp[dataLengthTemp] =  mainHeaderByte; // 0.Byte -- Main Header
		dataLengthTemp = dataLengthTemp + 1;

		//Write Routing Header
		dataPtrTemp[dataLengthTemp] =  ownUnitId; // 1.Byte -- Source Unit
		dataLengthTemp = dataLengthTemp + 1;

		// params check
		if ( dataLengthParam > RH_RF92_FIFO_SIZE - dataLengthTemp)
		{
			//printf("<RadioManager> Radio_Send_Package -> length param is INVALID !!  %d: \n",length);
			return 0x03;
		}

		//Write Data
		memcpy(dataPtrTemp + dataLengthTemp, dataPtrParam  , dataLengthParam  );
		dataLengthTemp += dataLengthParam;
	}
	else
	{
		return 0x04; // wrong service type
	}

	radioManagerState = RADIO_TRANSMITTING;

	Radio_WriteFIFO( dataPtrTemp, dataLengthTemp , contextParam );

    SX1272_SetMode( RHModeTx, contextParam ); // Start the transmitter

	Radio_Start_TxTimeout_Timer( 500, contextParam );
		
	return 0x00;
}

// Radio Event Handlers //
void	Radio_Handle_TX_Done_Event(uint8_t irq_flags)
{

	LoRaMessageQueueElement *queueElement;
	_txGood++;
	const TickType_t xDelay = LORA_PROCESS_TIME_IN_MILLIS / portTICK_PERIOD_MS;

	queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex); // queue'da en son kullandigimiz elemani aldik
	radioManagerState = RADIO_IDLE;
	SX1272_SetMode( RHModeRx , THREAD); // gonderme yaptiktan sonra hemen RX ' e geciyoruz...
	Radio_Stop_TxTimeout_Timer( THREAD );

	//kaanbak: burada paketin uni mi multi mi oldugu duruma gore bekleme sureleri degisecek...

	//burada gonderdigimiz son paketin Response bekleyip beklemedigi kontrol ediliyor.. eger Yanit Beklenmiyor ise isleme gecikmesi verip queue'yi triggerlayacagiz...
	bool areWeWaitingAnyResponseToLastSentPackage = checkThatMessageHaveResponse(queueElement->mainHeader.messageType );
	#ifdef GATEWAY_CODE
		printf("TX_DONE : MessageType : %d\n", queueElement->mainHeader.messageType);
	#endif
	if( !areWeWaitingAnyResponseToLastSentPackage ) // yanit yada ACK beklemedigimiz bir mesaj bu dolayisiyla atiyoruz direkt..
	{
		Delay_WithMillis(xDelay, THREAD );
		stepMessageQueue(TX_DONE);
	}
	else
	{
		#ifdef GATEWAY_CODE
		printf("Yanit bekledigimiz bir mesaj oldugu icin queue'yi ilerletemiyoruz..\n");
		#endif
		Radio_Start_Response_Timer(queueElement->timeout, THREAD );
	}
}

void	Radio_Handle_RX_Done_Event(uint8_t irq_flags)
{
	bool tempRetVal = false;
	RadioRoutingState routingState = NOT_SET_ROUTING_INFO;
	//Receive Package
	tempRetVal = Radio_ReceiveIncomingPackage();
	//UpdateNeighbourhoods

	if(tempRetVal)
	{
		tempRetVal = Radio_ParseIncomingPackage(_buf,_bufLen);
	}
	else
	{
		//printf("Received Package is not valid ...\n");
		return; // package is not valid...
	}
	//update NeighbourList
	Routing_UpdateNeighbourList(&(loraMessageStruct.routingHeader),loraMessageStruct.RSSI);
	Radio_EchoReceivedPackage(_buf,_bufLen, loraMessageStruct.RSSI );
	//	GetRouterState
	routingState = Radio_GetRouterState();

	if( routingState == DESTINATION_OF_THIS_PACKAGE)
	{
		//printf("We are destination of the package \n");
		Radio_ProcessIncomingPackage();
		// check if it's the package that we are waitin
		// if so stepQueueManager
	}
	else if(routingState == ROUTER_OF_PACKAGE)
	{
		//printf("We are router of the package \n");
		// we are transparent to package, only add to queue
		//TODO
		Routing_RouteReceivedPackage(&loraMessageStruct);
	}
	else if(routingState == INDEPENDENT_WITH_PACKAGE)
	{
		//
		//printf("We are independent  with this package \n");
	}
	else if(routingState == WRONG_ROUTED_MESSAGE)
	{
		//
		//printf("WRONG_ROUTED_MESSAGE \n");
	}
	else
	{
		return;
	}
	// We have received a message.
	//validateRxBuf(); -> kaanback bu fonksiyonlar implement edilecek
	//Routing_ProcessRoutingTableInformation();
	if ( _rxBufValid )
	{
		SX1272_SetMode( RHModeIdle, THREAD); // Got one
	}
	radioManagerState = RADIO_IDLE;
}

void 	Radio_Handle_CAD_Done_Event(uint8_t irq_flags)
{
	 _cad = irq_flags & RH_RF92_CAD_DETECTED;
	 SX1272_SetMode(RHModeIdle,THREAD);
}

void 	Radio_Handle_RX_Timeout_Event(uint8_t irq_flags)
{
	_rxBad++;
}

void	Radio_Handle_Payload_CRC_Error(uint8_t irq_flags)
{
	_rxBad++;
}

void    Radio_Handle_Response_Timeout_Event(void)
{
	#ifndef GATEWAY_CODE
		HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
	#endif
	radioManagerState = RADIO_IDLE;
	SX1272_ClearInterruptRegisters( THREAD );
	//Radio_Stop_Response_Timer( THREAD );
	LoRaMessageQueueElement *queueElement;

	queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex); // queue'da en son kullandigimiz elemani aldik

	if(queueElement->remainingRetryCount == 0)
	{
		handleErrorMessage( false , queueElement->mainHeader.messageType );
		stepMessageQueue( RESPONSE_FAILED );
	}
	else
	{
		stepMessageQueue( RESPONSE_TIMEOUT );
	}
	return;
}



bool Radio_ReceiveIncomingPackage()
{
	// Have received a packet
	int8_t snrTemp;
	int16_t rssiTemp;
	bool retVal = false;
	uint8_t len = SX1272_ReadRegister(RH_RF92_REG_13_RX_NB_BYTES,THREAD);
	// Reset the fifo read ptr to the beginning of the packet
	SX1272_WriteRegister(RH_RF92_REG_0D_FIFO_ADDR_PTR, SX1272_ReadRegister(RH_RF92_REG_10_FIFO_RX_CURRENT_ADDR,THREAD), THREAD);
	SX1272_ReadRegisters(RH_RF92_REG_00_FIFO, _buf, len,THREAD);
	_bufLen = len;
	SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, THREAD); // Clear all IRQ flags

	// SX1272 nin ornek kodundan alindi
	_lastSNR = (int8_t) SX1272_ReadRegister(RH_RF92_REG_19_PKT_SNR_VALUE,THREAD);
	if( _lastSNR & 0x80 ) // The SNR sign bit is 1
	{
		// Invert and divide by 4
		snrTemp = ( ( ~_lastSNR + 1 ) & 0xFF ) >> 2;
		snrTemp = -snrTemp;
	}
	else
	{
		// Divide by 4
		snrTemp = ( _lastSNR & 0xFF ) >> 2;
	}

	rssiTemp = SX1272_ReadRegister(RH_RF92_REG_1A_PKT_RSSI_VALUE, THREAD);
	if( snrTemp < 0 )
	{
		_lastRssi= RSSI_OFFSET + rssiTemp + ( rssiTemp >> 4 ) + snrTemp;
	}
	else
	{
		_lastRssi = RSSI_OFFSET + rssiTemp + ( rssiTemp >> 4 );
	}
	
	loraMessageStruct.RSSI = _lastRssi;
	loraMessageStruct.SNR = _lastSNR;
	if(_bufLen < MINIMUM_LORA_PACKAGE_SIZE)
	{
		retVal = false;
	}
	else
	{
		retVal = true;
	}
	// debug print
	/*printf(" Received Package : ");
	for ( int i = 0 ; i < _bufLen ; i++)
	{
		printf(" %d ",_buf[ i ]);
	}
	printf("\n"); */
	return retVal;
}

bool Radio_ParseIncomingPackage(uint8_t *data, uint8_t length)
{
	uint8_t messageHeaderLength = 0;
	bool retVal 				= false;
	memset(&loraMessageStruct, 0, sizeof(LoRaGenericMessageStruct));

	//parsing Message Type
	loraMessageStruct.mainHeader.messageType 		= (RadioMessageType)( ( data[0] & 0xF8 ) >> 3 ); // Message Type mask = 0b1111 1000
	//parsing Routing HeaderType
	loraMessageStruct.mainHeader.routingHeaderType 	= (RadioRoutingHeaderType) ( ( data[0] & 0x07) ); // Routing Header Mask = 0b0000 0111
	loraMessageStruct.routingHeader.routingHeaderType  = loraMessageStruct.mainHeader.routingHeaderType;
	//parsing Service Type
	loraMessageStruct.serviceType 					= findServiceTypeFromRoutingHeader(loraMessageStruct.mainHeader.routingHeaderType);


	//Parsing Routing Header
	if( loraMessageStruct.mainHeader.routingHeaderType == UNICAST_DIRECT_HEADER)
	{
		loraMessageStruct.routingHeader.unicastDirectHeader.sourceUnit 		= data[1];
		loraMessageStruct.routingHeader.unicastDirectHeader.destinationUnit = data[2];
		messageHeaderLength = 3;
	}
	else if( loraMessageStruct.mainHeader.routingHeaderType ==  UNICAST_ROUTED_HEADER)
	{
		//kaanbak bunlari definelamak gerek aslnda
		loraMessageStruct.routingHeader.unicastRoutedHeader.sourceUnit 		= data[1];
		loraMessageStruct.routingHeader.unicastRoutedHeader.destinationUnit = data[2];
		loraMessageStruct.routingHeader.unicastRoutedHeader.toUnit 			= data[3];
		loraMessageStruct.routingHeader.unicastRoutedHeader.fromUnit 		= data[4];
		loraMessageStruct.routingHeader.unicastRoutedHeader.hopIndex 		= data[5];

		messageHeaderLength = 6;
	}

	else if( loraMessageStruct.mainHeader.routingHeaderType ==  MULTICAST_DIRECT_HEADER)
	{
		uint8_t destinationUnitCountTemp = data[1];
		for(int i = 0 ; i < destinationUnitCountTemp ; i++ )
		{

		}
		//not implemented yet...
	}
	else if( loraMessageStruct.mainHeader.routingHeaderType ==  MULTICAST_ROUTED_HEADER )
	{
		//not implemented yet...
	}

	else if( loraMessageStruct.mainHeader.routingHeaderType ==  BROADCAST_HEADER)
	{
		loraMessageStruct.routingHeader.broadcastHeader.sourceUnit 			= data[1];
		messageHeaderLength = 2;
	}

	if(length <= messageHeaderLength)
	{
		retVal = false;
	}
	else
	{
		//kaankontrolet &() idi () yaptik
		memcpy((loraMessageStruct.data),data+messageHeaderLength,length-messageHeaderLength);
		retVal =  true;
	}
	return retVal;
}

RadioRoutingState	Radio_GetRouterState( void )
{
	uint16_t ownUnitNo;
	RadioRoutingState retVal;
	retVal = NOT_SET_ROUTING_INFO;
	ownUnitNo = getDeviceLoRaID(false,NON_THREAD);


	switch(loraMessageStruct.mainHeader.routingHeaderType)
	{
		case UNICAST_DIRECT_HEADER:
			if( ownUnitNo == loraMessageStruct.routingHeader.unicastDirectHeader.destinationUnit )
			{
				retVal = DESTINATION_OF_THIS_PACKAGE;
			}
			else
			{
				retVal = INDEPENDENT_WITH_PACKAGE;
			}
			break;
		case UNICAST_ROUTED_HEADER:
			if(			ownUnitNo == loraMessageStruct.routingHeader.unicastRoutedHeader.toUnit &&
						ownUnitNo != loraMessageStruct.routingHeader.unicastRoutedHeader.destinationUnit) // Aktarici olarak davranacagiz..
			{
				retVal = ROUTER_OF_PACKAGE;
			}
			else if(	ownUnitNo == loraMessageStruct.routingHeader.unicastRoutedHeader.toUnit &&
						ownUnitNo == loraMessageStruct.routingHeader.unicastRoutedHeader.destinationUnit) // paketin destination'u biziz..
			{
				retVal = DESTINATION_OF_THIS_PACKAGE;
			}
			else if(	ownUnitNo != loraMessageStruct.routingHeader.unicastRoutedHeader.toUnit &&
						ownUnitNo == loraMessageStruct.routingHeader.unicastRoutedHeader.destinationUnit) // paketin 'to su biz degiliz ama hedefi biziz ... hatali bir route olusmus burada
			{
				retVal = WRONG_ROUTED_MESSAGE;
			}
			else if(	ownUnitNo != loraMessageStruct.routingHeader.unicastRoutedHeader.toUnit &&
						ownUnitNo != loraMessageStruct.routingHeader.unicastRoutedHeader.destinationUnit) // paket'in to'su da dest'i de biz degiliz.. bizim ile alakali bir package deil
			{
				retVal = INDEPENDENT_WITH_PACKAGE;
			}
			else
			{
				retVal = NOT_SET_ROUTING_INFO; // yukaridaki 4'unden baska bi case yok zaten ama koruma amacli koydum buraya.
			}
			break;
		case MULTICAST_DIRECT_HEADER:
			for(int i = 0 ; loraMessageStruct.routingHeader.multicastDirectHeader.destinationUnitCount ; i ++)
			{

			}
			break;
		case MULTICAST_ROUTED_HEADER:
			for(int i = 0 ; loraMessageStruct.routingHeader.multicastRoutedHeader.destinationUnitCount ; i ++)
			{

			}
			break;
		case BROADCAST_HEADER:
			retVal = DESTINATION_OF_THIS_PACKAGE;
			break;
		default:
			retVal = NOT_SET_ROUTING_INFO;
			break;

	}

	return retVal;
}

uint8_t Radio_ProcessIncomingPackage()
{

	RadioMessageType				waitingResponseMessageType;
	LoRaMessageQueueElement 		*queueElement;
	bool							areWeWaitingAnyResponse = false;
	bool 							areWeReceivedPackageWeAreWaiting = false;
	bool							receivedPackageIsRequestMsgTemp = false;


	// buraya gelmisse bu paketin destination'u bizizdir zaten..
	receivedPackageIsRequestMsgTemp = checkThatMessageIsRequest(loraMessageStruct.mainHeader.messageType);

	if ( receivedPackageIsRequestMsgTemp  ) // bekledigimiz paketten bagimsiz olarak bize bir request gelmisse, onu da isleyip queue'miza ekleyebiliriz o sirada..
	{
		handleReceivedMessage(false , receivedPackageIsRequestMsgTemp, loraMessageStruct.mainHeader.messageType );
	}
	else // bir yanit mesaji geldi...
	{
		queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex); // package bekleyip beklemedigimizi anlamamiz lazim..
		// queue uzerinde cevap bekledigimiz bir mesaj var mi ? bakalim

		if(queueElement != NULL)
		{
			areWeWaitingAnyResponse = getResponseMessageIDofThisMessage(queueElement->mainHeader.messageType , &waitingResponseMessageType );
		}
		else // queue'muz bos zaten yanit bekledigimiz bir sey olamaz
		{
			areWeWaitingAnyResponse = false;
		}

		if( areWeWaitingAnyResponse )
		{
			// bekledigimiz paket'in gelip gelmedigini anlamamiz gerekiyor burada...
			// gonderdigimiz paket'in destination'unu ve gelen paket'in source'ini karsilastiralim... check1
			unitNo_t receivedSourceUnitTemp = getSourceUnitFromGenericRoutingHeader( &(loraMessageStruct.routingHeader) );
			unitNo_t expectedSourceUnitTemp = queueElement->destinationUnit;
			if( receivedSourceUnitTemp == expectedSourceUnitTemp )
			{
				if( waitingResponseMessageType  == loraMessageStruct.mainHeader.messageType) // bekledigimiz message id' si gelip gelmedigini kontrol ediyoruz.. check2..
				{
					Radio_Stop_Response_Timer(THREAD); 	// bekledigimiz paket gelmise responseTimer'i kapatabiliriz...//
					areWeReceivedPackageWeAreWaiting = true;
				}
			}
		}

		/* burada messageType 'a gore handler callback'i cagirilmali*/
		handleReceivedMessage( areWeReceivedPackageWeAreWaiting, receivedPackageIsRequestMsgTemp, loraMessageStruct.mainHeader.messageType );

		if(queueElement != NULL ) //kaanbak : buraya queue'nin kesin bos olmadigi kontrolu eklenecek...
		{
			if ( areWeReceivedPackageWeAreWaiting )
			{
				stepMessageQueue( RESPONSE_RECEIVED );
			}
		}
	}
	return 0x01;
}



/* device-abstracted function */
void 	Radio_Start_Response_Timer(uint32_t periodInMillisParam, Context_Enum contextParam)
{
	#ifndef GATEWAY_CODE

	TickType_t xDelay = periodInMillisParam / portTICK_PERIOD_MS;

	if( radioResponseTimerHandler != NULL)
	{
		if( contextParam != FROM_ISR)
		{
			xTimerStop( radioResponseTimerHandler, 0 );
			xTimerChangePeriod( radioResponseTimerHandler, xDelay, 0);
		}
		else if( contextParam == FROM_ISR)
		{
			xTimerStopFromISR( radioResponseTimerHandler, 0 );
			xTimerChangePeriodFromISR( radioResponseTimerHandler, xDelay, 0);
		}
		else
		{
			return;
		}
	}

	#else // gateway_code

	radioTimeoutTimer.timerPeriodInMillis = periodInMillisParam;
	radioTimeoutTimer.timerCallbackFunction = responseTimeoutHandler;
	SetupSoftTimer(&radioTimeoutTimer);

	#endif
}

/* device-abstracted function */
void 	Radio_Stop_Response_Timer( Context_Enum contextParam )
{
	#ifndef GATEWAY_CODE
	if( radioResponseTimerHandler != NULL)
	{
		if(contextParam != FROM_ISR)
		{
			xTimerStop( radioResponseTimerHandler, 0 );
		}
		else if(contextParam == FROM_ISR)
		{
			xTimerStopFromISR( radioResponseTimerHandler, 0 );
		}
		else
		{
			return;
		}
	}
	#else
		timer_delete(radioTimeoutTimer.timerID);
	#endif
}

/* device-abstracted function */
void 	Radio_SetResponseTimer_Timeout( uint32_t periodInMillisParam ,Context_Enum contextParam )
{
	#ifndef GATEWAY_CODE
	TickType_t xDelay = periodInMillisParam / portTICK_PERIOD_MS;
	if(radioResponseTimerHandler != NULL)
	{
		if( contextParam != FROM_ISR)
		{
			xTimerChangePeriod(radioResponseTimerHandler, xDelay, 0);
		}
		else if( contextParam == FROM_ISR)
		{
			xTimerChangePeriodFromISR(radioResponseTimerHandler, xDelay,0);
		}
		else
		{
			return;
		}
	}
	#else
	//gateway:todo

	#endif
}

/* device-abstracted function */
void	Radio_Start_TxTimeout_Timer( uint32_t periodInMillisParam, Context_Enum contextParam )
{
	TickType_t xDelay = periodInMillisParam/portTICK_PERIOD_MS;
#ifndef GATEWAY_CODE
	if( sx1272TxTimeOutTimerHandler != NULL)
	{
		if(contextParam != FROM_ISR)
		{
			if ( pdFAIL == xTimerStop( sx1272TxTimeOutTimerHandler, 0 ))
			{
				configASSERT(0);
			}
			xTimerChangePeriod( sx1272TxTimeOutTimerHandler, xDelay, 0);
		}
		else if(contextParam == FROM_ISR)
		{
			if ( pdFAIL == xTimerStopFromISR( sx1272TxTimeOutTimerHandler, 0 ))
			{
				configASSERT(0);
			}
			xTimerChangePeriodFromISR( sx1272TxTimeOutTimerHandler, xDelay, 0);
		}
		else
		{
			return;
		}
	}
#else // GATEWAY_CODE
	radioTimeoutTimer.timerPeriodInMillis = periodInMillisParam;
	radioTimeoutTimer.timerCallbackFunction = txTimeoutHandler;
	SetupSoftTimer(&radioTimeoutTimer);
#endif
}

/* device-abstracted function */
void 	Radio_Stop_TxTimeout_Timer( Context_Enum contextParam )
{
#ifndef GATEWAY_CODE
	if( sx1272TxTimeOutTimerHandler != NULL)
	{
		if(contextParam != FROM_ISR)
		{
			xTimerStop( sx1272TxTimeOutTimerHandler, 0 );
		}
		else if(contextParam == FROM_ISR)
		{
			xTimerStopFromISR( sx1272TxTimeOutTimerHandler, 0 );
		}
		else
		{
			return;
		}
	}
#else  // GATEWAY_CODE
	timer_delete(radioTimeoutTimer.timerID);
#endif
}


void 	Radio_Set_TxTimeout_Timer_Timeout( uint32_t periodInMillisParam, Context_Enum contextParam)
{
	#ifndef GATEWAY_CODE
	TickType_t xDelay = periodInMillisParam/portTICK_PERIOD_MS;
	if(sx1272TxTimeOutTimerHandler != NULL)
	{
		if(contextParam != FROM_ISR)
		{
			xTimerChangePeriod( sx1272TxTimeOutTimerHandler, xDelay, 0);
		}
		else if(contextParam == FROM_ISR)
		{
			xTimerChangePeriodFromISR( sx1272TxTimeOutTimerHandler, xDelay,0);
		}
		else
		{
			return;
		}
	}
	#else  // GATEWAY_CODE
	//gateway:todo
	#endif
}



#ifndef GATEWAY_CODE // stm codes


void 	vLoRaTimeoutTimerCallback( TimerHandle_t xTimer )
{
	txTimeoutSemaphoreVar = 0;
	responseTimeoutSemaphoreVar = 1;

	xSemaphoreGive(radioInterrupt_0_Semaphore );
	//Radio_Handle_Response_Timeout_Event();
}


void	Radio_Handle_TX_Timeout_Callback( TimerHandle_t xTimer )
{
	txTimeoutSemaphoreVar = 1;
	responseTimeoutSemaphoreVar = 0;
	xSemaphoreGive( radioInterrupt_0_Semaphore );
}

#endif

void vRadioInterruptHandlerTask( void * pvParameters )
{
#ifndef GATEWAY_CODE
	while(1)
	{
		if(pdTRUE == xSemaphoreTake(radioInterrupt_0_Semaphore,portMAX_DELAY))
		{
			// Read the interrupt register
			lastReadIRQFlags = SX1272_ReadRegister(RH_RF92_REG_12_IRQ_FLAGS, THREAD);
			if( 1 == txTimeoutSemaphoreVar && 0 == responseTimeoutSemaphoreVar )
			{
				txTimeoutSemaphoreVar = 0;
				responseTimeoutSemaphoreVar = 0;
				//Radio_Stop_TxTimeout_Timer( THREAD );

				SX1272_SetMode( RHModeIdle, THREAD);
				radioManagerState = RADIO_IDLE;
				Radio_Stop_TxTimeout_Timer( THREAD );
				SX1272_ClearInterruptRegisters( THREAD );
				stepMessageQueue( TX_TIMEOUT );

			}
			else if( 0 == txTimeoutSemaphoreVar && 1 == responseTimeoutSemaphoreVar )
			{

				txTimeoutSemaphoreVar = 0;
				responseTimeoutSemaphoreVar = 0;
				Radio_Handle_Response_Timeout_Event();
				SX1272_ClearInterruptRegisters( THREAD );
			}
		    if (_mode == RHModeRx && (lastReadIRQFlags & RH_RF92_PAYLOAD_CRC_ERROR))
		    {
		    	Radio_Handle_Payload_CRC_Error(lastReadIRQFlags);
				SX1272_ClearInterruptRegisters( THREAD );
		    }
		    else if(_mode == RHModeRx && (lastReadIRQFlags & RH_RF92_RX_TIMEOUT))
		    {
		    	Radio_Handle_RX_Timeout_Event(lastReadIRQFlags);
				SX1272_ClearInterruptRegisters( THREAD );
		    }
		    else if (_mode == RHModeRx && (lastReadIRQFlags & RH_RF92_RX_DONE))
		    {
		    	Radio_Handle_RX_Done_Event(lastReadIRQFlags);
				SX1272_ClearInterruptRegisters( THREAD );
		    }
		    else if (_mode == RHModeTx && (lastReadIRQFlags & RH_RF92_TX_DONE))
		    {
		    	Radio_Handle_TX_Done_Event(lastReadIRQFlags);
				SX1272_ClearInterruptRegisters( THREAD );
		    }
		    else if (_mode == RHModeCad && (lastReadIRQFlags & RH_RF92_CAD_DONE))
		    {
		    	Radio_Handle_CAD_Done_Event(lastReadIRQFlags);
				SX1272_ClearInterruptRegisters( THREAD );
		    }
		    // Sigh: on some processors, for some unknown reason, doing this only once does not actually
		    // clear the radio's interrupt flag. So we do it twice. Why?

		    
		}
	}
	#else  // GATEWAY_CODE

	// Read the interrupt register
 	uint8_t irq_flags = SX1272_ReadRegister(RH_RF92_REG_12_IRQ_FLAGS, THREAD);
    if (_mode == RHModeRx && (irq_flags & RH_RF92_PAYLOAD_CRC_ERROR))
    {
    	Radio_Handle_Payload_CRC_Error(irq_flags);
    }
    else if(_mode == RHModeRx && (irq_flags & RH_RF92_RX_TIMEOUT))
    {
    	Radio_Handle_RX_Timeout_Event(irq_flags);
    }
    else if (_mode == RHModeRx && (irq_flags & RH_RF92_RX_DONE))
    {
    	//printf("Rx_Done_Event \n");
    	Radio_Handle_RX_Done_Event(irq_flags);
    }
    else if (_mode == RHModeTx && (irq_flags & RH_RF92_TX_DONE))
    {
    	//printf("Tx_Done_Event \n");
    	Radio_Handle_TX_Done_Event(irq_flags);
    }
    else if (_mode == RHModeCad && (irq_flags & RH_RF92_CAD_DONE))
    {
    	Radio_Handle_CAD_Done_Event(irq_flags);
    }
    // Sigh: on some processors, for some unknown reason, doing this only once does not actually
    // clear the radio's interrupt flag. So we do it twice. Why?
    SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, THREAD); // Clear all IRQ flags
    SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, THREAD); // Clear all IRQ flags

	#endif
}


//Utils Functions//

void  Radio_EchoReceivedPackage(uint8_t *data,uint8_t length,int16_t rssi)
{
	if(Radio_GetEchoSettings(SERIAL_DEVICE))
	{
		#ifndef GATEWAY_CODE
		uint16_t messageCmd 	= OUTGOING_CMD_LORA_RECEIVED_PACKAGE;
		uint16_t messageLength = (length+2);
		uint8_t messageData[messageLength];
		memcpy(messageData,data,length);
		//data rssi'ini ekliyoruz
		messageData[length    ] = (uint8_t)(_lastRssi>>8);
		messageData[length+1] = (uint8_t)(_lastRssi&0xFF);
		/*package send */
		SerialControl_SendCommand(messageCmd, (uint8_t*) messageData, messageLength ,THREAD);
		#endif
	}
	if(Radio_GetEchoSettings(BLUETOOTH_DEVICE))
	{
		//kaantodo : not implemented yet
	}
	if(Radio_GetEchoSettings(ETHERNET_CLIENT_DEVICE))
	{
		#ifdef GATEWAY_CODE
		uint16_t messageCmd 	= OUTGOING_CMD_LORA_RECEIVED_PACKAGE;
		uint16_t messageLength = (length+2);
		uint8_t messageData[messageLength];
		memcpy(messageData,data,length);
		//data rssi'ini ekliyoruz
		messageData[length  ] = (uint8_t)(_lastRssi>>8);
		messageData[length+1] = (uint8_t)(_lastRssi&0xFF);
		printf("<EchoReceivedPackage>\n");
		/*package send */
		EthernetControl_Broadcast( messageCmd ,messageData , messageLength );
		//SerialControl_SendCommand(messageCmd,messageData,messageLength ,THREAD);
		#endif
	}
}
int	Radio_CalculateRSSI(int snr,int rssi)
{
	int retval = 0;
	return retval;
}

void	Radio_SetEchoSettings(Radio_EchoDevice echoId,bool enabled)
{
	if(echoId == 0)
	{
		_echoToSerialPort = enabled;
	}
	else if(echoId == 1)
	{
		_echoToBluetooth = enabled;
	}
	else if(echoId == 2)
	{
		_echoToEthernetClient = enabled;
	}
}

bool	Radio_GetEchoSettings(Radio_EchoDevice echoId)
{
	bool retVal;
	if(echoId == 0)
	{
		retVal = _echoToSerialPort;
	}
	else if(echoId == 1)
	{
		retVal = _echoToBluetooth;
	}
	else if(echoId == 2)
	{
		retVal = _echoToEthernetClient;
	}
	else
	{
		retVal = false;
	}
	return retVal;
}



int16_t Radio_GetLastRssi()
{
    return _lastRssi;
}
void Radio_EnableTCXO(Context_Enum context)
{

}

float Radio_GetFrequencyError(Context_Enum context)
{
	return 0;
}

bool Radio_CheckIsChannelActive(Context_Enum context)
{
	//kaanbak : bu fonksiyonu eklemisim ama ne amacli??
	return false;
}


void Radio_SetPromiscuous(bool promiscuous)
{
    _promiscuous = promiscuous;
}


int  	 Radio_GetLastSNR(void)
{
	int retVal = 0x00;
	return retVal;
}

RadioServiceType findServiceTypeFromRoutingHeader(RadioRoutingHeaderType routingHeaderTypeParam  )
{
	RadioServiceType returnValTemp = UNICAST_SERVICE;
	switch( routingHeaderTypeParam )
	{
		case UNICAST_DIRECT_HEADER:
		case UNICAST_ROUTED_HEADER:
			returnValTemp = UNICAST_SERVICE;
			break;
		case MULTICAST_DIRECT_HEADER:
		case MULTICAST_ROUTED_HEADER:
			returnValTemp = MULTICAST_SERVICE;
			break;
		case BROADCAST_HEADER:
			returnValTemp = BROADCAST_SERVICE;
			break;
		default:
			break;
	}
	return returnValTemp;
}

#ifdef GATEWAY_CODE

void 	*sx1272_Poll_Dio_Pin_Task()
{
	int gpioLvl = 0;
	int latchVar = 0;
	int lastState = 0;
	printf("<LoRa> Read DIO_0_PIN Thread Created ! \n");
	while(1)
	{
		bcm2835_delay(2);
		gpioLvl = bcm2835_gpio_lev(SX1272_DIO_0_PIN);
		if(lastState != gpioLvl) // transaction occured
		{
			lastState = gpioLvl;
			if(1 == gpioLvl) // Low-to-High transaction
			{
				//printf("<SX1272 PinReadOut> Low-To-High Transaction \n");
				vRadioInterruptHandlerTask((void*)NULL); /* gateway-code'u ile stm code'unu ortaklamak icin fonksiyon adi degistirilmedi. gateway-code'da task degil duz fonksiyondur.*/

			}
			else // High-to-Low transaction
			{
				//printf("<SX1272 PinReadOut> High-To-Low Transaction \n");
			}
		}
	}
}
void 	*loraEventHandlerTask()
{

}
#endif


#ifdef GATEWAY_CODE
/* Private Functions */

static void txTimeoutHandler(int sig, siginfo_t *si, void *uc)
{
	//CLEAR IRQ REGISTERS kaanbak : radio_reset gibi bisey ekleyelim bunun yerine
	radioManagerState = RADIO_IDLE;
	SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, THREAD); // Clear all IRQ flags
    SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, THREAD); // Clear all IRQ flags

	printf("TX Timeout Handler Function Called %d\n", sig);
	printCurrentTime();
	signal(sig, SIG_IGN);
	stepMessageQueue(TX_TIMEOUT);
	timer_delete(radioTimeoutTimer.timerID); // remove timer
	return;
}

static void responseTimeoutHandler( int sig, siginfo_t *si, void *uc )
{
	radioManagerState = RADIO_IDLE;
	SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, THREAD); // Clear all IRQ flags
    SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, THREAD); // Clear all IRQ flags

	LoRaMessageQueueElement *queueElement;
	int resultOfSendPackageTemp = 0;
	queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex); // queue'da en son kullandigimiz elemani aldik
	
	printf("RX Timeout Handler Function Called  MsgType :%d ",queueElement->mainHeader.messageType );
	printCurrentTime();
	signal(sig, SIG_IGN);
	timer_delete(radioTimeoutTimer.timerID); // remove timer

	if(queueElement->remainingRetryCount == 0)
	{
		handleErrorMessage( false , queueElement->mainHeader.messageType );
		stepMessageQueue( RESPONSE_FAILED );
	}
	else
	{
		stepMessageQueue( RESPONSE_TIMEOUT );
	}
	return;	
}

#endif


void Delay_WithMillis( uint32_t millisParam, Context_Enum contextParam )
{
#ifndef GATEWAY_CODE
	if(contextParam == THREAD)
	{
		vTaskDelay(millisParam);
	}
	else if(contextParam == NON_THREAD)
	{
		HAL_Delay(millisParam);
	}
#else
	delay_millisecond((unsigned long)millisParam);
#endif
}
