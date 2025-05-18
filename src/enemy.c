#include "enemy.h"
#include <stdlib.h> // Para malloc, free
#include "raymath.h" // Para Vector2Normalize, Vector2Scale, Vector2Subtract
#include <math.h>    // Para funções trigonométricas

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
    newEnemy->isDying = false;    // Inicializar o novo campo
    newEnemy->deathTimer = 0.0f;  // Inicializar o timer
    
    // Configurações específicas por tipo
    switch (type) {
        case ENEMY_TYPE_NORMAL:
            newEnemy->color = WHITE;
            newEnemy->health = 2;
            break;
        case ENEMY_TYPE_SPEEDER:
            newEnemy->color = SKYBLUE;
            newEnemy->health = 1;
            newEnemy->speed *= 1.5f; // 50% mais rápido
            break;
        case ENEMY_TYPE_TANK:
            newEnemy->color = GRAY;
            newEnemy->health = 3; // Aguenta 3 tiros
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
    
    // Configurar ponteiros para lista duplamente encadeada
    newEnemy->next = list->head;
    newEnemy->prev = NULL;
    
    // Configurar o ponteiro prev do antigo primeiro nó
    if (list->head != NULL) {
        list->head->prev = newEnemy;
    }
    
    list->head = newEnemy;
    list->count++;
}

// Comportamento normal: perseguir o jogador
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

// Inimigo que mantém distância e atira
void UpdateShooterEnemy(Enemy *enemy, Vector2 playerPosition, float deltaTime, Bullet **enemyBullets) {
    Vector2 directionToPlayer = Vector2Subtract(playerPosition, enemy->position);
    float distanceToPlayer = Vector2Length(directionToPlayer);
    
    // Atualizar o timer de tiro independentemente da posição
    enemy->shootTimer += deltaTime;
    
    // Lógica de movimento
    if (distanceToPlayer < 200.0f) {
        // Se estiver muito perto, afasta-se mais rapidamente
        enemy->velocity = Vector2Scale(Vector2Normalize(directionToPlayer), -enemy->speed * 1.2f);
    } else if (distanceToPlayer > 450.0f) {
        // Se estiver muito longe, aproxima-se para ficar no alcance de tiro
        enemy->velocity = Vector2Scale(Vector2Normalize(directionToPlayer), enemy->speed * 0.8f);
    } else {
        // Se estiver em uma boa distância, move-se lateralmente para dificultar o tiro do jogador
        
        // Cria um vetor perpendicular à direção do jogador para movimento lateral
        Vector2 sideDirection = {-directionToPlayer.y, directionToPlayer.x};
        sideDirection = Vector2Normalize(sideDirection);
        
        // Alterna entre mover para esquerda/direita usando o tempo como base
        float oscillation = sinf(GetTime() * 2.0f); // Movimento de oscilação
        sideDirection = Vector2Scale(sideDirection, oscillation * enemy->speed * 0.7f);
        
        // Combine movimento lateral com um leve afastamento
        Vector2 backDirection = Vector2Scale(Vector2Normalize(directionToPlayer), -enemy->speed * 0.3f);
        enemy->velocity = Vector2Add(sideDirection, backDirection);
    }
    
    // Atirar se o timer atingir o limite e estiver a uma distância razoável
    if (enemy->shootTimer >= 1.0f && distanceToPlayer < 500.0f) {
        enemy->shootTimer = 0.0f;
        
        // Atirar na direção do jogador com um leve ajuste aleatório para adicionar imperfeição
        if (Vector2LengthSqr(directionToPlayer) > 0) {
            // Pequena variação na direção do tiro para não ser 100% preciso
            float angleVariation = GetRandomValue(-5, 5) * 0.01f; // -0.05 a 0.05 radianos
            float currentAngle = atan2f(directionToPlayer.y, directionToPlayer.x);
            float newAngle = currentAngle + angleVariation;
            
            Vector2 shootDirection = {
                cosf(newAngle),
                sinf(newAngle)
            };
            
            AddBullet(enemyBullets, enemy->position, shootDirection);
        }
    }
    
    // Aplicar movimento
    enemy->position.x += enemy->velocity.x * deltaTime;
    enemy->position.y += enemy->velocity.y * deltaTime;
}

void UpdateEnemies(EnemyList *list, Vector2 playerPosition, float deltaTime, 
                  int screenWidth, int screenHeight, 
                  Bullet **playerBullets, Bullet **enemyBullets) {
    Enemy *currentEnemy = list->head;
    
    while (currentEnemy != NULL) {
        Enemy *nextEnemy = currentEnemy->next; // Guardar próximo antes de potencialmente remover
        
        if (currentEnemy->active) {
            // Comportamentos específicos por tipo
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
            
            // Manter inimigos dentro da tela
            if (currentEnemy->position.x < 0) currentEnemy->position.x = 0;
            if (currentEnemy->position.y < 0) currentEnemy->position.y = 0;
            if (currentEnemy->position.x > screenWidth) currentEnemy->position.x = screenWidth;
            if (currentEnemy->position.y > screenHeight) currentEnemy->position.y = screenHeight;
        }
        else if (currentEnemy->isDying) {
            // Atualizar timer de animação de morte
            currentEnemy->deathTimer += deltaTime;
            
            // Quando a animação terminar, remover o inimigo
            if (currentEnemy->deathTimer >= DEATH_ANIMATION_DURATION) {
                // Remover o inimigo da lista
                if (currentEnemy->prev == NULL) {
                    // É o primeiro da lista
                    list->head = currentEnemy->next;
                    if (list->head != NULL) {
                        list->head->prev = NULL;
                    }
                } else {
                    // Não é o primeiro
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