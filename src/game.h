#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "audio.h"
#include "powerup.h"
#include "boss.h"
#include "scoreboard.h" // Adicione esta linha para acessar SortType

#define MAX_NAME_LENGTH 50  // Definindo um tamanho máximo para o nome do jogador

typedef enum {
    GAME_STATE_MAIN_MENU,  
    GAME_STATE_TUTORIAL, 
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,   
    GAME_STATE_GAME_OVER,
    GAME_STATE_ENTER_NAME,   // Novo estado para inserir nome
    GAME_STATE_SCOREBOARD    // Novo estado para mostrar o placar
} GameState;

typedef struct Game {
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

    // Para entrada de nome
    char playerName[MAX_NAME_LENGTH];
    int nameLength;
    bool isHighScore;

    // Para rastreamento de tempo
    float gameTime;          // Tempo total de jogo
    bool showGameSummary;    // Flag para mostrar resumo do jogo antes do game over
    SortType currentSortType; // Tipo de ordenação atual no scoreboard

} Game;

// Funções de manipulação principal do jogo
void InitGame(Game *game);
void ResetGame(Game *game);
void UpdateGame(Game *game, float deltaTime);
void DrawGame(Game *game);  // Adicione esta linha

#endif // GAME_H
