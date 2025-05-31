#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

void LoadGameAudio(Sound *shoot, Sound *enemyExplode, Sound *playerExplode, 
                 Sound *enemyNormalDeath, Sound *enemyTankDeath, 
                 Sound *enemyExploderDeath, Sound *enemyShooterDeath,
                 Sound *dashSound, // Adicione esta linha
                 Music *bgMusic, Music *menuMusic, Music *tutorialMusic, 
                 Music *pauseMusic, Music *gameOverMusic, Music *nameEntryMusic,
                 Music *bossMusic,
                 Sound *menuClick, Sound *powerupDamageSound, Sound *powerupHealSound, Sound *powerupShieldSound);

void UnloadGameAudio(Sound shoot, Sound enemyExplode, Sound playerExplode,
                    Sound enemyNormalDeath, Sound enemyTankDeath, 
                    Sound enemyExploderDeath, Sound enemyShooterDeath,
                    Sound dashSound, // Adicione esta linha
                    Music bgMusic, Music menuMusic, Music tutorialMusic, 
                    Music pauseMusic, Music gameOverMusic, Music nameEntryMusic,
                    Music bossMusic,
                    Sound menuClick, Sound powerupDamageSound, Sound powerupHealSound, Sound powerupShieldSound);

void PlayGameSound(Sound sound);
void UpdateGameMusicStream(Music music);

#endif