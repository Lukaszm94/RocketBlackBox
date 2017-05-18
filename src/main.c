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
#include "memory.h"
#include "memorymanager.h"
#include "buttons.h"
#include "config.h"
			
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

void perform_startup_sequence()
{
	delay(200);
	if(!buttons_is_button_pressed(BUTTON_1)) {
		return;
	}
	delay(2000);
	if(buttons_is_button_pressed(BUTTON_1)) {
		return;
	}
	uint32_t wait_until = millis() + 3000;
	uint8_t button_was_pressed = 0;
#if DEBUG
	UART_sendString("Waiting 3000ms for button press\n");
#endif
	while(millis() < wait_until) {
		if(buttons_is_button_pressed(BUTTON_1)) {
			button_was_pressed = 1;
			break;
		}
	}
	if(!button_was_pressed) {
		return;
	}
	mm_dump_memory();
	// 10 seconds window to send erase command
	wait_until = millis() + 10000;
	uint8_t erase = 0;
#if DEBUG
	UART_sendString("Waiting 10000ms for erase command\n");
#endif
	// empty input buffer
	char c;
	while(ring_buffer_num_items(&usart_rx_buffer)) {
		ring_buffer_dequeue(&usart_rx_buffer, &c);
	}
	while(millis() < wait_until) {
		if(ring_buffer_num_items(&usart_rx_buffer)) {
			ring_buffer_dequeue(&usart_rx_buffer, &c);
			if(c == 'e') {
				erase = 1;
			}
			break;
		}
	}
	if(erase) {
		mm_reset_memory();
	}
}

int main(void)
{
	RCC_conf();
	millis_init();
	UART_init();
	memory_init();
	memory_unlock_protection();
	buttons_init();
	ring_buffer_init(&usart_rx_buffer);
	mm_init();
	perform_startup_sequence();

	for(;;) {
		if(ring_buffer_num_items(&usart_rx_buffer) >= (RING_BUFFER_SIZE - 1)) {
			NVIC_DisableIRQ(USART1_IRQn);
			char buff[RING_BUFFER_SIZE];
			uint8_t bytes = ring_buffer_num_items(&usart_rx_buffer);
			uint8_t bytesRead = ring_buffer_dequeue_arr(&usart_rx_buffer, &buff[0], bytes);
			NVIC_EnableIRQ(USART1_IRQn);
			mm_append_page((uint8_t*)buff, bytesRead);
		}
		mm_update_autosave();
	}
}
