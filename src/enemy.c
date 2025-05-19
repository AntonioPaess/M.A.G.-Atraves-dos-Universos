#include "enemy.h"
#include <stdlib.h> 
#include "raymath.h" 
#include <math.h>    

void InitEnemyList(EnemyList *list) {
    list->head = NULL;
    list->count = 0;
}

void AddEnemy(EnemyList *list, Vector2 position, float radius, float speed, Color color, EnemyType type) {
    Enemy *newEnemy = (Enemy *)malloc(sizeof(Enemy));
    if (!newEnemy) return;

    newEnemy->position = position;
    newEnemy->radius = radius;
    newEnemy->speed = speed;
    newEnemy->active = true;
    newEnemy->type = type;
    newEnemy->isDying = false;    
    newEnemy->deathTimer = 0.0f;  
    
    
    switch (type) {
        case ENEMY_TYPE_NORMAL:
            newEnemy->color = WHITE;
            newEnemy->health = 2;
            break;
        case ENEMY_TYPE_SPEEDER:
            newEnemy->color = SKYBLUE;
            newEnemy->health = 1;
            newEnemy->speed *= 1.5f; 
            break;
        case ENEMY_TYPE_TANK:
            newEnemy->color = GRAY;
            newEnemy->health = 3; 
            break;
        case ENEMY_TYPE_EXPLODER:
            newEnemy->color = RED;
            newEnemy->health = 2;
            break;
        case ENEMY_TYPE_SHOOTER:
            newEnemy->color = YELLOW;
            newEnemy->health = 1;
            newEnemy->shootTimer = 0.0f;
            break;
    }
    
    newEnemy->velocity = (Vector2){0,0};
    
    
    newEnemy->next = list->head;
    newEnemy->prev = NULL;
    
    
    if (list->head != NULL) {
        list->head->prev = newEnemy;
    }
    
    list->head = newEnemy;
    list->count++;
}


void UpdateNormalEnemy(Enemy *enemy, Vector2 playerPosition, float deltaTime) {
    Vector2 directionToPlayer = Vector2Subtract(playerPosition, enemy->position);
    if (Vector2LengthSqr(directionToPlayer) > 0) {
        enemy->velocity = Vector2Scale(Vector2Normalize(directionToPlayer), enemy->speed);
    } else {
        enemy->velocity = (Vector2){0,0};
    }

    enemy->position.x += enemy->velocity.x * deltaTime;
    enemy->position.y += enemy->velocity.y * deltaTime;
}


void UpdateShooterEnemy(Enemy *enemy, Vector2 playerPosition, float deltaTime, Bullet **enemyBullets) {
    Vector2 directionToPlayer = Vector2Subtract(playerPosition, enemy->position);
    float distanceToPlayer = Vector2Length(directionToPlayer);
    
    
    enemy->shootTimer += deltaTime;
    
    
    if (distanceToPlayer < 200.0f) {
        
        enemy->velocity = Vector2Scale(Vector2Normalize(directionToPlayer), -enemy->speed * 1.2f);
    } else if (distanceToPlayer > 450.0f) {
        
        enemy->velocity = Vector2Scale(Vector2Normalize(directionToPlayer), enemy->speed * 0.8f);
    } else {
        
        
        
        Vector2 sideDirection = {-directionToPlayer.y, directionToPlayer.x};
        sideDirection = Vector2Normalize(sideDirection);
        
        
        float oscillation = sinf(GetTime() * 2.0f); 
        sideDirection = Vector2Scale(sideDirection, oscillation * enemy->speed * 0.7f);
        
        
        Vector2 backDirection = Vector2Scale(Vector2Normalize(directionToPlayer), -enemy->speed * 0.3f);
        enemy->velocity = Vector2Add(sideDirection, backDirection);
    }
    
    
    if (enemy->shootTimer >= 1.0f && distanceToPlayer < 500.0f) {
        enemy->shootTimer = 0.0f;
        
        
        if (Vector2LengthSqr(directionToPlayer) > 0) {
            
            float angleVariation = GetRandomValue(-5, 5) * 0.01f; 
            float currentAngle = atan2f(directionToPlayer.y, directionToPlayer.x);
            float newAngle = currentAngle + angleVariation;
            
            Vector2 shootDirection = {
                cosf(newAngle),
                sinf(newAngle)
            };
            
            AddBullet(enemyBullets, enemy->position, shootDirection);
        }
    }
    
    
    enemy->position.x += enemy->velocity.x * deltaTime;
    enemy->position.y += enemy->velocity.y * deltaTime;
}

void UpdateEnemies(EnemyList *list, Vector2 playerPosition, float deltaTime, 
                  int screenWidth, int screenHeight, 
                  Bullet **playerBullets, Bullet **enemyBullets) {
    Enemy *currentEnemy = list->head;
    
    while (currentEnemy != NULL) {
        Enemy *nextEnemy = currentEnemy->next; 
        
        if (currentEnemy->active) {
            
            switch (currentEnemy->type) {
                case ENEMY_TYPE_SPEEDER:
                case ENEMY_TYPE_TANK:
                case ENEMY_TYPE_NORMAL:
                case ENEMY_TYPE_EXPLODER:
                    UpdateNormalEnemy(currentEnemy, playerPosition, deltaTime);
                    break;
                case ENEMY_TYPE_SHOOTER:
                    UpdateShooterEnemy(currentEnemy, playerPosition, deltaTime, enemyBullets);
                    break;
            }
            
            
            if (currentEnemy->position.x < 0) currentEnemy->position.x = 0;
            if (currentEnemy->position.y < 0) currentEnemy->position.y = 0;
            if (currentEnemy->position.x > screenWidth) currentEnemy->position.x = screenWidth;
            if (currentEnemy->position.y > screenHeight) currentEnemy->position.y = screenHeight;
        }
        else if (currentEnemy->isDying) {
            
            currentEnemy->deathTimer += deltaTime;
            
            
            if (currentEnemy->deathTimer >= DEATH_ANIMATION_DURATION) {
                
                if (currentEnemy->prev == NULL) {
                    
                    list->head = currentEnemy->next;
                    if (list->head != NULL) {
                        list->head->prev = NULL;
                    }
                } else {
                    
                    currentEnemy->prev->next = currentEnemy->next;
                    if (currentEnemy->next != NULL) {
                        currentEnemy->next->prev = currentEnemy->prev;
                    }
                }
                
                free(currentEnemy);
                list->count--;
            }
        }
        
        currentEnemy = nextEnemy;
    }
}

void RemoveEnemy(EnemyList *list, Enemy *toRemove) {
    if (!list || !toRemove) return;

    Enemy *current = list->head;
    Enemy *prev = NULL;

    while (current != NULL) {
        if (current == toRemove) {
            if (prev == NULL) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            list->count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void FreeEnemies(EnemyList *list) {
    Enemy *current = list->head;
    while (current != NULL) {
        Enemy *next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
    list->count = 0;
}