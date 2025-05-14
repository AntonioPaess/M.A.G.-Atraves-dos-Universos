#include "audio.h"
#include "raylib.h"

// Variáveis globais para os sons e música (exemplo)
// Sound fxShoot;
// Sound fxEnemyHit;
// Sound fxPlayerHit;
// Sound fxLevelUp;
// Sound fxGameOver;
// Music musicBackground;

void LoadAudioResources(void) {
    InitAudioDevice(); // Inicializa o dispositivo de áudio

    // Carregar efeitos sonoros - Exemplo: substitua "path/to/" pelos caminhos reais na pasta assets
    // fxShoot = LoadSound("assets/audio/shoot.wav");
    // fxEnemyHit = LoadSound("assets/audio/enemy_hit.wav");
    // fxPlayerHit = LoadSound("assets/audio/player_hit.wav");
    // fxLevelUp = LoadSound("assets/audio/level_up.wav");
    // fxGameOver = LoadSound("assets/audio/game_over.wav");

    // Carregar música de fundo - Exemplo:
    // musicBackground = LoadMusicStream("assets/audio/background_music.ogg");
    // PlayMusicStream(musicBackground);
    // SetMusicVolume(musicBackground, 0.5f);
}

void UnloadAudioResources(void) {
    // Descarregar efeitos sonoros
    // UnloadSound(fxShoot);
    // UnloadSound(fxEnemyHit);
    // UnloadSound(fxPlayerHit);
    // UnloadSound(fxLevelUp);
    // UnloadSound(fxGameOver);

    // Descarregar música
    // UnloadMusicStream(musicBackground);

    CloseAudioDevice(); // Fecha o dispositivo de áudio
}

void PlaySoundEffect(Sound fx) {
    // Adicionar verificação se o áudio está inicializado, se necessário
    // if (IsAudioDeviceReady()) PlaySound(fx);
}

void PlayBackgroundMusic(Music music) {
    // if (IsAudioDeviceReady()) PlayMusicStream(music);
}

void UpdateMusicStreamLocal(Music music) {
    // if (IsAudioDeviceReady()) UpdateMusicStream(music);
}

void StopBackgroundMusic(Music music) {
    // if (IsAudioDeviceReady()) StopMusicStream(music);
}

void PauseBackgroundMusic(Music music) {
    // if (IsAudioDeviceReady()) PauseMusicStream(music);
}

void ResumeBackgroundMusic(Music music) {
    // if (IsAudioDeviceReady()) ResumeMusicStream(music);
}

