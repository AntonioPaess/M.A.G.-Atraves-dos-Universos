#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

// Constantes de configuração da tela
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1200
#define TARGET_FPS 60

// Variável global para controlar modo de tela
extern bool isFullscreenGlobal;

// Função de utilidade para alternar entre modo janela e tela cheia
void AppToggleFullscreen(void);

// Constantes para o novo jogo estilo "O"
#define PLAYER_RADIUS 15.0f
#define ENEMY_RADIUS_MIN 10.0f
#define ENEMY_RADIUS_MAX 30.0f
#define BULLET_RADIUS 5.0f
#define PLAYER_SPEED 300.0f
#define BULLET_SPEED 700.0f
#define ENEMY_SPEED_MIN 100.0f
#define ENEMY_SPEED_MAX 250.0f

#endif // UTILS_H