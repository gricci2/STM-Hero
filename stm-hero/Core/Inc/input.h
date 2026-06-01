/*
 * input.h
 *
 *  Created on: May 18, 2026
 *      Author: Greggory
 */

#ifndef INC_INPUT_H_
#define INC_INPUT_H_
#include <stdint.h>

void PushInputEvent(uint8_t lane, uint32_t time);
void ProcessInputs(uint32_t song_time);

#endif /* INC_INPUT_H_ */
