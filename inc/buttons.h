/*
 * buttons.h
 *
 *  Created on: 18 mar 2017
 *      Author: Luke
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <stm32f0xx.h>

#define BUTTON_1_PIN GPIO_Pin_8
#define BUTTON_GPIO GPIOB

#define BUTTON_1 1

void buttons_init();
uint8_t buttons_is_button_pressed(uint8_t button); // returns 1 if pressed, 0 otherwise

#endif /* BUTTONS_H_ */
