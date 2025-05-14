#ifndef BULLET_H
#define BULLET_H

#include "raylib.h" // For Vector2
#include <stdbool.h>

#define BULLET_SPEED 400  // Velocidade dos projéteis (pixels por segundo)
#define BULLET_SIZE 8     // Tamanho do projétil (largura/altura)

typedef struct Bullet {
    Vector2 position;     // Posição do projétil (top-left)
    Vector2 size;         // Tamanho do projétil
    Vector2 direction;    // Direção normalizada do movimento do projétil
    bool active;          // Estado do projétil (ativo ou a ser removido)
    struct Bullet *next;  // Próximo projétil na lista encadeada simples
} Bullet;

// Adiciona um novo projétil à lista (gerenciada como lista encadeada simples).
// O ponteiro para o head da lista é passado para que possa ser modificado.
void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction);

// Atualiza a posição de todos os projéteis ativos.
// Remove projéteis que saem da tela ou são marcados como inativos.
// Retorna true se algum projétil foi removido (para possível otimização de loop de colisão).
void UpdateBullets(Bullet **head, float deltaTime, int windowWidth, int windowHeight);

// A função drawBullets foi removida daqui. A renderização será feita pelo módulo render.c
// void drawBullets(Bullet *head); // Removido

#endif // BULLET_H
