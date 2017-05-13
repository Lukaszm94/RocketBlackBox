/*
 * millis.c
 *
 *  Created on: 11 May 2017
 *      Author: luke
 */
#include "millis.h"
#include "stm32f0xx.h"

volatile uint32_t msec;

void millis_init()
{
	msec = 0;
	SysTick_Config(SystemCoreClock/1000);
}

void SysTick_Handler(void)
{
	msec++;
}

uint32_t millis()
{
	return msec;
}

void delay(uint32_t delayTime)
{
	uint32_t until = millis() + delayTime;
	while(millis() < until);
}

