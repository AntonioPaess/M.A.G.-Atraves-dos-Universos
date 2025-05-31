#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"


#define MAX(a, b) ((a) > (b) ? (a) : (b))


#define SCREEN_WIDTH 1700
#define SCREEN_HEIGHT 1000
#define TARGET_FPS 60


extern bool isFullscreenGlobal;


void AppToggleFullscreen(void);


#define PLAYER_RADIUS 15.0f
#define ENEMY_RADIUS_MIN 10.0f
#define ENEMY_RADIUS_MAX 30.0f
#define BULLET_RADIUS 5.0f
#define PLAYER_SPEED 300.0f
#define BULLET_SPEED 700.0f
#define ENEMY_SPEED_MIN 100.0f
#define ENEMY_SPEED_MAX 250.0f

#define PLAY_AREA_MARGIN 50.0f  
extern float PLAY_AREA_LEFT;
extern float PLAY_AREA_TOP;
extern float PLAY_AREA_RIGHT;
extern float PLAY_AREA_BOTTOM;


#define PLAY_AREA_CENTER_X (SCREEN_WIDTH / 2.0f)
extern float PLAY_AREA_CENTER_Y;  // Declaração de variável global
#define PLAY_AREA_RADIUS (fminf(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f - PLAY_AREA_MARGIN)


float LerpUtil(float start, float end, float t);


void UpdateScreenSizeVars(void);


bool IsPointInPlayArea(Vector2 point);

#define MAX_ENEMIES 15  


void InitPlayArea(void);


void UpdateDynamicPlayArea(float deltaTime, float gameScore); 

#endif