#include "audio.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>  // Adicionado para função printf

void LoadGameAudio(Sound *shoot, Sound *enemyExplode, Sound *playerExplode, 
                 Sound *enemyNormalDeath, Sound *enemyTankDeath, 
                 Sound *enemyExploderDeath, Sound *enemyShooterDeath,
                 Sound *dashSound, // Novo parâmetro
                 Music *bgMusic, Music *menuMusic, Music *tutorialMusic, 
                 Music *pauseMusic, Music *gameOverMusic, Music *nameEntryMusic,
                 Music *bossMusic, // Novo parâmetro
                 Sound *menuClick, Sound *powerupDamageSound, Sound *powerupHealSound, Sound *powerupShieldSound) {
    // Inicializar o sistema de áudio
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
        printf("Dispositivo de áudio inicializado\n");
    }
    
    // Inicializar todos os sons com uma estrutura vazia primeiro
    *shoot = (Sound){0};
    *enemyExplode = (Sound){0};
    *playerExplode = (Sound){0};
    *enemyNormalDeath = (Sound){0};
    *enemyTankDeath = (Sound){0};
    *enemyExploderDeath = (Sound){0};
    *enemyShooterDeath = (Sound){0};
    *menuClick = (Sound){0};
    *powerupDamageSound = (Sound){0};
    *powerupHealSound = (Sound){0};
    *powerupShieldSound = (Sound){0};
    *dashSound = (Sound){0}; // Inicializar o som do dash
    
    // Inicializar todas as músicas com uma estrutura vazia primeiro
    *bgMusic = (Music){0};
    *menuMusic = (Music){0};
    *tutorialMusic = (Music){0};
    *pauseMusic = (Music){0};
    *gameOverMusic = (Music){0};
    *nameEntryMusic = (Music){0};
    *bossMusic = (Music){0}; // Inicializar a música do boss
    
    // Carregar efeitos sonoros básicos
    *shoot = LoadSound("assets/sounds/Player/Tiro.wav");
    *enemyExplode = LoadSound("assets/sounds/Inimigo/Explode.wav");
    *playerExplode = LoadSound("assets/sounds/Player/Sofrer dano.wav");
    *dashSound = LoadSound("assets/sounds/Player/Dash.wav"); // Carregando o som do dash
    
    // Carregar sons específicos para cada tipo de inimigo
    *enemyNormalDeath = LoadSound("assets/sounds/Inimigo/MorteSimples.wav");
    *enemyTankDeath = LoadSound("assets/sounds/Inimigo/MorteTanker.wav");
    *enemyExploderDeath = LoadSound("assets/sounds/Inimigo/Explode.wav");
    *enemyShooterDeath = LoadSound("assets/sounds/Inimigo/Shooter.wav");
    
    // Carregar sons da interface
    *menuClick = LoadSound("assets/music/Click_Menu.wav");
    
    // Carregar sons diferentes para cada powerup
    *powerupDamageSound = LoadSound("assets/sounds/Power up/O vermelho.wav");
    *powerupHealSound = LoadSound("assets/sounds/Power up/Livup.wav");
    *powerupShieldSound = LoadSound("assets/sounds/Power up/Shield.wav");
    
    // Carregar músicas para os diferentes estados
    *bgMusic = LoadMusicStream("assets/music/Soundtrack Options/Loop 2.wav");
    *menuMusic = LoadMusicStream("assets/music/Soundtrack Options/Space Station Intro.wav");
    *tutorialMusic = LoadMusicStream("assets/music/Soundtrack Options/Space Station Intro.wav");
    *pauseMusic = LoadMusicStream("assets/music/Soundtrack Options/Loop 2.wav");
    *gameOverMusic = LoadMusicStream("assets/music/Soundtrack Options/Loop 2.wav");
    *nameEntryMusic = LoadMusicStream("assets/music/Soundtrack Options/Loop 2.wav");
    
    // Carregar música do boss
    *bossMusic = LoadMusicStream("assets/music/Soundtrack Options/Fast and Furious BG loop by Dirtyflint.wav");
    
    // Verificar e configurar volumes (adicionando verificações de segurança)
    printf("Configurando volumes de sons...\n");
    
    // Configurar volumes das músicas de forma segura
    if (menuMusic->ctxData != NULL) {
        SetMusicVolume(*menuMusic, 0.5f);
        printf("Volume da música do menu configurado\n");
    } else {
        printf("AVISO: Música do menu não carregada corretamente!\n");
    }
    
    if (bgMusic->ctxData != NULL) SetMusicVolume(*bgMusic, 0.4f);
    if (tutorialMusic->ctxData != NULL) SetMusicVolume(*tutorialMusic, 0.5f);
    if (pauseMusic->ctxData != NULL) SetMusicVolume(*pauseMusic, 0.3f);
    if (gameOverMusic->ctxData != NULL) SetMusicVolume(*gameOverMusic, 0.4f);
    if (nameEntryMusic->ctxData != NULL) SetMusicVolume(*nameEntryMusic, 0.4f);
    if (bossMusic->ctxData != NULL) SetMusicVolume(*bossMusic, 0.85f); // Volume aumentado para 85%
    
    // Configurar volumes dos sons de forma segura
    if (shoot->frameCount > 0) SetSoundVolume(*shoot, 0.7f);
    if (enemyExplode->frameCount > 0) SetSoundVolume(*enemyExplode, 0.8f);
    if (playerExplode->frameCount > 0) SetSoundVolume(*playerExplode, 1.0f);
    if (menuClick->frameCount > 0) SetSoundVolume(*menuClick, 0.6f);
    if (dashSound->frameCount > 0) SetSoundVolume(*dashSound, 0.7f);
    
    // Configurar volumes dos powerups
    if (powerupDamageSound->frameCount > 0) SetSoundVolume(*powerupDamageSound, 0.8f);
    if (powerupHealSound->frameCount > 0) SetSoundVolume(*powerupHealSound, 0.8f);
    if (powerupShieldSound->frameCount > 0) SetSoundVolume(*powerupShieldSound, 0.8f);
    
    // Configurar volumes dos sons de morte dos inimigos de forma segura
    if (enemyNormalDeath->frameCount > 0) SetSoundVolume(*enemyNormalDeath, 0.8f);
    if (enemyTankDeath->frameCount > 0) SetSoundVolume(*enemyTankDeath, 0.8f);
    if (enemyExploderDeath->frameCount > 0) SetSoundVolume(*enemyExploderDeath, 0.8f);
    if (enemyShooterDeath->frameCount > 0) SetSoundVolume(*enemyShooterDeath, 0.8f);
    
    printf("Configuração de áudio concluída\n");
}

void UnloadGameAudio(Sound shoot, Sound enemyExplode, Sound playerExplode,
                    Sound enemyNormalDeath, Sound enemyTankDeath, 
                    Sound enemyExploderDeath, Sound enemyShooterDeath,
                    Sound dashSound, // Novo parâmetro
                    Music bgMusic, Music menuMusic, Music tutorialMusic, 
                    Music pauseMusic, Music gameOverMusic, Music nameEntryMusic,
                    Music bossMusic, // Novo parâmetro
                    Sound menuClick, Sound powerupDamageSound, Sound powerupHealSound, Sound powerupShieldSound) {
    // Descarregar sons básicos
    UnloadSound(shoot);
    UnloadSound(enemyExplode);
    UnloadSound(playerExplode);
    
    // Descarregar sons específicos de inimigos
    UnloadSound(enemyNormalDeath);
    UnloadSound(enemyTankDeath);
    UnloadSound(enemyExploderDeath);
    UnloadSound(enemyShooterDeath);
    
    // Descarregar todas as músicas
    UnloadMusicStream(bgMusic);
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(tutorialMusic);
    UnloadMusicStream(pauseMusic);
    UnloadMusicStream(gameOverMusic);
    UnloadMusicStream(nameEntryMusic);
    UnloadMusicStream(bossMusic); // Adicione esta linha
    
    // Descarregar também os sons dos powerups
    UnloadSound(menuClick);
    UnloadSound(powerupDamageSound);
    UnloadSound(powerupHealSound);
    UnloadSound(powerupShieldSound);
    UnloadSound(dashSound); // Descarregar o som do dash
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
