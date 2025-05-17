#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

void LoadGameAudio(Sound *shoot, Sound *enemyExplode, Sound *playerExplode, Music *bgMusic);
void UnloadGameAudio(Sound shoot, Sound enemyExplode, Sound playerExplode, Music bgMusic);
void PlayGameSound(Sound sound);
void UpdateGameMusicStream(Music music);

#endif // AUDIO_H