/*
 * game.c
 *
 *  Created on: May 5, 2026
 *      Author: Greggory
 */
#include "game.h"
#include "stm32f4xx_hal.h"
#include "oled.h"
#include "song.h"
#include <stdio.h>
#include <string.h>

#define NOTE_TRAVEL_TIME_MS 1400
#define NOTE_TRAVEL_TIME_US 1400000
#define LED_ROWS 32
#define LANES 8
#define MAX_ACTIVE_NOTES 32
#define NOTE_CLEANUP_TIME 200
#define NOTE_CLEANUP_TIME_US 200000
#define PERFECT_WINDOW_US  50000
#define GOOD_WINDOW_US    100000
#define MISS_WINDOW_US    150000

uint32_t song_start_time = 0;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

static GameState game_state = GAME_STATE_MENU;
static uint32_t next_song_note = 0;
static const Note* song;
static uint32_t song_length;

static ActiveNote active_notes[MAX_ACTIVE_NOTES];

volatile uint16_t perfect_count = 0;
volatile uint16_t good_count = 0;
volatile uint16_t bad_count = 0;
volatile uint16_t miss_count = 0;

uint32_t micros(void)
{
	return TIM2->CNT;
}

void ResetScore(void)
{
	perfect_count = 0;
	good_count = 0;
	bad_count = 0;
	miss_count = 0;
}

//uint32_t millis(void)
//{
//	return micros()/1000;
//}

void SpawnActiveNote(uint32_t hit_time, uint8_t lane)
{
	for(uint32_t i = 0; i < MAX_ACTIVE_NOTES; i++)
	{
		if(!active_notes[i].active)
		{
//			char msg[64];
//			sprintf(msg, "spawn lane %d\r\n", lane);
//			HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

			active_notes[i].active = 1;
			active_notes[i].hit_time = hit_time;
			active_notes[i].lane = lane;
			active_notes[i].hit = 1;
			return;
		}
	}
}


void SpawnFromHex(uint32_t hit_time, uint8_t hexLanes)
{
	for(uint8_t i = 0; i < LANES; i++)
	{
		if(hexLanes & (1 << i))
		{
			if(i / 2 == 0)
			{
				hexLanes |= (3 << 0);
			}
			else if(i / 2 == 1)
			{
				hexLanes |= (3 << 2);
			}
			else if(i / 2 == 2)
			{
				hexLanes |= (3 << 4);
			}
			else if(i / 2 == 3)
			{
				hexLanes |= (3 << 6);
			}
		}
	}
	for(uint8_t i = 0; i < LANES; i++)
	{
		if(hexLanes & (1 << i))
		{
			SpawnActiveNote(hit_time, i);
		}
	}
}


void StartSong(void)
{
    song_start_time = micros();
    next_song_note = 0;
    song = GetSongAddress(CurrentSong);
    song_length = GetSongLength(CurrentSong);

    for (int i = 0; i < MAX_ACTIVE_NOTES; i++)
	{
		active_notes[i].active = 0;
	}
}


void OLED_Render(void)
{
	OLED_Clear();
	switch(game_state)
	{
	case GAME_STATE_MENU:
		OLED_MainMenu();
		break;
	case GAME_STATE_RUNNING:
		OLED_Running();
		break;
	case GAME_STATE_RESULTS:
		OLED_Results();
	}
	OLED_Refresh();
}

void SongUpdate(uint32_t song_time)
{
	while(next_song_note < song_length)
	{
		const Note* note = &song[next_song_note];

		if(note->time > song_time + NOTE_TRAVEL_TIME_US)
		{
			break;
		}

		SpawnFromHex(note->time, note->lanes);
		next_song_note++;


//		char msg[64];
//		sprintf(msg, "t=%lu note=%lu\r\n", current_time, note->time);
//		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);


	}
}


void RenderNotes(uint32_t song_time)
{
	LED_Clear();

	for(uint32_t i = 0; i < MAX_ACTIVE_NOTES; i++)
	{

		if(!active_notes[i].active)
		{
			continue;
		}

		int32_t time_to_hit = (int32_t)active_notes[i].hit_time - (int32_t)song_time;

		if(time_to_hit < -NOTE_CLEANUP_TIME_US || time_to_hit > NOTE_TRAVEL_TIME_US)
			continue;

		float progress = 1.0f - ((float)time_to_hit / (float)NOTE_TRAVEL_TIME_US);

		if(progress < 0.0f) progress = 0.0f;
		if(progress > 1.0f) progress = 1.0f;

		uint8_t y = (uint8_t)(progress * (LED_ROWS - 1));






//		int32_t dt = NOTE_TRAVEL_TIME_US - time_to_hit;
//		int32_t progress = (dt * 255) / NOTE_TRAVEL_TIME_US;
//
//		if (progress < 0) progress = 0;
//		if (progress > 255) progress = 255;
//
//		uint8_t y = (progress * (LED_ROWS - 1)) / 255;


		LED_SetPixel(active_notes[i].lane, y, 1);


//		char msg[80];
//		sprintf(msg, "hit=%lu now=%lu dt=%ld lane=%d\r\n",
//		        active_notes[i].hit_time,
//		        current_time,
//		        (int32_t)time_to_hit,
//		        active_notes[i].lane);
//		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
	}
}

void RegisterHit(HitType hit)
{
	switch(hit)
	{
	case HIT_PERFECT:
		perfect_count++;
		break;
	case HIT_GOOD:
		good_count++;
		break;
	case HIT_BAD:
		bad_count++;
		break;
	case HIT_MISS:
		miss_count++;
		break;
	}
}

void CleanupNotes(uint32_t song_time)
{
	for(uint32_t i = 0; i < MAX_ACTIVE_NOTES; i++)
	{
		if(!active_notes[i].active)
		{
			continue;
		}

		int32_t time_to_hit = (int32_t)active_notes[i].hit_time - (int32_t)song_time;
		if(time_to_hit < -NOTE_CLEANUP_TIME_US)
		{
			active_notes[i].active = 0;
			RegisterHit(HIT_MISS);
		}
	}
}


void GameRender(uint32_t song_time)
{
	if (game_state == GAME_STATE_RUNNING)
	{
		RenderNotes(song_time);
	}
	else
	{
		LED_Clear();
	}
}

void JudgeLaneHit(uint8_t lane, uint32_t timestamp)
{
	ActiveNote* best_note = NULL;
	ActiveNote* best_note_two = NULL;

	int32_t best_error = MISS_WINDOW_US + 1;

	for(uint32_t i = 0; i < MAX_ACTIVE_NOTES; i++)
	{
		if(!active_notes[i].hit)
			continue;

		if(active_notes[i].lane/2 != lane)
			continue;

		int32_t error = (int32_t)timestamp - (int32_t)active_notes[i].hit_time;
		int32_t abs_error = (error < 0) ? -error : error;

		if(abs_error < best_error)
		{
			best_error = abs_error;
			best_note = &active_notes[i];
			continue;
		}

		if(abs_error == best_error)
		{
			best_note_two = &active_notes[i];
			best_note_two->hit = 0;
		}
	}

	if(best_note == NULL)
	{
		return;
	}

	if(best_error <= PERFECT_WINDOW_US)
	{
		RegisterHit(HIT_PERFECT);
	}
	else if(best_error <= GOOD_WINDOW_US)
	{
		RegisterHit(HIT_GOOD);
	}
	else if(best_error <= MISS_WINDOW_US)
	{
		RegisterHit(HIT_BAD);
	}
	else
	{
		return;
	}

	best_note->hit = 0;
}

void GameUpdate(uint32_t song_time)
{
	if(blue_button_pressed)
	{
		blue_button_pressed = 0;
		switch(game_state)
		{
		case GAME_STATE_MENU:
			StartSong();
			game_state = GAME_STATE_RUNNING;
			ResetScore();
			OLED_Render();
			return;
			break;
		case GAME_STATE_RUNNING:
			game_state = GAME_STATE_RESULTS;
			LED_Clear();
			OLED_Render();
			break;
		case GAME_STATE_RESULTS:
			game_state = GAME_STATE_MENU;
			OLED_Render();
			break;
		}
	}

	if (game_state == GAME_STATE_RUNNING)
	{
		SongUpdate(song_time);
		CleanupNotes(song_time);
	}
	else if(game_state == GAME_STATE_MENU)
	{
		OLED_Render();
	}
}





