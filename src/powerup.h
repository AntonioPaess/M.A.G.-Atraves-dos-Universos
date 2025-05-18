#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"

// Tipos de power-ups
typedef enum {
    POWERUP_DAMAGE,   // Aumenta dano (e reduz vida)
    POWERUP_HEAL,     // Recupera todas as vidas
    POWERUP_SHIELD    // Novo: Cria um escudo que protege de um hit
} PowerupType;

// Estrutura para power-ups
typedef struct Powerup {
    Vector2 position;
    float radius;
    PowerupType type;
    bool active;
    float lifeTime;    // Tempo de duração do power-up na tela
    struct Powerup *next;
} Powerup;

// Função para inicializar a lista de power-ups
void InitPowerups(Powerup **powerups);

// Função para adicionar um novo power-up
void AddPowerup(Powerup **powerups, Vector2 position, PowerupType type);

// Função para atualizar os power-ups (tempo de vida, etc)
void UpdatePowerups(Powerup **powerups, float deltaTime);

// Função para limpar todos os power-ups
void ClearPowerups(Powerup **powerups);

// Função para verificar colisão com power-ups
bool CheckPowerupCollision(Powerup **powerups, Vector2 position, float radius, PowerupType *collectedType);

#endif // POWERUP_H