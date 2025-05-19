#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "raylib.h"

#define MAX_SCORES 10        // Número máximo de pontuações no placar
#define MAX_NAME_LENGTH 50   // Tamanho máximo do nome do jogador

// Tipo de ordenação para o scoreboard
typedef enum {
    SORT_BY_SCORE,   // Ordenar por pontuação
    SORT_BY_KILLS,   // Ordenar por inimigos mortos
    SORT_BY_TIME     // Ordenar por tempo (menor primeiro)
} SortType;

typedef struct {
    char name[MAX_NAME_LENGTH];  // Nome do jogador
    long score;                  // Pontuação
    int kills;                   // Inimigos mortos
    float gameTime;              // Tempo de jogo em segundos
    bool isNew;                  // Indica se é uma entrada nova
} ScoreEntry;

// Funções para manipular o scoreboard
void InitScoreboard(void);
void AddScore(const char *name, long score, int kills, float gameTime);
void SaveScoreboard(void);
void LoadScoreboard(void);
void SortScoreboard(SortType sortType);
void DrawScoreboard(SortType sortType);
void DrawGameSummary(long score, int kills, float gameTime);
bool IsHighScore(long score);
bool IsHighKills(int kills);
bool IsFastTime(float gameTime);
void ResetNewFlags(void);

// Funções auxiliares para obter informações
int GetScoreCount(void);
ScoreEntry GetScoreAt(int index);
const char* FormatTime(float seconds); // Adicione esta linha

#endif // SCOREBOARD_H