#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "powerup.h" 
#include "boss.h" 
#include "game.h"


void DrawPixelRect(float x, float y, float width, float height, Color color);
void DrawPixelCircleV(Vector2 center, float radius, Color color);
void DrawPixelLine(float x1, float y1, float x2, float y2, Color color);
void DrawPlayAreaBorder(void);


void DrawGameplay(const Player *player, const EnemyList *enemies, const Bullet *bullets, const Bullet *enemyBullets, const Powerup *powerups, long score);
void DrawGameOverScreen(long finalScore);
void DrawMainMenu(void);
void DrawMinimalistCursor(void);
void DrawEnemyDeathAnimation(const Enemy *enemy);
void DrawTutorialScreen(void);
void DrawBoss(const Boss *boss);
void DrawPauseMenu(void);
void DrawGameSummary(long score, int kills, float gameTime);
void RenderScoreboardScreen(void);
void DrawNameEntryScreen(Game *game);

#endif 