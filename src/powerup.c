#include "powerup.h"
#include <stdlib.h>
#include <math.h>  


void InitPowerups(Powerup **powerups) {
    *powerups = NULL;
}


void AddPowerup(Powerup **powerups, Vector2 position, PowerupType type) {
    Powerup *newPowerup = (Powerup *)malloc(sizeof(Powerup));
    if (newPowerup) {
        newPowerup->position = position;
        newPowerup->type = type;
        newPowerup->radius = 15.0f;  
        newPowerup->active = true;
        newPowerup->lifeTime = 10.0f;  
        newPowerup->next = *powerups;
        *powerups = newPowerup;
    }
}


void UpdatePowerups(Powerup **powerups, float deltaTime) {
    Powerup *current = *powerups;
    Powerup *prev = NULL;
    
    while (current != NULL) {
        
        current->lifeTime -= deltaTime;
        
        
        if (current->lifeTime <= 0.0f || !current->active) {
            Powerup *toRemove = current;
            
            if (prev == NULL) {
                
                *powerups = current->next;
                current = *powerups;
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


void ClearPowerups(Powerup **powerups) {
    Powerup *current = *powerups;
    while (current != NULL) {
        Powerup *next = current->next;
        free(current);
        current = next;
    }
    *powerups = NULL;
}


bool CheckPowerupCollision(Powerup **powerups, Vector2 position, float radius, PowerupType *collectedType) {
    Powerup *current = *powerups;
    Powerup *prev = NULL;
    bool collisionDetected = false;
    
    while (current != NULL) {
        if (current->active) {
            
            float dx = current->position.x - position.x;
            float dy = current->position.y - position.y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            
            if (distance < current->radius + radius) {
                *collectedType = current->type;
                current->active = false;  
                collisionDetected = true;
                
            }
        }
        
        prev = current;
        current = current->next;
    }
    
    
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