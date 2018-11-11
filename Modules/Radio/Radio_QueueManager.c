//author : mkerkoc

#include "Radio_QueueManager.h"

/* privates */
time_t timeStrTemp;
struct tm * timeinfo;


//Functions
bool	Initialize_LoRaMessageQueue(void)
{
	loraMessageQueueHeadIndex = -1;
	loraMessageQueueLastIndex = -1;
	loraMessageCountInQueue = 0;
	for(int i = 0 ; i < LORA_MESSAGE_QUEUE_SIZE ; i++)
	{

	}
	//printf( "Initialize_LoRaMessageQueue\n");
	return true;
}

/* notes : bu fonksiyonda yeni bir lora mesaj istegi queue'ya ekleniyor. fakat yonlendirme bilgisi burada hesaplanmiyor.
			yonlendirme bilgisi radio_send_package' da hesaplanarak gonderiliyor. bunun nedeni ise queue'ya eklendikten sonra
			gonderilme sirasi gelene kadar yonlendirme bilgisinin degismis olabilecegi ihtimali..
*/
bool		addNewMessageToQueue(	unitNo_t 				destinationParam,
									RadioMessageType 		messageTypeEnumParam,
									RadioServiceType 		serviceTypeParam,
									RadioMessagePriority 	priorityParam,
									uint8_t 				*dataPtrParam,
									uint8_t 				dataLengthParam,
									uint16_t 				responsetimeoutInMillisParam,
									uint8_t 				retryCountParam,
									bool 					allowOverwriteParam,
									RadioMessageSourceType  sourceTypeParam
								)
{
	//kaantodo : messageID'yi nasil hesapliycaz? CRC ? incremental ?

	LoRaMessageQueueElement 	*queueElement;
	LoRaMessageMainHeader 		mainHeader;
	LoRaGenericRoutingHeader  	genRoutingHeader;
	time ( &timeStrTemp );
  	timeinfo = localtime ( &timeStrTemp );
  	bool messageHaveAnyResponseTemp = checkThatMessageHaveResponse( messageTypeEnumParam );
	//printf("New Item Add to Queue !! , Queue Element Count : %d , DateTime : %s",loraMessageCountInQueue,asctime (timeinfo) );
	if( loraMessageCountInQueue == 0 ) // queue is empty
	{
		loraMessageQueueHeadIndex = 0;
		loraMessageQueueLastIndex = 0;
		loraMessageCountInQueue++;
		queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
		//fill element
		queueElement->mainHeader.messageType = messageTypeEnumParam;
		queueElement->serviceType = serviceTypeParam;
		queueElement->haveAnyResponse = messageHaveAnyResponseTemp;
		queueElement->destinationUnit = destinationParam;

		memcpy(&(queueElement->messageData),dataPtrParam,dataLengthParam ); // burada patliyoruz herhalde
		//queueElement->messageData = data;
		queueElement->messageLength = dataLengthParam;
		queueElement->retryCount = retryCountParam;
		queueElement->remainingRetryCount = retryCountParam;
		queueElement->timeout = responsetimeoutInMillisParam ;
		queueElement->state = MESSAGE_WAITING_FOR_SEND;
		queueElement->sourceType = sourceTypeParam;
		stepMessageQueue( FIRST_MESSAGE_ADDED ); // burasi suan blocking call degil
		// sorun su ;
		// queue'ya ekledikten sonra
		return true;
	}
	else if(loraMessageCountInQueue == LORA_MESSAGE_QUEUE_SIZE) // queue is full
	{
		if( allowOverwriteParam )
		{
			loraMessageQueueLastIndex = (loraMessageQueueLastIndex + 1 ) % LORA_MESSAGE_QUEUE_SIZE;
			loraMessageQueueHeadIndex = (loraMessageQueueHeadIndex + 1 ) % LORA_MESSAGE_QUEUE_SIZE;
			queueElement = getMessageQueueElementByIndex(loraMessageQueueLastIndex);
			//fill element
			queueElement->mainHeader.messageType = messageTypeEnumParam;
			queueElement->serviceType = serviceTypeParam ;

			queueElement->haveAnyResponse = messageHaveAnyResponseTemp;

			queueElement->destinationUnit = destinationParam;
			memcpy(&(queueElement->messageData),dataPtrParam ,dataLengthParam);
			queueElement->messageLength = dataLengthParam;
			queueElement->retryCount = retryCountParam;
			queueElement->remainingRetryCount = retryCountParam;
			queueElement->timeout = responsetimeoutInMillisParam;
			queueElement->state = MESSAGE_WAITING_FOR_SEND;
			queueElement->sourceType = sourceTypeParam;
			time(&(queueElement->timeStruct));
		}
		else
		{
			return false; // buffer is full and allowOverwrite param is false... so we cant overwrite..
		}
		return true;
	}
	else // buffer is neither empty nor null
	{
		if(loraMessageCountInQueue>5)
		{
			// queue uzerinde 5 den fazla mesaj var. tikanmis olabilir queue trigger event'lar // check for time out
			queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
			if(queueElement!=NULL)
			{
				// NULL gelmesi zaten imkansiz ama kontrol amacli ekledik
				double diffInSecondsTemp = difftime(queueElement->timeStruct,timeStrTemp);
				if(diffInSecondsTemp>10)
				{
					// kaanbak: ee ne yapicaz burada..
				}
			}
		}
		loraMessageQueueLastIndex = (loraMessageQueueLastIndex + 1 ) % LORA_MESSAGE_QUEUE_SIZE;
		loraMessageCountInQueue++;
		queueElement = getMessageQueueElementByIndex(loraMessageQueueLastIndex);
		//fill element
		queueElement->mainHeader.messageType = messageTypeEnumParam;
		queueElement->serviceType = serviceTypeParam;

		queueElement->haveAnyResponse = messageHaveAnyResponseTemp;

		queueElement->destinationUnit = destinationParam ;
		memcpy(&(queueElement->messageData),dataPtrParam,dataLengthParam);

		queueElement->messageLength 		= dataLengthParam;
		queueElement->retryCount 			= retryCountParam;
		queueElement->remainingRetryCount 	= retryCountParam;
		queueElement->timeout = responsetimeoutInMillisParam;
		queueElement->sourceType = sourceTypeParam;
		queueElement->state = MESSAGE_WAITING_FOR_SEND;

		return true;
	}
}

bool	clearAllMessageQueue(void)
{
	for(int i = 0 ; i < LORA_MESSAGE_QUEUE_SIZE ; i++)
	{

	}
	return true;
}

bool	clearMessageFromQueueByMessageID(uint16_t messageID)
{

	for(int i = 0 ; i < LORA_MESSAGE_QUEUE_SIZE ; i++)
	{

	}
	return true;
}

bool	 clearMessageFromQueueByIndex(uint8_t index)
{
	if(index > LORA_MESSAGE_QUEUE_SIZE)
	{
		return false;
	}
	LoRaMessageQueueElement 	*queueElement = getMessageQueueElementByIndex(index);
	memset(queueElement,0,sizeof(LoRaMessageQueueElement));
	queueElement->state = MESSAGE_EMPTY;
	return true;
}
/* @brief : queue icerisinde head objesinin indexini bir arttirarak queue' yi kaydirir. Ayni zamanda count'i bir azaltir.
 *
 *
 */
bool removeHeadElementInQueue(void)
{
	if(loraMessageCountInQueue == 0)
	{
		return false; // queue zaten bos.. nesini remove edicez.. ayrica queue bosken bu fonksiyon nasil cagirilmis olabilir ??
	}
	if(clearMessageFromQueueByIndex(loraMessageQueueHeadIndex))
	{
		loraMessageQueueHeadIndex = (loraMessageQueueHeadIndex + 1)  % LORA_MESSAGE_QUEUE_SIZE; // move head index
		loraMessageCountInQueue--;
		if(loraMessageCountInQueue == 0) // buffer cleared, return initial state.
		{
			loraMessageQueueHeadIndex = -1;
			loraMessageQueueLastIndex = -1;
			//printf("MessageQueue Cleared \n");
		}
		return true;
	}
	else
	{
		return false; // headIndex'i nasil olmussa bir sekilde LORA_MESSAGE_QUEUE_SIZE'dan fazla olarak verilmis..
	}

}

//stepMessageQueue fonksiyonun cagiralacagi 2 yer var;
// ilk mesaj eklendigi yerde ve loraHandleTask icerisinde... delay gereken yerler Task icersiinde cagirilacagi icin sorun yok gibi..
bool stepMessageQueue(MessageQueueStepReason reason)
{
	int resultOfSendPackageTemp = 0;
	LoRaMessageQueueElement *queueElement;
	
	if(loraMessageCountInQueue == 0)
	{
		return false; // queue is empty...
	}

	switch(reason)
	{
		case FIRST_MESSAGE_ADDED:
			//printf("<QueueManager> stepMessageQueue triggered by FIRST_MESSAGE_ADDED event ! \n");
			queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
			if(queueElement != NULL)
			{
				//PARAMETRE CHECK BURAYA EKLENCEK

				resultOfSendPackageTemp = Radio_Send_Package(	queueElement->destinationUnit,
																queueElement->mainHeader.messageType,
																queueElement->serviceType,
																queueElement->messageData,
																queueElement->messageLength,
																queueElement->timeout,
																THREAD );
				if(resultOfSendPackageTemp != 0)
				{
					// paket gonderiminde sorun oldu.. parametreleri felan begenmedi herhalde.. step atlatmamiz lazim..
					stepMessageQueue(SEND_FAILED); // dangereous move ? 
				}
				else
				{
					return true;
				}
			}
			//Note : !! bingo aq.. ilk mesaj eklendiginde sadece gonderme islemi yapiyoruz.. buffer'dan temizleme islemlerini TX_DONE ,RX_DONE v.s. gibi
			// islem bitecegi yerlerde yapiyoruzz..
			break;
		case TX_DONE:
			//printf("<QueueManager> stepMessageQueue triggered by TX_DONE event ! \n");
			//bence bu islemler baska bi yerde yapilmali.. process 'de felan
			queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
			if( queueElement != NULL )
			{
				if( !(queueElement->haveAnyResponse) )
				{
					if( removeHeadElementInQueue()) // yanit beklemedigimiz bir mesaji cikarabiliriz queue'dan
					{
						//basari ile elemani remove ettik... simdi yeni message'i gonderebiliriz
						if(loraMessageCountInQueue == 0)
						{
							return true; // queue'nun son elemanini gondermisiz... queue is empty...
						}
						queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
						if(queueElement != NULL)
						{
							//PARAMETRE CHECK BURAYA EKLENCEK
							resultOfSendPackageTemp = Radio_Send_Package(	queueElement->destinationUnit,
									queueElement->mainHeader.messageType,
									queueElement->serviceType,
									queueElement->messageData,
									queueElement->messageLength,
									queueElement->timeout,
									THREAD );
							if(resultOfSendPackageTemp != 0)
							{
								stepMessageQueue(SEND_FAILED); // dangereous move ? 
							}
							else
							{
								return true;
							}
						}
						return false;
					} // if( removeHeadElementInQueue())
					else
					{
						return false;// basari ile remove edememisiz, ya queue bos yada headIndex sacma sapan birsey
					}
				} // if(queueElement->mainHeader.reliableness == UNRELIABLE_MESSAGE)
				else
				{
					return false; // gonderdigimiz mesaj'in response'unu bekliyor isek, queue'yi ilerletemeyiz..
				}
			} // if queueElement != NULL)
			else
			{
				return false;
			}
		case RX_DONE:
			//printf("<QueueManager> stepMessageQueue triggered by RX_DONE event ! \n"); bunun kontrolu nerde yapiliyor?
			break;
		case RESPONSE_TIMEOUT:
			queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
			if( queueElement != NULL )
			{
				queueElement->remainingRetryCount = queueElement->remainingRetryCount - 1;
				resultOfSendPackageTemp = Radio_Send_Package(	queueElement->destinationUnit,
												queueElement->mainHeader.messageType,
												queueElement->serviceType,
												queueElement->messageData,
												queueElement->messageLength,
												queueElement->timeout,
												THREAD );
			}
			else
			{
				return false;
			}
			break;
		case RESPONSE_FAILED:
		case RESPONSE_RECEIVED:
			queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
			if(queueElement != NULL)
			{
				if( removeHeadElementInQueue()) // En Son ki mesaji yok ediyoruz..
				{
					//basari ile elemani remove ettik... simdi yeni message'i gonderebiliriz
					if(loraMessageCountInQueue == 0)
					{
						return false; // queue'nun son elemanini gondermisiz... queue is empty... buraya zaten girmez de kontrol amacli..
					}
					queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex); // queue'nun en ustundeki elemani cikardiktan sonraki en ustteki yeni eleman
					if(queueElement != NULL)
					{
						//PARAMETRE CHECK BURAYA EKLENCEK
						resultOfSendPackageTemp = Radio_Send_Package(	
								queueElement->destinationUnit,
								queueElement->mainHeader.messageType,
								queueElement->serviceType,
								queueElement->messageData,
								queueElement->messageLength,
								queueElement->timeout,
								THREAD );
						if(resultOfSendPackageTemp != 0)
						{
							stepMessageQueue(SEND_FAILED); // dangereous move ?
						}
						else
						{
							return true;
						}
					}
					return false;
				}
				else
				{
					return false;// basari ile remove edememisiz, ya queue bos yada headIndex sacma sapan birsey
				}
			}
			else
			{
				return false;
			}
			break;
		case TX_TIMEOUT:
			queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
			if(queueElement != NULL)
			{
				if( removeHeadElementInQueue()) // En Son ki mesaji yok ediyoruz..
				{
					//basari ile elemani remove ettik... simdi yeni message'i gonderebiliriz
					if(loraMessageCountInQueue == 0)
					{
						return false; // queue'nun son elemanini gondermisiz... queue is empty...
					}
					queueElement = getMessageQueueElementByIndex(loraMessageQueueHeadIndex);
					if(queueElement != NULL)
					{
						resultOfSendPackageTemp = Radio_Send_Package(	
								queueElement->destinationUnit,
								queueElement->mainHeader.messageType,
								queueElement->serviceType,
								queueElement->messageData,
								queueElement->messageLength,
								queueElement->timeout,
								THREAD);
						if(resultOfSendPackageTemp != 0)
						{
							stepMessageQueue(SEND_FAILED); // dangereous move ? 
						}
						else
						{
							return true;
						}
					}
					return false;
				}
				else
				{
					return false;// basari ile remove edememisiz, ya queue bos yada headIndex sacma sapan birsey
				}
			}
			else
			{
				return false;
			}
			break;
		default:
			return false;
	} // end of switch( reason )
	return false;
}

LoRaMessageQueueElement* 	getMessageQueueElementByMessageID(uint16_t messageID)
{
	for(int i = 0 ; i < LORA_MESSAGE_QUEUE_SIZE ; i++)
	{
		if(messageID == loraMessageQueue[i].messageID)
		{
			return &(loraMessageQueue[i]);
		}
	}
}
LoRaMessageQueueElement* 	getMessageQueueElementByIndex(uint8_t index)
{
	if(index>=LORA_MESSAGE_QUEUE_SIZE) // sacma sapan indexler bunlar aq...
	{
		return NULL;
	}
	if(loraMessageCountInQueue == 0 ) // queue bos
	{
		return NULL;
	}
	return &(loraMessageQueue[index]);
}
