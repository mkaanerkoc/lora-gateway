#include "EEPROMModule.h"

/*
size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
              
size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
*/

FILE *eepromFile;

void 		EEPROM_ReadBulk(EEPROM_DeviceNumber_t devNum, uint32_t address,uint8_t *data, uint16_t length,Context_Enum context)
{
	#ifdef GATEWAY_CODE
	 	eepromFile = fopen("/home/pi/lora-gateway/config_inc.ci", "r");
	 	if(eepromFile == NULL )
	 	{
	 		printf("Cannot Open Config file \n");
	 		return;
	 	}
	 	fseek( eepromFile, (long int ) address , SEEK_SET);
	 	fread(	data, length , 1 , eepromFile );
	 	fclose(eepromFile);
	#endif
}

void 		EEPROM_WriteBulk(EEPROM_DeviceNumber_t devNum, uint32_t address,uint8_t *data,uint16_t length, Context_Enum context)
{
	#ifdef GATEWAY_CODE
		eepromFile = fopen("/home/pi/lora-gateway/config_inc.ci", "r+b");
		if(eepromFile == NULL )
	 	{
	 		printf("Cannot Open Config file \n");
	 		return;
	 	}
	 	fseek( eepromFile, (long int ) address , SEEK_SET);
	 	fwrite( data , length , 1 ,  eepromFile);
		fclose(eepromFile);
	#endif
}

uint8_t 	EEPROM_ReadByte(EEPROM_DeviceNumber_t devNum, uint32_t address,Context_Enum context)
{
	#ifdef GATEWAY_CODE
	 	eepromFile = fopen("/home/pi/lora-gateway/config_inc.ci", "r");
	 	if(eepromFile == NULL )
	 	{
	 		printf("Cannot Open Config file \n");
	 		return;
	 	}
	 	uint8_t readByteTemp = 0;
	 	fseek( 	eepromFile, (long int ) address , SEEK_SET);
	 	fread(	&readByteTemp, 1, 1, eepromFile );
		fclose(	eepromFile);
		return readByteTemp;
	#endif
}

bool 		EEPROM_WriteByte(EEPROM_DeviceNumber_t devNum, uint32_t address,uint8_t inData,Context_Enum context)
{
	#ifdef GATEWAY_CODE
		eepromFile = fopen("/home/pi/lora-gateway/config_inc.ci", "r+b");
		if(eepromFile == NULL )
	 	{
	 		printf("Cannot Open Config file \n");
	 		return;
	 	}
	 	fseek( 	eepromFile, (long int ) address , SEEK_SET);
	 	fwrite( &inData , 1 , 1, eepromFile);
		fclose(	eepromFile);
	#endif
}

#ifdef GATEWAY_CODE

void		EEPROM_CreateInitialConfigFile(void)
{
	eepromFile = fopen("/home/pi/lora-gateway/config_inc.ci", "w");
	if(eepromFile == NULL )
 	{
 		printf("Cannot Open Config file \n");
 		return;
 	}
	uint8_t tempValue[2048] = {255};
	fwrite( tempValue , 1 , 2048, eepromFile);
	fclose(eepromFile);
}

#endif