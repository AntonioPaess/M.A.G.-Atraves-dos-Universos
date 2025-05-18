#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

// Macro para encontrar o maior de dois valores
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Constantes de configuração da tela
#define SCREEN_WIDTH 1700
#define SCREEN_HEIGHT 1000
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

#define PLAY_AREA_MARGIN 50.0f  // Margem da área jogável em pixels
extern float PLAY_AREA_LEFT;
extern float PLAY_AREA_TOP;
extern float PLAY_AREA_RIGHT;
extern float PLAY_AREA_BOTTOM;

// Definições para área de jogo circular
#define PLAY_AREA_CENTER_X (SCREEN_WIDTH / 2.0f)
#define PLAY_AREA_CENTER_Y (SCREEN_HEIGHT / 2.0f)
#define PLAY_AREA_RADIUS (fminf(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f - PLAY_AREA_MARGIN)

// Função para interpolação linear
float LerpUtil(float start, float end, float t);

// Atualiza as variáveis de tamanho de tela
void UpdateScreenSizeVars(void);

// Função para verificar se um ponto está dentro da área de jogo circular
bool IsPointInPlayArea(Vector2 point);

#define MAX_ENEMIES 15  // Define o número máximo de inimigos na tela ao mesmo tempo

// Inicializa as variáveis da área de jogo
void InitPlayArea(void);

// Função para atualizar a área jogável dinamicamente
void UpdateDynamicPlayArea(float deltaTime, float gameScore); // Mantemos o parâmetro para compatibilidade

#endif // UTILS_H