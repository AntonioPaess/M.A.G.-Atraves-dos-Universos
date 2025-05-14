#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "scoreboard.h" // Adicionado para acessar ScoreEntry

// Funções de inicialização e finalização de recursos de renderização (fontes, texturas, etc.)
void InitRenderResources(void);
void UnloadRenderResources(void);

// Funções de desenho principais
void DrawGame(const Player *player, const EnemyList *enemies, const Bullet *bullets, int score, int level, int lives, int enemiesRemaining);
void DrawMainMenu(void); // Exemplo, se houver menu
void DrawGameOverScreen(int finalScore, ScoreEntry *ranking, int rankingSize);
void DrawVictoryScreen(int finalScore);
void DrawHUD(int lives, int score, int level, int enemiesRemaining);

#endif // RENDER_H
