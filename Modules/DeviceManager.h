/*
 * DeviceManager.h
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#ifndef DEVICEMANAGER_H_
#define DEVICEMANAGER_H_

#include "EEPROMModule.h"
#include "Common_TypeDefs.h"

#ifndef GATEWAY_CODE

Peripheral_Status 	p_RadioStatus;
Peripheral_Status 	p_EepromStatus;
Peripheral_Status 	p_BME280Status;
Peripheral_Status 	p_MAX31865_1_Status;
Peripheral_Status 	p_MAX31865_2_Status;
Peripheral_Status 	p_GPS_Status;
Peripheral_Status 	p_SD_Card_Status;

#endif // end of #ifndef GATEWAY_CODE

uint16_t 			deviceUniqueID;
unitNo_t 			deviceLoRaID;
unitNo_t 			deviceLoRaGatewayID;



void				checkForPeripherals(void);

//TODO EEPROM'da kaydedilen ve runtime'da kullanilabilecek tum global degiskenler burada saklanacak, Initialize fonksiyonunda bu degiskenler
// eeprom'dan okunarak global' degiskenlerine aktarilacak.
// bu degiskenleri kullanmak isteyen diger moduller bu degiskenlere get fonksiyonlariyla ulasarak pointer'larini alacaklar

void 			InitializeDeviceManager(void);

void 			setDeviceUniqueID( uint16_t devID,Context_Enum context);
uint16_t 		getDeviceUniqueID( bool fromEEPROM, Context_Enum context);

void 			setDeviceLoRaID( unitNo_t loraID,Context_Enum context );
unitNo_t 		getDeviceLoRaID( bool fromEEPROM, Context_Enum context );

void 			setDeviceLoRaGatewayID( unitNo_t gatewayID,Context_Enum context);
unitNo_t 		getDeviceLoRaGatewayID(bool fromEEPROM, Context_Enum context );



/* Device Routines */




#endif /* DEVICEMANAGER_H_ */
