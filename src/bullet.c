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
    newBullet->color = WHITE;
    newBullet->next = *head;
    *head = newBullet;
}

void UpdateBullets(Bullet **head, float deltaTime, int screenWidth, int screenHeight) {
    Bullet *current = *head;
    Bullet *prev = NULL;

    while (current != NULL) {
        if (current->active) {
            current->position.x += current->velocity.x * deltaTime;
            current->position.y += current->velocity.y * deltaTime;

            // Desativar projétil se sair da tela
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
            } else {
                prev->next = current->next;
            }
            current = current->next;
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