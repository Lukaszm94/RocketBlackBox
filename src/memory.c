/*
 * memory.c
 *
 *  Created on: 15 mar 2017
 *      Author: Luke
 */

#include <stdint.h>
#include "spi.h"

#define MEMORY_PAGE_PROGRAM_OPERATION 0x02
#define MEMORY_READ_OPERATION 0x03
#define MEMORY_HIGH_SPEED_READ_OPERATION 0x0B
#define MEMORY_READ_STATUS_REGISTER_OPERATION 0x05
#define MEMORY_READ_CONFIGURATION_REGISTER_OPERATION 0x35
#define MEMORY_GLOBAL_BLOCK_PROTECTION_UNCLOCK_OPERATION 0x98
#define MEMORY_ENABLE_WRITE_OPERATION 0x06
#define MEMORY_SECTOR_ERASE_OPERATION 0x20
#define MEMORY_CHIP_ERASE_OPERATION 0xC7

#define MEMORY_BUSY_FLAG (1<<7)


void memory_init(void)
{
	SPI_init();
}
void memory_enable_write()
{
	SPI_selectDevice();
	SPI_sendByte(MEMORY_ENABLE_WRITE_OPERATION);
	SPI_deselectDevice();
}

void memory_read(uint32_t address, uint8_t* buffer, uint16_t length)
{
	SPI_selectDevice();
	SPI_sendByte(MEMORY_READ_OPERATION);
	uint8_t a3 = (uint8_t)((address & 0x00FF0000) >> 16); // MSB
	uint8_t a2 = (uint8_t)((address & 0x0000FF00) >> 8);
	uint8_t a1 = (uint8_t)(address & 0x000000FF); // LSB
	SPI_sendByte(a3);
	SPI_sendByte(a2);
	SPI_sendByte(a1);
	int i = 0;
	for(i = 0; i < length; i++) {
		buffer[i] = SPI_receiveByte();
	}
	SPI_deselectDevice();
}

void memory_write(uint32_t address, uint8_t* buffer, uint16_t length)
{
	memory_enable_write();
	SPI_selectDevice();
	SPI_sendByte(MEMORY_PAGE_PROGRAM_OPERATION);
	uint8_t a3 = (uint8_t)((address & 0x00FF0000) >> 16); // MSB
	uint8_t a2 = (uint8_t)((address & 0x0000FF00) >> 8);
	uint8_t a1 = (uint8_t)(address & 0x000000FF); // LSB
	SPI_sendByte(a3);
	SPI_sendByte(a2);
	SPI_sendByte(a1);
	int i = 0;
	for(i = 0; i < length; i++) {
		SPI_sendByte(buffer[i]);
	}
	SPI_deselectDevice();
}

uint8_t memory_read_status_register()
{
	return SPI_readRegister(MEMORY_READ_STATUS_REGISTER_OPERATION);
}

uint8_t memory_read_configuration_register()
{
	return SPI_readRegister(MEMORY_READ_CONFIGURATION_REGISTER_OPERATION);
}

uint8_t memory_get_busy_flag()
{
	return (memory_read_status_register() & MEMORY_BUSY_FLAG);
}

void memory_sector_erase(uint32_t address)
{
	memory_enable_write();
	SPI_selectDevice();
	SPI_sendByte(MEMORY_SECTOR_ERASE_OPERATION);
	uint8_t a3 = (uint8_t)((address & 0x00FF0000) >> 16); // MSB
	uint8_t a2 = (uint8_t)((address & 0x0000FF00) >> 8);
	uint8_t a1 = (uint8_t)(address & 0x000000FF); // LSB
	SPI_sendByte(a3);
	SPI_sendByte(a2);
	SPI_sendByte(a1);
	SPI_deselectDevice();
}

void memory_chip_erase()
{
	memory_enable_write();
	SPI_selectDevice();
	SPI_sendByte(MEMORY_CHIP_ERASE_OPERATION);
	SPI_deselectDevice();
	while(memory_get_busy_flag()); // wait for chip to be erased
}

void memory_unlock_protection()
{
	memory_enable_write();
	SPI_selectDevice();
	SPI_sendByte(MEMORY_GLOBAL_BLOCK_PROTECTION_UNCLOCK_OPERATION);
	SPI_deselectDevice();
}


