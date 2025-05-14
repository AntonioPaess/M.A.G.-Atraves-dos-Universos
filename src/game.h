#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "scoreboard.h"

// Enum para os estados do jogo
typedef enum {
    GAME_STATE_MAIN_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER,
    GAME_STATE_VICTORY,
    GAME_STATE_RANKING // Adicionado para exibir ranking após game over
} GameState;

// Estrutura para o estado geral do jogo
typedef struct {
    Player player;
    EnemyList enemies;
    Bullet *bullets;
    int score;
    int currentLevel;
    int totalLevels;
    bool newLevelStarted;
    GameState currentState;
    // Adicionar outros campos de estado do jogo conforme necessário
    // Por exemplo: timer, contagem de inimigos para o nível, etc.
    int enemiesToSpawnThisLevel;
    int enemiesRemainingInLevel;
    ScoreEntry ranking[5]; // Para armazenar o top 5 do ranking
    int rankingCount;      // Número de entradas válidas no ranking
    char playerNameInput[50]; // Para entrada do nome no ranking
    int charCountPlayerName; // Contador de caracteres para o nome
} Game;

// Funções de gerenciamento do jogo
void InitGame(Game *game);          // Inicializa o estado do jogo
void UpdateGame(Game *game, float deltaTime); // Atualiza a lógica do jogo
void ResetGame(Game *game);         // Reinicia o jogo para um novo playthrough
void StartNextLevel(Game *game);
void LoadRanking(Game *game);
void SavePlayerScore(Game *game);

#endif // GAME_H
