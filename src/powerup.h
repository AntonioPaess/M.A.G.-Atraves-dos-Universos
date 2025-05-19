#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"


typedef enum {
    POWERUP_DAMAGE,   
    POWERUP_HEAL,     
    POWERUP_SHIELD    
} PowerupType;


typedef struct Powerup {
    Vector2 position;
    float radius;
    PowerupType type;
    bool active;
    float lifeTime;    
    struct Powerup *next;
} Powerup;


void InitPowerups(Powerup **powerups);


void AddPowerup(Powerup **powerups, Vector2 position, PowerupType type);


void UpdatePowerups(Powerup **powerups, float deltaTime);


void ClearPowerups(Powerup **powerups);


bool CheckPowerupCollision(Powerup **powerups, Vector2 position, float radius, PowerupType *collectedType);

#endif 