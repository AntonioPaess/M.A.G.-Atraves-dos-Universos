#include "bullet.h"
#include "raylib.h" // For Vector2, etc.
#include <stdlib.h> // For malloc, free
#include <math.h>   // For vector normalization if needed, though simple direction is used here

// Adiciona um novo projétil à lista (gerenciada como lista encadeada simples).
void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet*)malloc(sizeof(Bullet));
    if (!newBullet) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for new bullet.");
        return;
    }
    newBullet->position = startPosition;
    newBullet->size = (Vector2){ BULLET_SIZE, BULLET_SIZE };
    
    // Normaliza o vetor de direção se não estiver já normalizado
    // No código original do player, last_direction já é normalizado.
    // Se a direção puder vir não normalizada, descomente o bloco abaixo.
    /*
    float dirLength = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (dirLength > 0) {
        newBullet->direction.x = direction.x / dirLength;
        newBullet->direction.y = direction.y / dirLength;
    } else {
        // Direção padrão se o vetor de entrada for (0,0), por exemplo, para cima
        newBullet->direction = (Vector2){0.0f, -1.0f};
    }
    */
    newBullet->direction = direction; // Assume que a direção já está normalizada

    newBullet->active = true;
    newBullet->next = *head; // Adiciona no início da lista
    *head = newBullet;
}

// Atualiza a posição de todos os projéteis ativos.
// Remove projéteis que saem da tela ou são marcados como inativos.
void UpdateBullets(Bullet **head, float deltaTime, int windowWidth, int windowHeight) {
    Bullet **current_ptr_to_ptr = head;
    while (*current_ptr_to_ptr != NULL) {
        Bullet *b = *current_ptr_to_ptr;

        if (b->active) {
            b->position.x += b->direction.x * BULLET_SPEED * deltaTime;
            b->position.y += b->direction.y * BULLET_SPEED * deltaTime;

            // Verifica se o projétil saiu da janela
            if (b->position.x + b->size.x < 0 || b->position.x > windowWidth ||
                b->position.y + b->size.y < 0 || b->position.y > windowHeight) {
                b->active = false; // Marca como inativo para remoção
            }
        }

        // Lógica de remoção de projéteis inativos
        if (!b->active) {
            *current_ptr_to_ptr = b->next; // Remove o projétil da lista
            free(b);
            // Não avança current_ptr_to_ptr aqui, pois o próximo elemento já está em *current_ptr_to_ptr
        } else {
            current_ptr_to_ptr = &(b->next); // Avança para o próximo ponteiro na lista
        }
    }
}

// A função drawBullets foi removida daqui. A renderização será feita pelo módulo render.c

