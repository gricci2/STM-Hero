/*
 * led.c
 *
 *  Created on: Apr 30, 2026
 *      Author: Greggory
 */

#include "led.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_def.h"
#include <string.h>

static uint32_t framebuffer[8] = {0};
extern SPI_HandleTypeDef hspi1;

static uint8_t memFrame[8];
static uint8_t txFrame[8];
static volatile uint8_t dma_busy = 0;


void LED_Send(uint8_t reg, uint8_t data)
{
	uint8_t buf[2] = {reg, data};

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, buf, 2, 100);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

}

void LED_SendAll(uint8_t reg, uint8_t data)
{
	uint8_t buf[8] = {reg, data,
			reg, data,
			reg, data,
			reg, data};

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, buf, 8, 100);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

}


void LED_Clear(void)
{
	memset(framebuffer, 0, sizeof(framebuffer));
}

void LED_Fill(void)
{
	memset(framebuffer, 1, sizeof(framebuffer));
}

void LED_Init(void)
{
	LED_SendAll(0x0F, 0x00);
	LED_SendAll(0x0C, 0x01);
	LED_SendAll(0x0B, 0x07);
	LED_SendAll(0x0A, 0x08);
	LED_SendAll(0x09, 0x00);
	LED_Clear();
}

void LED_SetPixel(uint8_t x, uint8_t y, uint8_t value)
{
  if(x >= 8 || y >= 32) return;

  if(value)
  {
	  framebuffer[x] |= (1UL << y);
  }
  else
  {
	  framebuffer[x] &= ~(1UL << y);
  }
}




void LED_Update(void)
{
	if(dma_busy)
	{
		return;
	}

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	dma_busy = 1;
	HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(&hspi1, txFrame, 8);

	if(status != HAL_OK)
	{
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	    dma_busy = 0;
	}
//	HAL_StatusTypeDef status =
//	        HAL_SPI_Transmit_DMA(&hspi1, txFrame, 64);
//
//	    if(status != HAL_OK)
//	    {
//	        dma_busy = 0;
//	    }
}

void LED_PrepareFrame(void)
{
    memcpy(txFrame, memFrame, sizeof(memFrame));
}

void LED_Build_Frame(void)
{
	for(uint8_t col = 0; col < 8; col++)
	{
		uint32_t line = framebuffer[col];

		uint8_t b0 = (line >> 0) & (0xFF);
		uint8_t b1 = (line >> 8) & (0xFF);
		uint8_t b2 = (line >> 16) & (0xFF);
		uint8_t b3 = (line >> 24) & (0xFF);

		memFrame[0] = col + 1;
		memFrame[1] = b3;
		memFrame[2] = col + 1;
		memFrame[3] = b2;
		memFrame[4] = col + 1;
		memFrame[5] = b1;
		memFrame[6] = col + 1;
		memFrame[7] = b0;

		LED_PrepareFrame();
		LED_Update();
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	//if(hspi->Instance == SPI1)

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		dma_busy = 0;

}

//void LED_Update(void)
//{
//	uint8_t buf[8];
//
//	for(uint8_t col = 0; col < 8; col++)
//	{
//	  uint32_t line = framebuffer[col];
//
//	  uint8_t b0 = (line >> 0) & (0xFF);
//	  uint8_t b1 = (line >> 8) & (0xFF);
//	  uint8_t b2 = (line >> 16) & (0xFF);
//	  uint8_t b3 = (line >> 24) & (0xFF);
//
//	  buf[0] = col + 1;
//	  buf[1] = b3;
//	  buf[2] = col + 1;
//	  buf[3] = b2;
//	  buf[4] = col + 1;
//	  buf[5] = b1;
//	  buf[6] = col + 1;
//	  buf[7] = b0;
//
//	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
//	  HAL_SPI_Transmit(&hspi1, buf, 8, 100);
//	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
//	}
//}
