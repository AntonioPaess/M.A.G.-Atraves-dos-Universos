#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "utils.h" // Para raios e velocidades de inimigo
#include "bullet.h" // Para permitir que inimigos atirem
#include <stdbool.h>

// Tipos de inimigos
typedef enum {
    ENEMY_TYPE_NORMAL,   // Inimigo padrão
    ENEMY_TYPE_SPEEDER,  // Pequeno e rápido
    ENEMY_TYPE_TANK,     // Grande e resistente
    ENEMY_TYPE_EXPLODER, // Explode em projéteis
    ENEMY_TYPE_SHOOTER   // Mantém distância e dispara
} EnemyType;

typedef struct Enemy {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    Color color;
    float speed;
    EnemyType type;      // Tipo do inimigo
    int health;          // Pontos de vida
    float shootTimer;    // Para inimigos atiradores
    int dodgeCount;      // Para inimigos que desviam
    struct Enemy *next;
} Enemy;

typedef struct {
    Enemy *head;
    int count;
} EnemyList;

void InitEnemyList(EnemyList *list);
void AddEnemy(EnemyList *list, Vector2 position, float radius, float speed, Color color, EnemyType type);
void UpdateEnemies(EnemyList *list, Vector2 playerPosition, float deltaTime, int screenWidth, int screenHeight, Bullet **playerBullets, Bullet **enemyBullets);
void DrawEnemies(const EnemyList *list);
void RemoveEnemy(EnemyList *list, Enemy *toRemove);
void FreeEnemies(EnemyList *list);

#endif // ENEMY_H