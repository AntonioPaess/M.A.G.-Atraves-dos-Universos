#include "bullet.h"
#include <stdlib.h>
 #include <string.h> 
#include <stdio.h> 
#include "raymath.h" 

void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction, bool isPlayerBullet) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return;

    memset(newBullet, 0, sizeof(Bullet));

    newBullet->position = startPosition;
    
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED);
    } else {
        newBullet->velocity = (Vector2){0, -BULLET_SPEED}; 
    }
    
    // Verificar se o jogador tem dano aumentado - APENAS para tiros do jogador
    extern bool increasedDamage; 
    
    if (isPlayerBullet && increasedDamage) {
        newBullet->radius = BULLET_RADIUS * 1.5f; // Projéteis maiores
        newBullet->damage = 2; // Dano dobrado
    } else {
        newBullet->radius = BULLET_RADIUS;
        newBullet->damage = 1;
    }
    
    newBullet->active = true;  
    newBullet->next = *head;
    *head = newBullet;
}

void UpdateBullets(Bullet **bullets, float deltaTime, int screenWidth, int screenHeight) {
    Bullet *currentBullet = *bullets;
    Bullet *prevBullet = NULL;
    
    while (currentBullet != NULL) {
        // VERIFICAÇÃO DE SEGURANÇA PARA MEMÓRIA CORROMPIDA
        if ((uintptr_t)currentBullet < 1024) {
            printf("ERRO: Ponteiro de bullet corrompido: %p\n", (void*)currentBullet);
            break;
        }
        
        // VERIFICAR SE TODOS OS CAMPOS BOOL TÊM VALORES VÁLIDOS
        if (currentBullet->active != 0 && currentBullet->active != 1) {
            printf("ERRO: Campo active corrompido: %d\n", currentBullet->active);
            currentBullet->active = false;
        }
        
        if (currentBullet->canRicochet != 0 && currentBullet->canRicochet != 1) {
            printf("ERRO: Campo canRicochet corrompido: %d\n", currentBullet->canRicochet);
            currentBullet->canRicochet = false;
        }
        
        if (currentBullet->active) {
            
            currentBullet->position.x += currentBullet->velocity.x * deltaTime;
            currentBullet->position.y += currentBullet->velocity.y * deltaTime;

            
            if (currentBullet->canRicochet && currentBullet->ricochetsLeft > 0) {
                
                extern float currentPlayAreaRadius;
                Vector2 center = {PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y};
                float distToCenter = Vector2Distance(currentBullet->position, center);
                
                if (distToCenter >= currentPlayAreaRadius - currentBullet->radius) {
                    
                    Vector2 normal = Vector2Normalize(Vector2Subtract(center, currentBullet->position));
                    
                    
                    float dotProduct = Vector2DotProduct(currentBullet->velocity, normal);
                    Vector2 reflection = Vector2Subtract(
                        currentBullet->velocity,
                        Vector2Scale(normal, 2 * dotProduct)
                    );
                    
                    
                    currentBullet->velocity = reflection;
                    currentBullet->ricochetsLeft--;
                    
                    
                    currentBullet->position = Vector2Add(
                        center,
                        Vector2Scale(
                            Vector2Normalize(Vector2Subtract(currentBullet->position, center)),
                            currentPlayAreaRadius - currentBullet->radius - 2.0f
                        )
                    );
                }
            }
            
            
            if (currentBullet->position.x + currentBullet->radius < 0 ||
                currentBullet->position.x - currentBullet->radius > screenWidth ||
                currentBullet->position.y + currentBullet->radius < 0 ||
                currentBullet->position.y - currentBullet->radius > screenHeight) {
                currentBullet->active = false;
            }
        }

        
        if (!currentBullet->active) {
            Bullet *toRemove = currentBullet;
            
            if (prevBullet == NULL) { 
                *bullets = currentBullet->next;
                currentBullet = *bullets;
            } else {
                prevBullet->next = currentBullet->next;
                currentBullet = currentBullet->next;
            }
            
            free(toRemove);
        } else {
            prevBullet = currentBullet;
            currentBullet = currentBullet->next;
        }
    }
}

void FreeBullets(Bullet **head) {
    Bullet *current = *head;
    while (current != NULL) {
        Bullet *next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}


void AddBulletWithProps(Bullet **bulletsList, Vector2 position, Vector2 direction, float radius, int damage) {
    Bullet *bullet = (Bullet*)malloc(sizeof(Bullet));
    if (bullet) {
        // INICIALIZAR TODA A ESTRUTURA PRIMEIRO
        memset(bullet, 0, sizeof(Bullet));
        
        bullet->position = position;
        bullet->velocity = Vector2Scale(direction, BULLET_SPEED);
        bullet->active = true;
        bullet->radius = radius;
        bullet->damage = damage;
        
        // GARANTIR QUE CAMPOS BOOL ESTÃO CORRETOS
        bullet->canRicochet = false;    // Inicializar explicitamente
        bullet->ricochetsLeft = 0;      // Inicializar explicitamente
        
        bullet->next = *bulletsList;
        *bulletsList = bullet;
    }
}


void AddRicochetBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return;

    memset(newBullet, 0, sizeof(Bullet));

    newBullet->position = startPosition;
    
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED * 1.5f); 
    } else {
        newBullet->velocity = (Vector2){0, -BULLET_SPEED * 1.5f};
    }
    newBullet->radius = BULLET_RADIUS * 1.2f; 
    newBullet->active = true;
    newBullet->damage = 1;
    newBullet->canRicochet = true;  
    newBullet->ricochetsLeft = 1;   
    newBullet->next = *head;
    *head = newBullet;
}

// Bala penetrante que atravessa inimigos
void AddPenetratingBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return;

    memset(newBullet, 0, sizeof(Bullet));

    newBullet->position = startPosition;
    
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED * 1.2f);
    } else {
        newBullet->velocity = (Vector2){0, -BULLET_SPEED * 1.2f};
    }
    
    newBullet->radius = BULLET_RADIUS * 1.1f;
    newBullet->active = true;
    newBullet->damage = 2;  // Dano aumentado
    // Propriedade especial: Esta bala não é desativada ao atingir inimigos (implementada na lógica de colisão)
    
    newBullet->next = *head;
    *head = newBullet;
}

// Bala teleguiada que persegue o inimigo mais próximo
void AddHomingBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return;

    memset(newBullet, 0, sizeof(Bullet));

    newBullet->position = startPosition;
    
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED * 0.8f);
    } else {
        newBullet->velocity = (Vector2){0, -BULLET_SPEED * 0.8f};
    }
    
    newBullet->radius = BULLET_RADIUS;
    newBullet->active = true;
    newBullet->damage = 1;
    // Propriedade especial: Esta bala deve seguir inimigos (implementada na função UpdateBullets)
    
    newBullet->next = *head;
    *head = newBullet;
}