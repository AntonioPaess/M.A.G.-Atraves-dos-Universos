#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "audio.h"
#include "powerup.h"
#include "boss.h"

typedef enum {
    GAME_STATE_MAIN_MENU,  
    GAME_STATE_TUTORIAL, 
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,   
    GAME_STATE_GAME_OVER
} GameState;

typedef struct {
    Player player;
    EnemyList enemies;
    Bullet *bullets;
    Bullet *enemyBullets;  // Adicionando lista de projéteis dos inimigos
    long score; // Pontuação pode ser grande
    GameState currentState;

    float enemySpawnTimer;
    float enemySpawnInterval; // Intervalo entre spawns, diminui com o tempo
    float difficultyTimer;    // Timer para aumentar a dificuldade geral
    float shootCooldown;  // Tempo entre disparos quando o botão é segurado

    // Sons
    Sound shootSound;
    Sound enemyExplodeSound;
    Sound playerExplodeSound;
    Music backgroundMusic;

    // Contador de inimigos eliminados para power-ups
    int enemiesKilled;
    int nextPowerupAt;  // Próximo milestone para mostrar power-ups (10, 20, 30, etc)

    // Lista de power-ups ativos
    Powerup *powerups;

    // Novo campo para rastrear se o jogador tem dano aumentado
    bool increasedDamage;

    // Novos campos para o boss
    Boss boss;
    bool bossActive;
    int enemiesKilledSinceBoss;
    bool showBossMessage;
    float bossMessageTimer;

} Game;

void InitGame(Game *game);
void UpdateGame(Game *game, float deltaTime);
void DrawGame(const Game *game); // Passa o jogo inteiro para renderização
void ResetGame(Game *game);     // Para reiniciar após game over

#endif // GAME_H
