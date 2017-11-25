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
#define MM_MAX_ADDRESS 0x007FFFFF

#define MM_DATA_START_ADDRESS_OFFSET 0x00001000
#define MM_AUTOSAVE_INTERVAL_MSEC 5000

uint32_t current_data_head_address = MM_DATA_START_ADDRESS_OFFSET;
uint32_t last_head_address_save_timestamp_msec = 0;

void mm_init()
{
	uint32_t head_address = 0;
	uint32_t crc = 0;
	uint8_t* data_counter_ptr = (uint8_t*) &head_address;
	uint8_t* crc_ptr = (uint8_t*) &crc;
	memory_read(MM_DATA_COUNTER_ADDRESS, data_counter_ptr, 4);
	memory_read(MM_DATA_COUNTER_CRC_ADDRESS, crc_ptr, 4);

	uint32_t readDataCounterCrc = ~head_address; // ghetto CRC
	if((readDataCounterCrc != crc) || (head_address < MM_DATA_START_ADDRESS_OFFSET)) {
		#if DEBUG
		UART_sendString("Reading data counter, CRC or counter value incorrect: ");
		UART_sendInt(head_address);
		UART_sendString("\r\n");
		#endif
		head_address = MM_MAX_ADDRESS / 2; // wild guess, maybe we won't erase any data
	}
	if((head_address % 256) != 0) {
#if DEBUG
		UART_sendString("Round head_address to 256 multiple");
#endif
		head_address = head_address - (head_address % 256);
	}

	current_data_head_address = head_address;
	last_head_address_save_timestamp_msec = millis();
}

void mm_update_autosave()
{
	uint32_t dt = millis() - last_head_address_save_timestamp_msec;
	if(dt > MM_AUTOSAVE_INTERVAL_MSEC) {
		mm_autosave();
	}
}

void mm_append_page(uint8_t* buffer, uint16_t buffer_length)
{
	if(buffer_length > 256) {
		buffer_length = 256;
	}
	memory_write(current_data_head_address, buffer, buffer_length);
	current_data_head_address += 256; // move to next page
}

void mm_autosave()
{
	memory_sector_erase(MM_DATA_COUNTER_ADDRESS); //erase 4kB sector
	while(memory_get_busy_flag());
	delay(2);
	uint32_t crc = ~current_data_head_address;
	//write counter and its crc
	memory_write(MM_DATA_COUNTER_ADDRESS, (uint8_t*) &current_data_head_address, 4);
	delay(2);
	memory_write(MM_DATA_COUNTER_CRC_ADDRESS, (uint8_t*) &crc, 4);
	delay(2);
	last_head_address_save_timestamp_msec = millis();
}

/*void mm_read_data(uint32_t address, TemperatureMeasurement buffer[], uint8_t bufferLength)
{
	address = address + MM_DATA_START_ADDRESS_OFFSET;
	uint32_t bytesToRead = sizeof(TemperatureMeasurement) * bufferLength;
	uint8_t* bufferPointer = (uint8_t*) &buffer[0];
	memory_read(address, bufferPointer, bytesToRead);
}*/



//IMPORTANT: sizeof(TemperatureMeasurement) * length should not exceed 256 bytes, start address should be a multiple of 256 (bits address[7:0] are zero)
/*void mm_save_data(uint32_t address, TemperatureMeasurement buffer[], uint8_t bufferLength)
{
	address = address + MM_DATA_START_ADDRESS_OFFSET;
	uint16_t bytesToWrite = sizeof(TemperatureMeasurement) * bufferLength;
	if(bytesToWrite > 256) {
		bytesToWrite = 256 / sizeof(TemperatureMeasurement);
	}
	uint8_t* bufferPointer = (uint8_t*) &(buffer[0]);
	memory_write(address, bufferPointer, bytesToWrite);
}*/

void mm_reset_memory()
{
	memory_chip_erase();
	delay(1);
	current_data_head_address = MM_DATA_START_ADDRESS_OFFSET;
	mm_autosave();
}

void mm_dump_memory()
{
	if(current_data_head_address < MM_DATA_START_ADDRESS_OFFSET) { //error
#if DEBUG
		UART_sendString("Cannot dump memory, head_addres < offset\n");
#endif
		return;
	}
	if(current_data_head_address > MM_MAX_ADDRESS) { //error
#if DEBUG
		UART_sendString("Cannot dump memory, head_addres > MM_MAX_ADDRESS\n");
#endif
		return;
	}
	uint32_t bytes_to_read = current_data_head_address - MM_DATA_START_ADDRESS_OFFSET;
	uint32_t current_address = MM_DATA_START_ADDRESS_OFFSET;
	uint32_t sectors = bytes_to_read / 256;
	if(sectors == 0) {
		sectors = 1;
	}
	uint32_t i = 0;
	char buffer[256];
	for(i = 0; i < sectors; i++) {
		memory_read(current_address, (uint8_t*)buffer, 256);
		current_address += 256;
		uint16_t j = 0;
		for(j = 0; j < 255; j++) {
			UART_sendChar(buffer[j]);
		}
	}
}

