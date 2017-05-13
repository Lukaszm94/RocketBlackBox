/*
 * memory.h
 *
 *  Created on: 15 mar 2017
 *      Author: Luke
 */

#ifndef MEMORY_H_
#define MEMORY_H_

void memory_init(void);
void memory_enable_write();
void memory_read(uint32_t address, uint8_t* buffer, uint16_t length);
void memory_write(uint32_t address, uint8_t* buffer, uint16_t length);

uint8_t memory_read_status_register();
uint8_t memory_read_configuration_register();
uint8_t memory_get_busy_flag(); // returns 1 if set, 0 if not set
void memory_sector_erase(uint32_t address); // MSB first
void memory_chip_erase();

void memory_unlock_protection();



#endif /* MEMORY_H_ */
