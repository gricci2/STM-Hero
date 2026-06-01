/*
 * oled.h
 *
 *  Created on: Apr 30, 2026
 *      Author: Greggory
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_
#include "song.h"
#include <stdint.h>
#include "stm32f4xx_hal.h"

extern volatile uint8_t up_button_pressed;
extern volatile uint8_t down_button_pressed;
extern SongList CurrentSong;


void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void OLED_DrawChar(uint8_t x, uint8_t page, const char c);
void OLED_Print(uint8_t x, uint8_t page, const char *str);
void OLED_MainMenu(void);
void OLED_Running(void);
void OLED_Results(void);
HAL_StatusTypeDef OLED_Refresh(void);




#endif /* INC_OLED_H_ */
