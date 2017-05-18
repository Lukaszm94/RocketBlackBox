/*
 * uart2.c
 *
 *  Created on: 11 lut 2016
 *      Author: LukaszSSD
 */

#include "stm32f0xx.h"
#include "uart.h"
#include <stdio.h>

void UART_init()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin = UART_TX_PIN; // TX pin
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(UART_GPIO, &gpio);

	gpio.GPIO_Pin = UART_RX_PIN; // RX pin
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(UART_GPIO, &gpio);

	GPIO_PinAFConfig(UART_GPIO, UART_TX_PS, GPIO_AF_1);
	GPIO_PinAFConfig(UART_GPIO, UART_RX_PS, GPIO_AF_1);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 9600;
	USART_Init(USART1, &uart);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);

	USART_Cmd(USART1, ENABLE);
}

void UART_sendChar(char c)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // wait for output buffer to be empty
	USART_SendData(USART1, c);
}

void UART_sendString(char* str)
{
	while (*str) {
		UART_sendChar(*str++);
	}
}

void UART_sendInt(int value)
{
	char buffer[12];
	itoa(value, buffer, 10);
	UART_sendString(buffer);
}

uint8_t UART_isAvailable()
{
	return (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET);
}

void UART_clear_input()
{
	if(UART_isAvailable()) {
		UART_readChar();
	}
}

char UART_readChar()
{
	return USART_ReceiveData(USART1);
}

/*int __io_putchar(int c)
{
	if (c=='\n')
		UART_sendChar('\r');
	UART_sendChar(c);
	return c;
}*/
