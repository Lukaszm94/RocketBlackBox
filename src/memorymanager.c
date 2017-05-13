/*
 * memorymanager.c
 *
 *  Created on: 20 mar 2017
 *      Author: Luke
 */

#include "memorymanager.h"
#include "memory.h"
#include "config.h"
#include "uart.h"
#include "millis.h"

#define MM_DATA_COUNTER_ADDRESS 0x00000000
#define MM_DATA_COUNTER_CRC_ADDRESS 0x00000004
#define MM_MEASUREMENT_STARTED_FLAG_ADDRESS 0x00000008
#define MM_MAX_ADDRESS 0x007FFFFF

#define MM_DATA_START_ADDRESS_OFFSET 0x00001000

uint32_t mm_read_data_counter(uint8_t* okFlag)
{
	uint32_t dataCounter = 0;
	uint32_t crc = 0;
	uint8_t* dataCounterPtr = (uint8_t*) &dataCounter;
	uint8_t* crcPtr = (uint8_t*) &crc;
	memory_read(MM_DATA_COUNTER_ADDRESS, dataCounterPtr, 4);
	memory_read(MM_DATA_COUNTER_CRC_ADDRESS, crcPtr, 4);

	uint32_t readDataCounterCrc = ~dataCounter; // ghetto CRC
	if((readDataCounterCrc != crc) || (dataCounter < MM_DATA_START_ADDRESS_OFFSET)) {
		#if DEBUG
		UART_sendString("Reading data counter, CRC or counter value incorrect: ");
		UART_sendInt(dataCounter);
		UART_sendString("\r\n");
		#endif
		if(okFlag != 0) {
			*okFlag = 0;
		}
		return 0;
	}
	if(okFlag != 0) {
		*okFlag = 1;
	}
	return dataCounter - MM_DATA_START_ADDRESS_OFFSET;
}

uint8_t mm_read_measurement_started_flag()
{
	uint8_t measurementStartedFlag = 0;
	memory_read(MM_MEASUREMENT_STARTED_FLAG_ADDRESS, &measurementStartedFlag, 1);
	return measurementStartedFlag;
}

void mm_read_data(uint32_t address, TemperatureMeasurement buffer[], uint8_t bufferLength)
{
	address = address + MM_DATA_START_ADDRESS_OFFSET;
	uint32_t bytesToRead = sizeof(TemperatureMeasurement) * bufferLength;
	uint8_t* bufferPointer = (uint8_t*) &buffer[0];
	memory_read(address, bufferPointer, bytesToRead);
}

void mm_save_data_info(uint32_t newCounter, uint8_t newFlag)
{
	memory_sector_erase(MM_DATA_COUNTER_ADDRESS); //erase 4kB sector
	while(memory_get_busy_flag());
	delay(2);
	newCounter += MM_DATA_START_ADDRESS_OFFSET;
	uint32_t crc = ~newCounter;
	//write counter, its crc and flag
	memory_write(MM_DATA_COUNTER_ADDRESS, (uint8_t*) &newCounter, 4);
	delay(2);
	memory_write(MM_DATA_COUNTER_CRC_ADDRESS, (uint8_t*) &crc, 4);
	delay(2);
	memory_write(MM_MEASUREMENT_STARTED_FLAG_ADDRESS, &newFlag, 1);
	delay(2);
}

//IMPORTANT: sizeof(TemperatureMeasurement) * length should not exceed 256 bytes, start address should be a multiple of 256 (bits address[7:0] are zero)
void mm_save_data(uint32_t address, TemperatureMeasurement buffer[], uint8_t bufferLength)
{
	address = address + MM_DATA_START_ADDRESS_OFFSET;
	uint16_t bytesToWrite = sizeof(TemperatureMeasurement) * bufferLength;
	if(bytesToWrite > 256) {
		bytesToWrite = 256 / sizeof(TemperatureMeasurement);
	}
	uint8_t* bufferPointer = (uint8_t*) &(buffer[0]);
	memory_write(address, bufferPointer, bytesToWrite);
}

void mm_reset_memory()
{
	memory_chip_erase();
	delay(1);
	mm_save_data_info(0, 0);
}

