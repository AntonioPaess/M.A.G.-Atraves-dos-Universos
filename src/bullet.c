#include "bullet.h"
#include <stdlib.h> // Para malloc, free
#include "raymath.h" // Para Vector2Normalize, Vector2Scale

void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return; // Falha na alocação

    newBullet->position = startPosition;
    // Normaliza a direção e aplica a velocidade
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED);
    } else {
        // Direção padrão se o mouse estiver no centro do jogador (ou outra lógica)
        newBullet->velocity = (Vector2){0, -BULLET_SPEED}; // Atira para cima por padrão
    }
    newBullet->radius = BULLET_RADIUS;
    newBullet->active = true;
    newBullet->next = *head;
    newBullet->damage = 1;  // Dano padrão
    *head = newBullet;
}

void UpdateBullets(Bullet **head, float deltaTime, int screenWidth, int screenHeight) {
    Bullet *current = *head;
    Bullet *prev = NULL;

    while (current != NULL) {
        if (current->active) {
            // Atualizar posição
            current->position.x += current->velocity.x * deltaTime;
            current->position.y += current->velocity.y * deltaTime;

            // Verificar colisão com as bordas para ricochete
            if (current->canRicochet && current->ricochetsLeft > 0) {
                // Verificar bordas da área circular
                extern float currentPlayAreaRadius;
                Vector2 center = {PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y};
                float distToCenter = Vector2Distance(current->position, center);
                
                if (distToCenter >= currentPlayAreaRadius - current->radius) {
                    // Calcular a normal na direção do centro
                    Vector2 normal = Vector2Normalize(Vector2Subtract(center, current->position));
                    
                    // Calcular vetor de reflexão: r = d - 2(d·n)n
                    float dotProduct = Vector2DotProduct(current->velocity, normal);
                    Vector2 reflection = Vector2Subtract(
                        current->velocity,
                        Vector2Scale(normal, 2 * dotProduct)
                    );
                    
                    // Aplicar reflexão
                    current->velocity = reflection;
                    current->ricochetsLeft--;
                    
                    // Ajustar posição para evitar ficar preso na borda
                    current->position = Vector2Add(
                        center,
                        Vector2Scale(
                            Vector2Normalize(Vector2Subtract(current->position, center)),
                            currentPlayAreaRadius - current->radius - 2.0f
                        )
                    );
                }
            }
            
            // Desativar se sair completamente da tela
            if (current->position.x + current->radius < 0 ||
                current->position.x - current->radius > screenWidth ||
                current->position.y + current->radius < 0 ||
                current->position.y - current->radius > screenHeight) {
                current->active = false;
            }
        }

        // Remover projéteis inativos
        if (!current->active) {
            Bullet *toRemove = current;
            
            if (prev == NULL) { // Cabeça da lista
                *head = current->next;
                current = *head;
            } else {
                prev->next = current->next;
                current = current->next;
            }
            
            free(toRemove);
        } else {
            prev = current;
            current = current->next;
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

// Adicione a função:
void AddBulletWithProps(Bullet **bulletsList, Vector2 position, Vector2 direction, float radius, int damage) {
    Bullet *bullet = (Bullet*)malloc(sizeof(Bullet));
    if (bullet) {
        bullet->position = position;
        bullet->velocity = Vector2Scale(direction, BULLET_SPEED);
        bullet->active = true;
        bullet->radius = radius;
        bullet->damage = damage;  // Dano personalizado
        bullet->next = *bulletsList;
        *bulletsList = bullet;
    }
}

// Adicione esta função para criar balas de ricochete
void AddRicochetBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return;

    newBullet->position = startPosition;
    // Normaliza a direção e aplica a velocidade
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED * 1.5f); // Mais rápido
    } else {
        newBullet->velocity = (Vector2){0, -BULLET_SPEED * 1.5f};
    }
    newBullet->radius = BULLET_RADIUS * 1.2f; // Maior
    newBullet->active = true;
    newBullet->damage = 1;
    newBullet->canRicochet = true;  // Pode ricocheter
    newBullet->ricochetsLeft = 1;   // Um ricochete
    newBullet->next = *head;
    *head = newBullet;
}