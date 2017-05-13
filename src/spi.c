/*
 * spi.c
 *
 *  Created on: 13 mar 2017
 *      Author: Luke
 */
// http://www.lxtronic.com/index.php/basic-spi-simple-read-write

#include "spi.h"
#include <stm32f0xx_spi.h>
#include "millis.h"

void SPI_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);


	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	//SCK, MISO, MOSI
	gpio.GPIO_Pin = SPI_SCK_PIN | SPI_MISO_PIN | SPI_MOSI_PIN;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SPI_GPIO, &gpio);

	/*gpio.GPIO_Pin = SPI_MISO_PIN;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SPI_GPIO, &gpio);*/

	//CS
	gpio.GPIO_Pin = SPI_CS_PIN;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(SPI_CS_GPIO, &gpio);
	SPI_deselectDevice();

	GPIO_PinAFConfig(SPI_GPIO, SPI_SCK_PS, GPIO_AF_0);
	GPIO_PinAFConfig(SPI_GPIO, SPI_MISO_PS, GPIO_AF_0);
	GPIO_PinAFConfig(SPI_GPIO, SPI_MOSI_PS, GPIO_AF_0);

	SPI_InitTypeDef spi;
	SPI_StructInit(&spi);

	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(SPI1, &spi);
	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);

	SPI_Cmd(SPI1, ENABLE);
}

void SPI_selectDevice(void)
{
	GPIO_ResetBits(SPI_CS_GPIO, SPI_CS_PIN);
}

void SPI_deselectDevice(void)
{
	GPIO_SetBits(SPI_CS_GPIO, SPI_CS_PIN);
}

void SPI_sendByte(uint8_t byte)
{
	//GPIO_ResetBits(SPI_CS_GPIO, SPI_CS_PIN);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); //TODO add timeout
	SPI_SendData8(SPI1, byte);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)); //TODO add timeout
	SPI_ReceiveData8(SPI1);
	//GPIO_SetBits(SPI_CS_GPIO, SPI_CS_PIN);
}

void SPI_writeRegister(uint8_t address, uint8_t data)
{
	SPI_writeRegisters(address, &data, 1);
}
void SPI_writeRegisters(uint8_t startAddress, uint8_t* data, uint8_t length)
{
	GPIO_ResetBits(SPI_CS_GPIO, SPI_CS_PIN);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); //TODO add timeout
	// WARNING: TXE flag is set when TX FIFO buffer is filled to more than 1/2 (so a few bytes). Writing one byte to empty TX FIFO will not set this flag!
	SPI_SendData8(SPI1, startAddress);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)); //TODO add timeout
	SPI_ReceiveData8(SPI1);

	int i = 0;
	for(i = 0; i < length; i++) {
		while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); //TODO add timeout
		SPI_SendData8(SPI1, data[i]);
		while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)); //TODO add timeout
		SPI_ReceiveData8(SPI1);
	}
	GPIO_SetBits(SPI_CS_GPIO, SPI_CS_PIN);
}

uint8_t SPI_readRegister(uint8_t command)
{
	SPI_selectDevice();
	SPI_sendByte(command);
	uint8_t registerValue = SPI_receiveByte();
	SPI_deselectDevice();
	return registerValue;
}

uint8_t SPI_receiveByte(/*uint8_t address*/)
{
	/*GPIO_ResetBits(SPI_CS_GPIO, SPI_CS_PIN);

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); //TODO add timeout
	SPI_SendData8(SPI1, address);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)); //TODO add timeout
	SPI_ReceiveData8(SPI1);*/

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_SendData8(SPI1, 0x00); //Dummy byte to generate clock
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

	//GPIO_SetBits(SPI_CS_GPIO, SPI_CS_PIN);
	return SPI_ReceiveData8(SPI1);
}

