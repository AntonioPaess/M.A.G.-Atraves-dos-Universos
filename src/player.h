#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "utils.h" // Para PLAYER_RADIUS, PLAYER_SPEED
#include <stdbool.h>

#define INVINCIBILITY_TIME 3.0f  // Tempo de invencibilidade em segundos
#define BLINK_FREQUENCY 0.1f     // Frequência do piscar em segundos

typedef struct {
    Vector2 position;
    float radius;
    Color color;
    int lives;               // Número de vidas (começa com 3)
    bool isInvincible;       // Flag de invencibilidade
    float invincibleTimer;   // Contador para o tempo de invencibilidade
    float blinkTimer;        // Contador para o efeito de piscar
    bool visible;            // Flag para controlar o efeito de piscar
} Player;

void InitPlayer(Player *player, int windowWidth, int windowHeight);
void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight);

#endif // PLAYER_H
