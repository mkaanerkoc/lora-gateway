/*
 * SX1272Driver.h
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#ifndef RADIO_SX1272DRIVER_H_
#define RADIO_SX1272DRIVER_H_

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "SX1272Driver_TypesConstants.h"
#include "RadioManager_TypesConstants.h"
#include "Common_TypeDefs.h"

#ifndef GATEWAY_CODE

#include "stm32l4xx_hal.h"

/*Externs */
extern SPI_HandleTypeDef hspi1; //kaanbak -> burasi degistirilecek ?? neden aq ??

SemaphoreHandle_t SX1272_SPIReadSemaphore;
SemaphoreHandle_t SX1272_SPIWriteSemaphore;

#else

#include <bcm2835.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>


#define SX1272_CS_PIN 			8
#define SX1272_DIO_0_PIN   		RPI_GPIO_P1_11
#define SX1272_DIO_1_PIN   		RPI_GPIO_P1_12
#define SX1272_DIO_2_PIN   		RPI_GPIO_P1_26
#define SX1272_DIO_3_PIN   		RPI_GPIO_P1_13
#define SX1272_NRESET_PIN  		RPI_GPIO_P1_7
#endif



volatile uint8_t    			_bufLen; /// Number of octets in the buffer
uint8_t             			_buf[RH_RF92_MAX_PAYLOAD_LEN]; /// The receiver/transmitter buffer
volatile bool       			_rxBufValid; /// True when there is a valid message in the buffer
bool                			_usingHFport; // True if we are using the HF port (779.0 MHz and above)
int8_t              			_lastSNR; // Last measured SNR, dB

//kaanbak, bunlar struct icine konulabilir...
uint8_t             			_thisAddress; /// This node id
bool               				 _promiscuous;     /// Whether the transport is in promiscuous mode
volatile int16_t     			_lastRssi;     /// The value of the last received RSSI value, in some transport specific units
volatile uint16_t   			_rxBad;     /// Count of the number of bad messages (eg bad checksum etc) received
volatile uint16_t   			_rxGood;    /// Count of the number of successfully transmitted messaged
volatile uint16_t   			_txGood;    /// Count of the number of bad messages (correct checksum etc) received
volatile bool       			_cad;    /// Channel activity detected
unsigned int        			_cad_timeout;    /// Channel activity timeout in ms

RHMode 							_mode;
void 			InitializeSX1272Driver(void);

uint8_t 		SX1272_Reset(void);

void 			SX1272_SetTxPower(int8_t  power, bool useRFO, Context_Enum context);
int8_t  		SX1272_GetTxPower(Context_Enum context);

void 			SX1272_SetFrequency(float frequency,Context_Enum context);
float 			SX1272_GetFrequency(Context_Enum context);

void 			SX1272_SetPreambleLength(uint16_t preLength,Context_Enum context);
uint16_t 		SX1272_GetPreambleLength(Context_Enum context);

bool 			SX1272_SetBandwidth(RadioBW_Enum bw, Context_Enum context);
RadioBW_Enum 	SX1272_GetBandwidth(Context_Enum context);

bool 			SX1272_SetCodingRate(RadioCR_Enum cr, Context_Enum context);
RadioCR_Enum 	SX1272_GetCodingRate(Context_Enum context);

bool 			SX1272_SetSpreadingFactor(RadioSF_Enum sf, Context_Enum context);
RadioSF_Enum 	SX1272_GetSpreadingFactor(Context_Enum context);

void 			SX1272_SetMode(RHMode mode, Context_Enum context);
RHMode 			SX1272_GetMode(Context_Enum context);


void 			SX1272_ClearInterruptRegisters( Context_Enum contextParam );
uint8_t			SX1272_GetInterruptRegisterValue( Context_Enum contextParam );


uint8_t 	 	SX1272_ReadRegister(uint8_t address,Context_Enum context);
void 			SX1272_ReadRegisters(uint8_t address,uint8_t *data,uint16_t length,Context_Enum context);
void 			SX1272_WriteRegister(uint8_t address,uint8_t data,Context_Enum context);
void 			SX1272_WriteRegisters(uint8_t address, uint8_t *data,uint16_t length,Context_Enum context);

#ifdef GATEWAY_CODE

void 			delay_millisecond(unsigned long milliseconds);
//Tasks//
void 			*sx1272_Poll_Dio_Pin_Task();

#endif // end of #ifdef GATEWAY_CODE




#endif /* RADIO_SX1272DRIVER_H_ */
