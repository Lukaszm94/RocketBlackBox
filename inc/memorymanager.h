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

uint32_t mm_read_data_counter(uint8_t* okFlag);
//uint8_t mm_read_measurement_started_flag();
void mm_read_data(uint32_t address, TemperatureMeasurement buffer[], uint8_t bufferLength);

void mm_save_data_info(uint32_t newCounter, uint8_t newFlag);
void mm_save_data(uint32_t address, TemperatureMeasurement buffer[], uint8_t bufferLength); // IMPORTANT: address starts from 0, MM automatically adds offset
void mm_reset_memory(); // erase everything (data counter, flag and saved data)

#endif /* MEMORYMANAGER_H_ */
