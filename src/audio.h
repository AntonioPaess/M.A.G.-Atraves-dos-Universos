#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

// Estrutura para agrupar sons, se necessário
// typedef struct {
//     Sound shoot;
//     Sound enemyHit;
//     Sound playerHit;
//     Sound levelUp;
//     Sound gameOver;
//     Music backgroundMusic;
// } GameAudio;

// Funções para carregar e descarregar recursos de áudio
void LoadAudioResources(void);
void UnloadAudioResources(void);

// Funções para tocar sons e música
void PlaySoundEffect(Sound fx); // Toca um efeito sonoro uma vez
void PlayBackgroundMusic(Music music); // Inicia ou continua uma música de fundo (em loop)
void UpdateMusicStreamLocal(Music music); // Necessário para músicas em stream
void StopBackgroundMusic(Music music);
void PauseBackgroundMusic(Music music);
void ResumeBackgroundMusic(Music music);

#endif // AUDIO_H
