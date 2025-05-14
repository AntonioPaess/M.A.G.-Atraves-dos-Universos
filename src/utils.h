#ifndef UTILS_H
#define UTILS_H

#include "raylib.h" // Essencial para bool, Vector2, Color, funções da Raylib

// Constantes de configuração da tela
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1200
#define TARGET_FPS 60

// Variável global para controlar modo de tela
extern bool isFullscreenGlobal; // Renomeado para clareza

// Função de utilidade para alternar entre modo janela e tela cheia
void AppToggleFullscreen(void); // Renomeado

#endif // UTILS_H
