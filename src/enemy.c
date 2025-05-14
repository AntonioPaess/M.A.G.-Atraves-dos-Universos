#include "enemy.h"
#include "raylib.h" // For Vector2, fabsf, sqrtf, etc.
#include <stdlib.h> // For malloc, free
#include <math.h>   // For sqrtf, atan2f, cosf, sinf if needed for more complex movement

void InitEnemyList(EnemyList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

// Modificado para aceitar Vector2 para posição
void AddEnemy(EnemyList *list, Vector2 position, float speed) {
    Enemy *newEnemy = (Enemy*)malloc(sizeof(Enemy));
    if (!newEnemy) {
        // Handle allocation failure, e.g., by logging an error or exiting
        TraceLog(LOG_ERROR, "Failed to allocate memory for new enemy.");
        return;
    }
    newEnemy->position = position;
    newEnemy->size = (Vector2){ ENEMY_SIZE, ENEMY_SIZE };
    newEnemy->speed = speed;
    newEnemy->active = true; // Assume new enemies are active
    // newEnemy->hp = ENEMY_HP; // If HP is implemented
    newEnemy->next = NULL;
    newEnemy->prev = list->tail;

    if (list->tail) {
        list->tail->next = newEnemy;
    }
    list->tail = newEnemy;

    if (!list->head) {
        list->head = newEnemy;
    }
    list->count++;
}

void RemoveEnemy(EnemyList *list, Enemy *enemy) {
    if (!list || !enemy) return;

    if (enemy->prev) {
        enemy->prev->next = enemy->next;
    }
    if (enemy->next) {
        enemy->next->prev = enemy->prev;
    }

    if (list->head == enemy) {
        list->head = enemy->next;
    }
    if (list->tail == enemy) {
        list->tail = enemy->prev;
    }

    free(enemy);
    list->count--;
}

void UpdateEnemies(EnemyList *list, const Player *player, float deltaTime, int windowWidth, int windowHeight) {
    Enemy *current = list->head;
    while (current) {
        if (!current->active) {
            current = current->next; // Skip inactive enemies
            continue;
        }

        // Calculate direction vector from enemy to player
        Vector2 direction = {
            player->position.x - current->position.x,
            player->position.y - current->position.y
        };

        // Calculate distance
        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0) { // Avoid division by zero and stop if on top of player
            // Normalize direction vector
            direction.x /= distance;
            direction.y /= distance;

            // Move enemy towards player
            current->position.x += direction.x * current->speed * deltaTime;
            current->position.y += direction.y * current->speed * deltaTime;
        }
        
        // Basic boundary check (optional, enemies might go off-screen depending on game design)
        // if (current->position.x < 0) current->position.x = 0;
        // if (current->position.y < 0) current->position.y = 0;
        // if (current->position.x + current->size.x > windowWidth) current->position.x = windowWidth - current->size.x;
        // if (current->position.y + current->size.y > windowHeight) current->position.y = windowHeight - current->size.y;

        current = current->next;
    }
}

// The drawEnemies function has been removed as per the new modular structure.
// Rendering will be handled by the render module (render.c).

