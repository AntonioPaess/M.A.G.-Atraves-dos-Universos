#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "audio.h" // Para sons

typedef enum {
    GAME_STATE_MAIN_MENU,  // Novo estado para o menu principal
    GAME_STATE_PLAYING,
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

    // Sons
    Sound shootSound;
    Sound enemyExplodeSound;
    Sound playerExplodeSound;
    Music backgroundMusic;

} Game;

void InitGame(Game *game);
void UpdateGame(Game *game, float deltaTime);
void DrawGame(const Game *game); // Passa o jogo inteiro para renderização
void ResetGame(Game *game);     // Para reiniciar após game over

#endif // GAME_H
