/*
 * spi.h
 *
 *  Created on: 13 mar 2017
 *      Author: Luke
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f0xx.h"

#define SPI_SCK_PIN GPIO_Pin_3
#define SPI_MISO_PIN GPIO_Pin_4
#define SPI_MOSI_PIN GPIO_Pin_5
#define SPI_CS_PIN GPIO_Pin_6
#define SPI_SCK_PS GPIO_PinSource3
#define SPI_MISO_PS GPIO_PinSource4
#define SPI_MOSI_PS GPIO_PinSource5

#define SPI_GPIO GPIOB
#define SPI_CS_GPIO GPIOA

void SPI_init(void);
void SPI_selectDevice(void);
void SPI_deselectDevice(void);
void SPI_sendByte(uint8_t byte); // send single byte over SPI, no CS handling!
void SPI_writeRegister(uint8_t address, uint8_t data); // write single byte to register
void SPI_writeRegisters(uint8_t startAddress, uint8_t* data, uint8_t length); // write multiple bytes to register
uint8_t SPI_readRegister(uint8_t command); // includes CS handling
uint8_t SPI_receiveByte(/*uint8_t address*/); // receive single byte over SPI, no CS handling!


#endif /* SPI_H_ */
