#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"
#include "enemy.h"
#include "bullet.h"

// Estrutura do boss
typedef struct {
    Vector2 position;      // Posição do boss
    Vector2 velocity;      // Velocidade do boss
    float radius;          // Raio base do boss
    int currentLayer;      // Camada atual (4 = hexágono, 3 = quadrado, 2 = círculo, 1 = mimético)
    float layerHealth;     // Saúde da camada atual
    float maxLayerHealth;  // Saúde máxima da camada atual (para cálculos de porcentagem)
    float attackTimer;     // Temporizador para ataques
    bool active;           // Se o boss está ativo
    bool isTransitioning;  // Se está em transição entre camadas
    float transitionTimer; // Temporizador de transição
    
    // Novas variáveis para a fase mimética
    bool isDashing;        // Se o boss está em dash
    Vector2 dashDirection; // Direção do dash
    float dashTimer;       // Temporizador do dash
    float dashCooldown;    // Cooldown do dash
    Vector2 targetPosition; // Posição alvo para movimentação mimética
} Boss;

// Inicializar o boss
void InitBoss(Boss *boss, Vector2 position);

// Atualizar o boss
void UpdateBoss(Boss *boss, Vector2 playerPosition, float deltaTime, Bullet **enemyBullets);

// Verificar se o boss foi atingido por um projétil
bool CheckBossHitByBullet(Boss *boss, Vector2 bulletPosition, float bulletRadius, int damage);

// Lançar projéteis que ricocheteiam quando uma camada é destruída
void LaunchRicochetBullets(Boss *boss, Bullet **enemyBullets);

// Desenhar o boss
void DrawBoss(const Boss *boss);

#endif // BOSS_H