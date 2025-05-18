#include "powerup.h"
#include <stdlib.h>
#include <math.h>  // Adicione esta linha para usar sqrtf()

// Inicializa a lista de power-ups como vazia
void InitPowerups(Powerup **powerups) {
    *powerups = NULL;
}

// Adiciona um novo power-up à lista
void AddPowerup(Powerup **powerups, Vector2 position, PowerupType type) {
    Powerup *newPowerup = (Powerup *)malloc(sizeof(Powerup));
    if (newPowerup) {
        newPowerup->position = position;
        newPowerup->type = type;
        newPowerup->radius = 15.0f;  // Tamanho padrão do power-up
        newPowerup->active = true;
        newPowerup->lifeTime = 10.0f;  // 10 segundos de vida na tela
        newPowerup->next = *powerups;
        *powerups = newPowerup;
    }
}

// Atualiza os power-ups (reduz tempo de vida, remove se necessário)
void UpdatePowerups(Powerup **powerups, float deltaTime) {
    Powerup *current = *powerups;
    Powerup *prev = NULL;
    
    while (current != NULL) {
        // Reduzir tempo de vida
        current->lifeTime -= deltaTime;
        
        // Remover se o tempo acabou
        if (current->lifeTime <= 0.0f || !current->active) {
            Powerup *toRemove = current;
            
            if (prev == NULL) {
                // É o primeiro da lista
                *powerups = current->next;
                current = *powerups;
            } else {
                // Não é o primeiro
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

// Limpa todos os power-ups da lista
void ClearPowerups(Powerup **powerups) {
    Powerup *current = *powerups;
    while (current != NULL) {
        Powerup *next = current->next;
        free(current);
        current = next;
    }
    *powerups = NULL;
}

// Verifica colisão com power-ups
bool CheckPowerupCollision(Powerup **powerups, Vector2 position, float radius, PowerupType *collectedType) {
    Powerup *current = *powerups;
    Powerup *prev = NULL;
    bool collisionDetected = false;
    
    while (current != NULL) {
        if (current->active) {
            // Calcular distância entre centros
            float dx = current->position.x - position.x;
            float dy = current->position.y - position.y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            // Verificar se há colisão
            if (distance < current->radius + radius) {
                *collectedType = current->type;
                current->active = false;  // Desativar para remoção
                collisionDetected = true;
                // Não sair do loop, continuar para desativar os outros
            }
        }
        
        prev = current;
        current = current->next;
    }
    
    // Se coletou um power-up, desativar todos os outros
    if (collisionDetected) {
        current = *powerups;
        while (current != NULL) {
            if (current->active) {
                current->active = false;
            }
            current = current->next;
        }
    }
    
    return collisionDetected;
}