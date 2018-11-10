/*
 * SX1272Driver.c
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#include "SX1272Driver.h"

//kaanbak, reset pini gpio port , pin degiskenkeri eklenecek


void 			InitializeSX1272Driver(void)
{
#ifndef GATEWAY_CODE
	SX1272_SPIReadSemaphore 	= xSemaphoreCreateBinary();
	SX1272_SPIWriteSemaphore 	= xSemaphoreCreateBinary();
#else
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
  	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	printf("Initialization Of SX1272 Driver\n");
  	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
	bcm2835_spi_begin();
	bcm2835_gpio_fsel(SX1272_CS_PIN, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(SX1272_CS_PIN,1);
#endif
}

//kaanbak: doldurulacak
uint8_t 	SX1272_Reset(void)
{
	return 0;
}

/* Application Layer */

void 			SX1272_SetTxPower(int8_t  power, bool useRFO, Context_Enum context)
{
	// Sigh, different behaviours depending on whther the module use PA_BOOST or the RFO pin
	// for the transmitter output
	if (useRFO)
	{
		if (power > 14)
		{
			power = 14;
		}
		if (power < -1)
		{
			power = -1;
		}
		SX1272_WriteRegister(RH_RF92_REG_09_PA_CONFIG, (RH_RF92_MAX_POWER | (power + 1)), context);
	}
	else
	{
		if (power > 23)
		{
			 power = 23;
		}
		if (power < 5)
		{
			power = 5;
		}
		// For RH_RF95_PA_DAC_ENABLE, manual says '+20dBm on PA_BOOST when OutputPower=0xf'
		// RH_RF95_PA_DAC_ENABLE actually adds about 3dBm to all power levels. We will us it
		// for 21, 22 and 23dBm
		if (power > 20)
		{
			SX1272_WriteRegister(RH_RF92_REG_4D_PA_DAC, RH_RF92_PA_DAC_ENABLE,context);
			power -= 3;
		}
		else
		{
			SX1272_WriteRegister(RH_RF92_REG_4D_PA_DAC, RH_RF92_PA_DAC_DISABLE,context);
		}

	// RFM95/96/97/98 does not have RFO pins connected to anything. Only PA_BOOST
	// pin is connected, so must use PA_BOOST
	// Pout = 2 + OutputPower.
	// The documentation is pretty confusing on this topic: PaSelect says the max power is 20dBm,
	// but OutputPower claims it would be 17dBm.
	// My measurements show 20dBm is correct
		SX1272_WriteRegister(RH_RF92_REG_09_PA_CONFIG, (RH_RF92_PA_SELECT | (power-5)),context);
	}
}

int8_t  		SX1272_GetTxPower(Context_Enum context)
{
	//KAANTODO READ FROM REGISTERS
	int8_t retVal = 0;
	return retVal;
}

void 			SX1272_SetFrequency(float frequency,Context_Enum context)
{
	// Frf = FRF / FSTEP
	uint32_t frf = (frequency * 1000000.0) / RH_RF92_FSTEP;
	SX1272_WriteRegister(RH_RF92_REG_06_FRF_MSB, (frf >> 16) & 0xff, context);
	SX1272_WriteRegister(RH_RF92_REG_07_FRF_MID, (frf >> 8) & 0xff, context);
	SX1272_WriteRegister(RH_RF92_REG_08_FRF_LSB, frf & 0xff , context);
	_usingHFport = (frequency >= 779.0);
}

float 			SX1272_GetFrequency(Context_Enum context)
{
	//KAANTODO: READ FROM REGISTERS
	float retVal = 0;
	return retVal;
}

void 			SX1272_SetPreambleLength(uint16_t preLength,Context_Enum context)
{
	SX1272_WriteRegister(RH_RF92_REG_20_PREAMBLE_MSB, preLength >> 8,context);
	SX1272_WriteRegister(RH_RF92_REG_21_PREAMBLE_LSB, preLength & 0xff,context);
}

uint16_t 		SX1272_GetPreambleLength(Context_Enum context)
{
	uint16_t retVal;
	uint8_t msbVal,lsbVal;
	msbVal = SX1272_ReadRegister(RH_RF92_REG_20_PREAMBLE_MSB,context);
	lsbVal   = SX1272_ReadRegister(RH_RF92_REG_21_PREAMBLE_LSB,context);
	retVal = (msbVal<<8)|(lsbVal);
	//kaanbak - fill here
	return retVal;
}

bool 			SX1272_SetBandwidth(RadioBW_Enum bw, Context_Enum context)
{
	uint8_t currentRegisterValue = SX1272_ReadRegister(RH_RF92_REG_1D_MODEM_CONFIG1,context);
	currentRegisterValue = currentRegisterValue & 0x3F; /* clear bits */
	switch(bw)
	{
		case BW_125Khz:
			break;
		case BW_250Khz:
			currentRegisterValue = currentRegisterValue | ( 0x01 << 6);
			break;
		case BW_500Khz:
			currentRegisterValue = currentRegisterValue | ( 0x02 << 6);
			break;
		default:
			return false;
	}
	SX1272_WriteRegister(RH_RF92_REG_1D_MODEM_CONFIG1, currentRegisterValue, context);
	return true;
}

RadioBW_Enum 	SX1272_GetBandwidth(Context_Enum context)
{
	uint8_t currentRegisterValue = SX1272_ReadRegister(RH_RF92_REG_1D_MODEM_CONFIG1,context);
	currentRegisterValue = currentRegisterValue & 0xC0; /* clear other bits */
	currentRegisterValue = currentRegisterValue >> 6;
	//kaanbak hangi formatta donus yapilacak, typedef'lerde ki degerleri register bit degerlerine gore yapabilir miyiz ?
	return currentRegisterValue;
}

bool 			SX1272_SetCodingRate(RadioCR_Enum cr, Context_Enum context)
{
	uint8_t currentRegisterValue = SX1272_ReadRegister(RH_RF92_REG_1D_MODEM_CONFIG1,context);
	currentRegisterValue = currentRegisterValue & 0xC7; /* clear bits */
	switch(cr)
	{
		case CR_4_5:
			currentRegisterValue = currentRegisterValue | ( 0x01 << 3 );
			break;
		case CR_4_6:
			currentRegisterValue = currentRegisterValue | ( 0x02 << 3 );
			break;
		case CR_4_7:
			currentRegisterValue = currentRegisterValue | ( 0x03 << 3 );
			break;
		case CR_4_8:
			currentRegisterValue = currentRegisterValue | ( 0x04 << 3 );
			break;
		default:
			break;
	}
	SX1272_WriteRegister(RH_RF92_REG_1D_MODEM_CONFIG1, currentRegisterValue, context);
	return true;
}

RadioCR_Enum   	SX1272_GetCodingRate(Context_Enum context)
{
	uint8_t currentRegisterValue = SX1272_ReadRegister(RH_RF92_REG_1D_MODEM_CONFIG1,context);
	currentRegisterValue = currentRegisterValue & 0x38; /* clear bits */
	currentRegisterValue = currentRegisterValue >> 3;
	return currentRegisterValue;
}

bool 			SX1272_SetSpreadingFactor(RadioSF_Enum sf, Context_Enum context)
{
	uint8_t currentRegisterValue = SX1272_ReadRegister(RH_RF92_REG_1E_MODEM_CONFIG2,context);
	currentRegisterValue = currentRegisterValue & 0x0F; /* clear  bits */
	switch(sf)
	{
		case SF_64C_Symbol:
			currentRegisterValue = currentRegisterValue | (0x06<<4);
			break;
		case SF_128C_Symbol:
			currentRegisterValue = currentRegisterValue | (0x07<<4);
			break;
		case SF_256C_Symbol:
			currentRegisterValue = currentRegisterValue | (0x08<<4);
			break;
		case SF_512C_Symbol:
			currentRegisterValue = currentRegisterValue | (0x09<<4);
			break;
		case SF_1024C_Symbol:
			currentRegisterValue = currentRegisterValue | (0x0A<<4);
			break;
		case SF_2048C_Symbol:
			currentRegisterValue = currentRegisterValue | (0x0B<<4);
			break;
		case SF_4096C_Symbol:
			currentRegisterValue = currentRegisterValue | (0x0C<<4);
			break;
		default:
			return false;
	}
	SX1272_WriteRegister(RH_RF92_REG_1E_MODEM_CONFIG2,currentRegisterValue,context);
	return true;
}

RadioSF_Enum 	SX1272_GetSpreadingFactor(Context_Enum context)
{
	uint8_t currentRegisterValue = SX1272_ReadRegister(RH_RF92_REG_1E_MODEM_CONFIG2,context);
	currentRegisterValue = currentRegisterValue & 0xF0; /* clear other bits */
	currentRegisterValue = currentRegisterValue >> 4;
	return currentRegisterValue;
}

void 			SX1272_SetMode(RHMode mode, Context_Enum context)
{
	if(mode == RHModeCad)
	{

	}
	else 	if(mode == RHModeIdle)
	{
		if(_mode != RHModeIdle)
		{
			SX1272_WriteRegister(RH_RF92_REG_01_OP_MODE, RH_RF92_MODE_STDBY,context);
			_mode = RHModeIdle;
		}
	}
	else 	if(mode == RHModeInitialising)
	{

	}
	else 	if(mode == RHModeRx)
	{
		 if (_mode != RHModeRx)
		{
			 SX1272_WriteRegister(RH_RF92_REG_01_OP_MODE, RH_RF92_MODE_RXCONTINUOUS,context);
			 SX1272_WriteRegister(RH_RF92_REG_40_DIO_MAPPING1, 0x00, context); // Interrupt on RxDone
			 _mode = RHModeRx;
		}
	}
	else 	if(mode == RHModeSleep)
	{
		 if (_mode != RHModeSleep)
		{
			 SX1272_WriteRegister(RH_RF92_REG_01_OP_MODE, RH_RF92_MODE_SLEEP,context);
			 _mode = RHModeSleep;
		}
	}
	else 	if(mode == RHModeTx)
	{
		 if (_mode != RHModeTx)
		{
			 SX1272_WriteRegister(RH_RF92_REG_01_OP_MODE, RH_RF92_MODE_TX, context);
			 SX1272_WriteRegister(RH_RF92_REG_40_DIO_MAPPING1, 0x40,context); // Interrupt on TxDone
			 _mode = RHModeTx;
		}
	}
	else
	{
		// Error !
	}
}

RHMode 			SX1272_GetMode(Context_Enum context)
{
	//KAANTODO, REGISTERDAN OKUNCAK FLN
	return _mode;
}

void 			SX1272_ClearInterruptRegisters( Context_Enum contextParam  )
{
	SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, contextParam); // Clear all IRQ flags
	SX1272_WriteRegister(RH_RF92_REG_12_IRQ_FLAGS, 0xff, contextParam); // Clear all IRQ flags
}

uint8_t			SX1272_GetInterruptRegisterValue( Context_Enum contextParam  )
{
	uint8_t returnValueTemp = 0x00;
	returnValueTemp = SX1272_ReadRegister(RH_RF92_REG_12_IRQ_FLAGS, THREAD);
	return returnValueTemp;
}



uint8_t 		SX1272_ReadRegister(uint8_t address, Context_Enum context)
{
	uint8_t readByte;
	uint8_t dummyByte = 0;
	uint8_t registerAddres = address & ~SPI_WRITE_MASK; //read mask
#ifndef GATEWAY_CODE
	if(context == THREAD)
	{
		HAL_GPIO_WritePin( SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		//kaanbak - fill here
		HAL_SPI_Transmit_IT( &hspi1, &registerAddres, 1 );
		xSemaphoreTake( SX1272_SPIWriteSemaphore, 1000 );
		HAL_SPI_TransmitReceive_IT(&hspi1,&dummyByte,&readByte,1);
		xSemaphoreTake(SX1272_SPIReadSemaphore,1000);
		HAL_GPIO_WritePin( SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_SET);
		return readByte;
	}
	else if(context == NON_THREAD)
	{
		HAL_GPIO_WritePin( SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		if (HAL_SPI_Transmit(&hspi1, &registerAddres, 1, 1000) == HAL_OK)
		{
			HAL_SPI_TransmitReceive(&hspi1, &dummyByte , &readByte, 1, 1000);

		}
		HAL_GPIO_WritePin( SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_SET);
		return readByte;
	}
#else // gateway code
	bcm2835_gpio_write(SX1272_CS_PIN,0);

	if(context == THREAD)
	{
			bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
			bcm2835_spi_transfer(registerAddres);
			readByte = bcm2835_spi_transfer(0);
	}
	else if(context == NON_THREAD)
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
		bcm2835_spi_transfer(registerAddres);
		readByte = bcm2835_spi_transfer(0);
	}
	bcm2835_gpio_write(SX1272_CS_PIN,1);
	return readByte;
#endif
}
void 			SX1272_ReadRegisters(uint8_t address,uint8_t *data,uint16_t length,Context_Enum context)
{
	uint8_t registerAddres = address & ~SPI_WRITE_MASK; //read mask
	uint8_t dummyRead = 0;
	uint16_t i=0;
#ifndef GATEWAY_CODE

	if(context == THREAD)
	{
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		//kaanbak- fill here
		//while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		HAL_SPI_Transmit_IT(&hspi1,&registerAddres,1);
		xSemaphoreTake(SX1272_SPIWriteSemaphore,1000);
		while(length--) //kaanbak , burada toplu bi okuma yapilamaz mi ??
		{
			HAL_SPI_TransmitReceive_IT(&hspi1,&dummyRead,data++,1);
			xSemaphoreTake(SX1272_SPIReadSemaphore,1000);
		}
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
	}
	else if(context == NON_THREAD)
	{
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		if (HAL_SPI_Transmit(&hspi1, &registerAddres, 1, 1000) == HAL_OK)
		{
			while (length--)
			{
					HAL_SPI_TransmitReceive(&hspi1, &dummyRead, data++, 1, 1000);
			}
		}
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
	}

#else // GATEWAY_CODE
	bcm2835_gpio_write(SX1272_CS_PIN,0);
	if(context == THREAD)
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
		bcm2835_spi_transfer(registerAddres);
		while(length--)
		{
			data[i]=bcm2835_spi_transfer(registerAddres);
			i++;
		}
	}
	else if(context == NON_THREAD)
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
		bcm2835_spi_transfer(registerAddres);
		while(length--)
		{
			data[i]=bcm2835_spi_transfer(registerAddres);
			i++;
		}
	}
	bcm2835_gpio_write(SX1272_CS_PIN,1);
#endif

	return;
}
void 			SX1272_WriteRegister(uint8_t address,uint8_t data,Context_Enum context)
{
	uint8_t registerAddres = address | SPI_WRITE_MASK;
	uint8_t registerData = data;

#ifndef GATEWAY_CODE

	if(context == THREAD)
	{
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit_IT(&hspi1,&registerAddres,1);
		xSemaphoreTake(SX1272_SPIWriteSemaphore,100);
		HAL_SPI_Transmit_IT(&hspi1,&registerData,1);
		xSemaphoreTake(SX1272_SPIWriteSemaphore,100);
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_SET);
	}
	else if(context == NON_THREAD)
	{
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		if (HAL_SPI_Transmit(&hspi1, &registerAddres, 1, 1000) == HAL_OK)
		{
			HAL_SPI_Transmit(&hspi1, &registerData, 1, 1000);
		}
		HAL_GPIO_WritePin( SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_SET);
	}

#else

	bcm2835_gpio_write(SX1272_CS_PIN,0);
	if(context == THREAD)
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
		bcm2835_spi_transfer(registerAddres);
		bcm2835_spi_transfer(data);
	}
	else if(context == NON_THREAD)
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
		bcm2835_spi_transfer(registerAddres);
		bcm2835_spi_transfer(data);
	}
	bcm2835_gpio_write(SX1272_CS_PIN,1);

#endif

}
void 			SX1272_WriteRegisters(uint8_t address, uint8_t *data,uint16_t length,Context_Enum context)
{
	bool r = false;
	uint8_t registerAddres = address | SPI_WRITE_MASK;
#ifndef GATEWAY_CODE

	if(context == THREAD)
	{
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		//kaanbak - fill here
		HAL_SPI_Transmit_IT(&hspi1, &registerAddres, 1);
		xSemaphoreTake(SX1272_SPIWriteSemaphore,100);
		while(length--)
		{
			HAL_SPI_Transmit_IT(&hspi1,data++,1);
			xSemaphoreTake(SX1272_SPIWriteSemaphore,1000);
		}
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_SET);
	}
	else if(context == NON_THREAD)
	{
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_RESET);
		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);

		if (HAL_SPI_Transmit(&hspi1, &registerAddres, 1, 1000) == HAL_OK)
		{
			while(length--)
			{
				HAL_SPI_Transmit(&hspi1, data++, 1, 1000);
			}
			r = true;
		}
		HAL_GPIO_WritePin(SX1272_CS_GPIO_Port, SX1272_CS_Pin, GPIO_PIN_SET);
	}

#else // GATEWAY_CODE

	uint16_t i=0;
	bcm2835_gpio_write(SX1272_CS_PIN,0);
	if(context == THREAD)
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
		bcm2835_spi_transfer(registerAddres);
		while(length--)
		{
			bcm2835_spi_transfer(data[i]);
			i++;
		}
	}
	else if(context == NON_THREAD)
	{
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
		bcm2835_spi_transfer(registerAddres);
		while(length--)
		{
			bcm2835_spi_transfer(data[i]);
			i++;
		}
	}
	bcm2835_gpio_write(SX1272_CS_PIN,1);

#endif

}

#ifdef GATEWAY_CODE
void delay_millisecond(unsigned long milliseconds)
{


  //Implement Delay function
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  nanosleep(&ts, NULL);

}
#endif

