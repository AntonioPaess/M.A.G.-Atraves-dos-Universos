#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "utils.h" 
#include <stdbool.h>

#define INVINCIBILITY_TIME 3.0f  
#define BLINK_FREQUENCY 0.1f     
#define DASH_DURATION 0.25f      
#define DASH_COOLDOWN 5.0f       
#define DASH_SPEED 800.0f        

typedef struct {
    Vector2 position;
    float radius;
    Color color;
    int lives;               
    bool isInvincible;       
    float invincibleTimer;   
    float blinkTimer;        
    bool visible;            
    bool hasShield;          
    float shieldTimer;       
    
    
    bool isDashing;          
    float dashTimer;         
    float dashCooldown;      
    Vector2 dashDirection;   
} Player;

void InitPlayer(Player *player, int windowWidth, int windowHeight);
void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight);

#endif 
