/*
 * game.h
 *
 *  Created on: May 5, 2026
 *      Author: Greggory
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_
#include <stdint.h>
#include "led.h"
#include "song.h"

extern volatile uint16_t noteIndex;
extern volatile uint8_t blue_button_pressed;

extern volatile uint16_t perfect_count;
extern volatile uint16_t good_count;
extern volatile uint16_t bad_count;
extern volatile uint16_t miss_count;

uint32_t micros(void);
//uint32_t millis(void);

void ResetScore(void);

typedef enum
{
	GAME_STATE_MENU,
	GAME_STATE_RUNNING,
	GAME_STATE_RESULTS
} GameState;

typedef enum
{
	HIT_PERFECT,
	HIT_GOOD,
	HIT_BAD,
	HIT_MISS
} HitType;

void SpawnActiveNote(uint32_t hit_time, uint8_t lane);
void SpawnFromHex(uint32_t hit_time, uint8_t hexLanes);
void StartSong(void);
void OLED_Render(void);
void SongUpdate(uint32_t current_time);
void RenderNotes(uint32_t current_time);
void RegisterHit(HitType hit);
void CleanupNotes(uint32_t current_time);
void GameRender(uint32_t now);
void JudgeLaneHit(uint8_t lane, uint32_t timestamp);
void GameUpdate(uint32_t now);
#endif /* INC_GAME_H_ */
