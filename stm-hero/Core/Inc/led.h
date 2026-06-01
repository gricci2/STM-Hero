/*
 * led.h
 *
 *  Created on: Apr 30, 2026
 *      Author: Greggory
 */

#ifndef INC_LED_H_
#define INC_LED_H_
#include <stdint.h>

void LED_Send(uint8_t reg, uint8_t data);
void LED_SendAll(uint8_t reg, uint8_t data);
void LED_Clear(void);
void LED_Init(void);
void LED_SetPixel(uint8_t x, uint8_t y, uint8_t value);
void LED_Update(void);
void LED_Fill(void);
void LED_PrepareFrame(void);
void LED_Build_Frame(void);


#endif /* INC_LED_H_ */
