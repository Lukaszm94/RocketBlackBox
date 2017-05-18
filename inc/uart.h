/*
 * uart.h
 *
 *  Created on: 11 lut 2016
 *      Author: LukaszSSD
 */

#ifndef UART_H_
#define UART_H_

#define UART_TX_PIN GPIO_Pin_9
#define UART_RX_PIN GPIO_Pin_10
#define UART_TX_PS GPIO_PinSource9
#define UART_RX_PS GPIO_PinSource10
#define UART_GPIO GPIOA


void UART_init();
void UART_sendChar(char c);
void UART_sendString(char* str);
void UART_sendInt(int value);
uint8_t UART_isAvailable();
void UART_clear_input();
char UART_readChar();
//int __io_putchar(int c); // needed by printf

#endif /* UART2_H_ */
