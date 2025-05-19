#include "player.h"
#include "raylib.h" 
#include "raymath.h" 
#include <math.h> 

void InitPlayer(Player *player, int windowWidth, int windowHeight) {
    
    player->position = (Vector2){
        (float)windowWidth / 2.0f,
        (float)windowHeight / 2.0f
    };
    player->radius = PLAYER_RADIUS;
    player->color = WHITE;
    player->lives = 3;
    player->isInvincible = false;
    player->invincibleTimer = 0.0f;
    player->blinkTimer = 0.0f;
    player->visible = true;
    player->hasShield = false;
    player->shieldTimer = 0.0f;
    
    
    player->isDashing = false;
    player->dashTimer = 0.0f;
    player->dashCooldown = 0.0f;
    player->dashDirection = (Vector2){0, 0};
}

void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight) {
    
    if (player->dashCooldown > 0.0f) {
        player->dashCooldown -= deltaTime;
        if (player->dashCooldown < 0.0f) {
            player->dashCooldown = 0.0f;
        }
    }
    
    
    if (IsKeyPressed(KEY_SPACE) && player->dashCooldown <= 0.0f && !player->isDashing) {
        
        Vector2 movement_input = {0.0f, 0.0f};
        
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement_input.y = -1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement_input.y = 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement_input.x = -1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement_input.x = 1.0f;
        
        
        if ((movement_input.x == 0.0f && movement_input.y == 0.0f)) {
            Vector2 mousePos = GetMousePosition();
            movement_input = Vector2Subtract(mousePos, player->position);
        }
        
        
        if (Vector2Length(movement_input) > 0.1f) {
            player->isDashing = true;
            player->dashTimer = DASH_DURATION;
            player->dashDirection = Vector2Normalize(movement_input);
        }
    }
    
    
    if (player->isDashing) {
        
        player->dashTimer -= deltaTime;
        
        
        Vector2 dashMovement = Vector2Scale(player->dashDirection, DASH_SPEED * deltaTime);
        Vector2 newPosition = Vector2Add(player->position, dashMovement);
        
        
        extern float currentPlayAreaRadius;
        float distanceToCenter = Vector2Distance(
            newPosition,
            (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}
        );
        
        
        if (distanceToCenter <= currentPlayAreaRadius - player->radius) {
            player->position = newPosition;
        } else {
            
            Vector2 dirFromCenter = Vector2Normalize(Vector2Subtract(newPosition, 
                                           (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}));
            
            player->position.x = PLAY_AREA_CENTER_X + dirFromCenter.x * (currentPlayAreaRadius - player->radius);
            player->position.y = PLAY_AREA_CENTER_Y + dirFromCenter.y * (currentPlayAreaRadius - player->radius);
        }
        
        
        if (player->dashTimer <= 0.0f) {
            player->isDashing = false;
            player->dashCooldown = DASH_COOLDOWN; 
        }
    } else {
        Vector2 movement_input = {0.0f, 0.0f};

        
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement_input.y = -1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement_input.y = 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement_input.x = -1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement_input.x = 1.0f;

        
        if (movement_input.x != 0.0f || movement_input.y != 0.0f) {
            if (Vector2Length(movement_input) > 0) {
                movement_input = Vector2Normalize(movement_input);
            }
            
            
            Vector2 newPosition = {
                player->position.x + movement_input.x * PLAYER_SPEED * deltaTime,
                player->position.y + movement_input.y * PLAYER_SPEED * deltaTime
            };
            
            
            extern float currentPlayAreaRadius; 
            
            float distanceToCenter = Vector2Distance(
                newPosition,
                (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}
            );
            
            
            if (distanceToCenter <= currentPlayAreaRadius - player->radius) {
                player->position = newPosition;
            } else {
                
                Vector2 dirFromCenter = Vector2Normalize(Vector2Subtract(newPosition, 
                                                   (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}));
                
                
                player->position.x = PLAY_AREA_CENTER_X + dirFromCenter.x * (currentPlayAreaRadius - player->radius);
                player->position.y = PLAY_AREA_CENTER_Y + dirFromCenter.y * (currentPlayAreaRadius - player->radius);
            }
        }
    }
    
    
    if (player->isInvincible) {
        
        player->invincibleTimer -= deltaTime;
        
        
        player->blinkTimer -= deltaTime;
        if (player->blinkTimer <= 0.0f) {
            player->visible = !player->visible; 
            player->blinkTimer = BLINK_FREQUENCY; 
        }
        
        
        if (player->invincibleTimer <= 0.0f) {
            player->isInvincible = false;
            player->visible = true; 
        }
    }
    
    
    if (player->hasShield) {
        player->shieldTimer -= deltaTime;
        if (player->shieldTimer <= 0.0f) {
            player->hasShield = false;
        }
    }
}

