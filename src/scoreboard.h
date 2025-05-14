#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <stdbool.h>

#define MAX_NAME_LENGTH 50
#define MAX_SCORES 5 // Para o ranking na tela de Game Over (últimos 5 melhores)

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
    int lives; // Vidas restantes no final (pode ser 0)
    float timeAlive; // Tempo de sobrevivência
    int enemiesKilled; // Total de inimigos eliminados
    int phasesWon; // Número de fases concluídas
} ScoreEntry;

// Carrega as entradas do placar de um arquivo.
// Retorna o número de entradas carregadas.
// O ponteiro 'entries' será alocado dinamicamente e deve ser liberado pelo chamador.
int LoadScoreboard(const char *filename, ScoreEntry **entries);

// Salva as entradas do placar em um arquivo.
void SaveScoreboard(const char *filename, const ScoreEntry *entries, int count);

// Adiciona uma nova entrada ao placar (em memória).
// O array 'entries' e 'count' são atualizados.
// Esta função pode precisar realocar 'entries' se o array estiver cheio.
// A ordenação (Quicksort) será aplicada antes de salvar ou exibir.
void AddScoreEntry(ScoreEntry **entries, int *count, ScoreEntry newEntry);

// Ordena as entradas do placar usando Quicksort (por pontuação, decrescente).
void SortScoreboard(ScoreEntry *entries, int count);

#endif // SCOREBOARD_H
