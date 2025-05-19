#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "audio.h"
#include "powerup.h"
#include "boss.h"
#include "scoreboard.h" 

#define MAX_NAME_LENGTH 50  

typedef enum {
    GAME_STATE_MAIN_MENU,  
    GAME_STATE_TUTORIAL, 
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,   
    GAME_STATE_GAME_OVER,
    GAME_STATE_ENTER_NAME,   
    GAME_STATE_SCOREBOARD    
} GameState;

typedef struct Game {
    Player player;
    EnemyList enemies;
    Bullet *bullets;
    Bullet *enemyBullets;  
    long score; 
    GameState currentState;

    float enemySpawnTimer;
    float enemySpawnInterval; 
    float difficultyTimer;    
    float shootCooldown;  

    
    Sound shootSound;
    Sound enemyExplodeSound;
    Sound playerExplodeSound;
    Music backgroundMusic;

    
    int enemiesKilled;
    int nextPowerupAt;  

    
    Powerup *powerups;

    
    bool increasedDamage;

    
    Boss boss;
    bool bossActive;
    int enemiesKilledSinceBoss;
    bool showBossMessage;
    float bossMessageTimer;

    
    char playerName[MAX_NAME_LENGTH];
    int nameLength;
    bool isHighScore;

    
    float gameTime;          
    bool showGameSummary;    
    SortType currentSortType; 

} Game;


void InitGame(Game *game);
void ResetGame(Game *game);
void UpdateGame(Game *game, float deltaTime);
void DrawGame(Game *game);  

#endif 
