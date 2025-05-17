#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "game.h" // Para acessar as estruturas do jogo

// Funções de desenho
void DrawGameplay(const Player *player, const EnemyList *enemies, const Bullet *bullets, const Bullet *enemyBullets, long score);
void DrawGameOverScreen(long finalScore);
void DrawMainMenu(void); // Nova função para o menu principal
void DrawMinimalistCursor(void);

#endif // RENDER_H