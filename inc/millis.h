/*
 * millis.h
 *
 *  Created on: 11 May 2017
 *      Author: luke
 */

#ifndef MILLIS_H_
#define MILLIS_H_

#include "stm32f0xx.h"

void millis_init();
void SysTick_Handler(void);
uint32_t millis();
void delay(uint32_t delayTime);

#endif /* MILLIS_H_ */
