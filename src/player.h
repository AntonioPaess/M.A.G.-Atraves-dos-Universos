#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "utils.h" // Para PLAYER_RADIUS, PLAYER_SPEED
#include <stdbool.h>

#define INVINCIBILITY_TIME 3.0f  // Tempo de invencibilidade em segundos
#define BLINK_FREQUENCY 0.1f     // Frequência do piscar em segundos
#define DASH_DURATION 0.25f      // Duração do dash em segundos
#define DASH_COOLDOWN 5.0f       // Cooldown entre dashes
#define DASH_SPEED 800.0f        // Velocidade do dash

typedef struct {
    Vector2 position;
    float radius;
    Color color;
    int lives;               // Número de vidas (começa com 3)
    bool isInvincible;       // Flag de invencibilidade
    float invincibleTimer;   // Contador para o tempo de invencibilidade
    float blinkTimer;        // Contador para o efeito de piscar
    bool visible;            // Flag para controlar o efeito de piscar
    bool hasShield;          // Propriedade para o escudo
    float shieldTimer;       // Tempo de duração do escudo
    
    // Novas propriedades para o dash
    bool isDashing;          // Se o jogador está em estado de dash
    float dashTimer;         // Quanto tempo resta do dash atual
    float dashCooldown;      // Tempo até poder usar dash novamente
    Vector2 dashDirection;   // Direção do dash
} Player;

void InitPlayer(Player *player, int windowWidth, int windowHeight);
void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight);

#endif // PLAYER_H
