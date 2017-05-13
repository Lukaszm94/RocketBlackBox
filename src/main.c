/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "millis.h"
#include "uart.h"
#include "ringbuffer.h"
			
#define LED_PIN GPIO_Pin_6

ring_buffer_t usart_rx_buffer;

void RCC_conf()
{
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_OFF);
	FLASH_PrefetchBufferCmd(ENABLE);
	FLASH_SetLatency(FLASH_Latency_1);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLKConfig(RCC_HCLK_Div1);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_8); // HSI=8MHz, f_PLL=32MHz
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void initGPIO(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_6;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &gpio);
}

void USART1_IRQHandler()
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		char c = USART_ReceiveData(USART1);
		ring_buffer_queue(&usart_rx_buffer, c);
		if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET) {
			USART_ClearFlag(USART1, USART_FLAG_ORE);
		}
	}
}

int main(void)
{
	RCC_conf();
	millis_init();
	initGPIO();

	ring_buffer_init(&usart_rx_buffer);
	UART_init();



	for(;;) {
		if(ring_buffer_num_items(&usart_rx_buffer) > (RING_BUFFER_SIZE - 10)) {
			NVIC_DisableIRQ(USART1_IRQn);
			char buff[RING_BUFFER_SIZE];
			uint8_t bytes = ring_buffer_num_items(&usart_rx_buffer);
			uint8_t bytesRead = ring_buffer_dequeue_arr(&usart_rx_buffer, &buff[0], bytes);
			if(bytesRead != bytes) {
				GPIO_SetBits(GPIOA, LED_PIN);
			}
			NVIC_EnableIRQ(USART1_IRQn);
			for(int i = 0; i < bytesRead; i++) {
				UART_sendChar(buff[i]);
			}

			//UART_sendChar('\n');
		}
//		NVIC_DisableIRQ(USART1_IRQn);
//		delay(500);
//		NVIC_EnableIRQ(USART1_IRQn);
//		delay(10);
	}
}
