/*
 * song.h
 *
 *  Created on: Apr 30, 2026
 *      Author: Greggory
 */

#ifndef INC_SONG_H_
#define INC_SONG_H_
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t time;
    uint8_t lanes;
} Note;

typedef struct {
	uint32_t hit_time;
	uint8_t lane;
	uint8_t active;
	uint8_t hit;
} ActiveNote;

typedef enum
{
	Wii,
	Mario,
	Tetris
} SongList;



extern const char* song_name_Wii;
extern const Note song_Wii[];
extern const uint32_t song_length_Wii;

extern const char* song_name_Mario;
extern const Note song_Mario[];
extern const uint32_t song_length_Mario;

extern const char* song_name_Tetris;
extern const Note song_Tetris[];
extern const uint32_t song_length_Tetris;

const char* GetSongName(SongList currentSong);
const uint32_t GetSongLength(SongList currentSong);
const Note* GetSongAddress(SongList currentSong);

#endif /* INC_SONG_H_ */
