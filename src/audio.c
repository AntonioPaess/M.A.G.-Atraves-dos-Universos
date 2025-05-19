#include "audio.h"
#include "raylib.h"
#include <stdlib.h> 




void LoadGameAudio(Sound *shoot, Sound *enemyExplode, Sound *playerExplode, Music *bgMusic) {
    
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }
    
    
    
    *shoot = (Sound){0};
    *enemyExplode = (Sound){0};
    *playerExplode = (Sound){0};
    *bgMusic = (Music){0};
}

void UnloadGameAudio(Sound shoot, Sound enemyExplode, Sound playerExplode, Music bgMusic) {
    
    
    
}

void PlayGameSound(Sound sound) {
    
    
    if (IsAudioDeviceReady() && sound.frameCount > 0) {
        PlaySound(sound);
    }
}

void UpdateGameMusicStream(Music music) {
    
    
    if (IsAudioDeviceReady() && music.ctxData != NULL) {
        UpdateMusicStream(music);
    }
}
