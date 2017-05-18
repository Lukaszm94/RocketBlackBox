/*
 * memorymanager.h
 *
 *  Created on: 20 mar 2017
 *      Author: Luke
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include <stm32f0xx.h>
//#include "datatypes.h"

void mm_init(void);
void mm_update_autosave();
void mm_append_page(uint8_t* buffer, uint16_t buffer_length); // 256 bytes max
void mm_autosave();

void mm_reset_memory(); // erase everything (data counter and saved data)
void mm_dump_memory(); // print saved data to serial port

#endif /* MEMORYMANAGER_H_ */
