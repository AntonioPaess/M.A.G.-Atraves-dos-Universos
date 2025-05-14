#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h" // Para Vector2
#include "player.h" // Para Player struct, se necessário para lógica de perseguição
#include <stdbool.h>

#define ENEMY_SIZE 32     // tamanho do inimigo
#define ENEMY_SPEED 100   // velocidade base dos inimigos
// Adicionar ENEMY_HP se os inimigos tiverem vida
// #define ENEMY_HP 1

typedef struct Enemy {
    Vector2 position;           // Posição do inimigo
    Vector2 size;               // Tamanho do inimigo
    float speed;                // Velocidade atual (pode aumentar por fase)
    // int hp;                  // Pontos de vida, se aplicável
    bool active;                // Estado do inimigo (ativo ou não)
    struct Enemy *prev, *next;  // Para lista duplamente encadeada
} Enemy;

// Lista duplamente encadeada de inimigos
typedef struct {
    Enemy *head;
    Enemy *tail;
    int count;  // Contador de inimigos ativos
} EnemyList;

// Inicializa a lista de inimigos vazia.
void InitEnemyList(EnemyList *list);

// Adiciona um inimigo à lista em posição (x, y) e com determinada velocidade.
// Modificado para aceitar Vector2 para posição
void AddEnemy(EnemyList *list, Vector2 position, float speed);

// Remove um inimigo da lista (por exemplo, quando morto ou desativado).
// A remoção pode ser lógica (marcando como inativo) ou física (liberando memória).
// Por ora, a função original remove da lista e libera memória.
void RemoveEnemy(EnemyList *list, Enemy *enemy);

// Atualiza todos os inimigos ativos: eles perseguem o jogador, etc.
void UpdateEnemies(EnemyList *list, const Player *player, float deltaTime, int windowWidth, int windowHeight);

// A função drawEnemies foi removida daqui. A renderização será feita pelo módulo render.c
// void drawEnemies(EnemyList *list); // Removido

#endif // ENEMY_H
