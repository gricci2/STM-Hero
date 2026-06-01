/*
 * input.c
 *
 *  Created on: May 18, 2026
 *      Author: Greggory
 */
#include "input.h"
#include "game.h"
#define INPUT_QUEUE_SIZE 32


typedef struct {
	uint8_t lane;
	uint32_t timestamp;
} InputEvent;

volatile InputEvent input_queue[INPUT_QUEUE_SIZE];

volatile uint8_t input_head = 0;
volatile uint8_t input_tail = 0;

void PushInputEvent(uint8_t lane, uint32_t time)
{
	uint8_t next = (input_head + 1) % INPUT_QUEUE_SIZE;

	if (next == input_tail)
	{
		return;
	}

	input_queue[input_head].lane = lane;
	input_queue[input_head].timestamp = time;

	input_head = next;
}

void ProcessInputs(uint32_t song_time)
{
	while(input_tail != input_head)
	{
		InputEvent event = input_queue[input_tail];

		input_tail = (input_tail + 1) % INPUT_QUEUE_SIZE;

		JudgeLaneHit(event.lane, event.timestamp);
	}
}

