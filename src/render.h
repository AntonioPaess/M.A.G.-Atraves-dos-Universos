#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "powerup.h" 
#include "boss.h" // Adicione esta linha

// Funções auxiliares de desenho para uso externo
void DrawPixelRect(float x, float y, float width, float height, Color color);
void DrawPixelCircleV(Vector2 center, float radius, Color color);
void DrawPixelLine(float x1, float y1, float x2, float y2, Color color);
void DrawPlayAreaBorder(void);

// Funções de desenho
void DrawGameplay(const Player *player, const EnemyList *enemies, const Bullet *bullets, const Bullet *enemyBullets, const Powerup *powerups, long score);
void DrawGameOverScreen(long finalScore);
void DrawMainMenu(void);
void DrawMinimalistCursor(void);
void DrawEnemyDeathAnimation(const Enemy *enemy);
void DrawTutorialScreen(void);
void DrawBoss(const Boss *boss); // Adicione esta declaração
void DrawPauseMenu(void); // Adicione esta linha

#endif // RENDER_H