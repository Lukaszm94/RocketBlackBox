/*
 * buttons.c
 *
 *  Created on: 18 mar 2017
 *      Author: Luke
 */
#include "buttons.h"

void buttons_init()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = BUTTON_1_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BUTTON_GPIO, &gpio);
}

uint8_t buttons_is_button_pressed(uint8_t button)
{
	uint16_t pin = BUTTON_1_PIN;
	if(button == BUTTON_1) {
		pin = BUTTON_1_PIN;
	}/* else if(button == BUTTON_2) {
		pin = BUTTON_2_PIN;
	}*/ else {
		return 0;
	}
	return (!GPIO_ReadInputDataBit(BUTTON_GPIO, pin));
}
