#include "boss.h"
#include "raymath.h"
#include "utils.h"
#include "render.h"
#include <stdlib.h>
#include <math.h>


#define BOSS_BASE_RADIUS 60.0f
#define BOSS_BASE_SPEED 80.0f


#define BOSS_LAYER4_HEALTH 50.0f  
#define BOSS_LAYER3_HEALTH 100.0f  
#define BOSS_LAYER2_HEALTH 150.0f 
#define BOSS_LAYER1_HEALTH 250.0f 

#define BOSS_ATTACK_INTERVAL_LAYER4 3.0f
#define BOSS_ATTACK_INTERVAL_LAYER3 2.0f
#define BOSS_ATTACK_INTERVAL_LAYER2 1.0f
#define BOSS_ATTACK_INTERVAL_LAYER1 0.5f

#define BOSS_DASH_COOLDOWN 3.0f
#define BOSS_DASH_DURATION 0.3f
#define BOSS_DASH_SPEED 800.0f

void InitBoss(Boss *boss, Vector2 position) {
    boss->position = position;
    boss->velocity = (Vector2){0, 0};
    boss->radius = BOSS_BASE_RADIUS;
    boss->currentLayer = 4; 
    boss->layerHealth = BOSS_LAYER4_HEALTH;
    boss->maxLayerHealth = BOSS_LAYER4_HEALTH;
    boss->attackTimer = 0.0f;
    boss->active = true;
    boss->isTransitioning = false;
    boss->transitionTimer = 0.0f;
    
    
    boss->isDashing = false;
    boss->dashDirection = (Vector2){0, 0};
    boss->dashTimer = 0.0f;
    boss->dashCooldown = 0.0f;
    boss->targetPosition = position;
}

void UpdateBoss(Boss *boss, Vector2 playerPosition, float deltaTime, Bullet **enemyBullets) {
    if (!boss->active) return;
    
    
    boss->attackTimer += deltaTime;
    
    
    if (boss->isTransitioning) {
        boss->transitionTimer += deltaTime;
        
        
        if (boss->transitionTimer >= 1.0f) {
            boss->isTransitioning = false;
            boss->transitionTimer = 0.0f;
            
            
            LaunchRicochetBullets(boss, enemyBullets);
            
            
            switch (boss->currentLayer) {
                case 3:
                    boss->layerHealth = BOSS_LAYER3_HEALTH;
                    boss->maxLayerHealth = BOSS_LAYER3_HEALTH;
                    break;
                case 2:
                    boss->layerHealth = BOSS_LAYER2_HEALTH;
                    boss->maxLayerHealth = BOSS_LAYER2_HEALTH;
                    break;
                case 1:
                    boss->layerHealth = BOSS_LAYER1_HEALTH;
                    boss->maxLayerHealth = BOSS_LAYER1_HEALTH;
                    boss->dashCooldown = BOSS_DASH_COOLDOWN; 
                    break;
            }
        }
        return; 
    }
    
    
    if (boss->dashCooldown > 0) {
        boss->dashCooldown -= deltaTime;
    }
    
    
    Vector2 direction = Vector2Normalize(Vector2Subtract(playerPosition, boss->position));
    
    
    switch (boss->currentLayer) {
        case 4: 
            
            boss->position = Vector2Add(boss->position, 
                                      Vector2Scale(direction, BOSS_BASE_SPEED * 0.5f * deltaTime));
            
            
            if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER4) {
                boss->attackTimer = 0.0f;
                
                
                AddBullet(enemyBullets, boss->position, direction, false);
            }
            break;
            
        case 3: 
            
            direction.x += (float)GetRandomValue(-50, 50) / 100.0f;
            direction.y += (float)GetRandomValue(-50, 50) / 100.0f;
            direction = Vector2Normalize(direction);
            
            
            boss->position = Vector2Add(boss->position, 
                                      Vector2Scale(direction, BOSS_BASE_SPEED * 0.7f * deltaTime));
            
            
            if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER3) {
                boss->attackTimer = 0.0f;
                
                
                for (int i = 0; i < 8; i++) {
                    float angle = i * (2.0f * PI / 8.0f);
                    Vector2 bulletDir = {cosf(angle), sinf(angle)};
                    AddBullet(enemyBullets, boss->position, bulletDir, false);
                }
            }
            break;
            
        case 2: 
            
            boss->position = Vector2Add(boss->position, 
                                      Vector2Scale(direction, BOSS_BASE_SPEED * 0.9f * deltaTime));
            
            
            if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER2) {
                boss->attackTimer = 0.0f;
                
                
                float baseAngle = atan2f(direction.y, direction.x);
                
                
                for (int i = -1; i <= 1; i++) {
                    float angle = baseAngle + i * (PI / 4.0f);
                    Vector2 bulletDir = {cosf(angle), sinf(angle)};
                    AddBullet(enemyBullets, boss->position, bulletDir, false);
                }
                
                
                if (boss->layerHealth < boss->maxLayerHealth / 2.0f && GetRandomValue(0, 100) < 20) {
                    
                    float teleportDistance = Vector2Distance(playerPosition, boss->position) * 0.7f;
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, teleportDistance));
                    
                    
                    for (int i = 0; i < 12; i++) {
                        float angle = i * (2.0f * PI / 12.0f);
                        Vector2 bulletDir = {cosf(angle), sinf(angle)};
                        AddBullet(enemyBullets, boss->position, bulletDir, false);
                    }
                }
            }
            break;
            
        case 1: 
            
            if (boss->isDashing) {
                
                boss->position = Vector2Add(boss->position, 
                                         Vector2Scale(boss->dashDirection, BOSS_DASH_SPEED * deltaTime));
                
                boss->dashTimer -= deltaTime;
                if (boss->dashTimer <= 0) {
                    boss->isDashing = false;
                    boss->dashCooldown = BOSS_DASH_COOLDOWN;
                    
                    
                    for (int i = 0; i < 16; i++) {
                        float angle = i * (2.0f * PI / 16.0f);
                        Vector2 bulletDir = {cosf(angle), sinf(angle)};
                        AddBullet(enemyBullets, boss->position, bulletDir, false);
                    }
                }
            } else {
                
                if (GetRandomValue(0, 100) < 2) { 
                    float angle = GetRandomValue(0, 360) * DEG2RAD;
                    float distance = GetRandomValue(100, 300);
                    boss->targetPosition = Vector2Add(playerPosition, 
                                                    (Vector2){cosf(angle) * distance, sinf(angle) * distance});
                }
                
                
                Vector2 toTarget = Vector2Subtract(boss->targetPosition, boss->position);
                float distToTarget = Vector2Length(toTarget);
                
                if (distToTarget > 5.0f) {
                    Vector2 moveDir = Vector2Normalize(toTarget);
                    boss->position = Vector2Add(boss->position, 
                                             Vector2Scale(moveDir, BOSS_BASE_SPEED * 1.2f * deltaTime));
                }
                
                
                if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER1) {
                    boss->attackTimer = 0.0f;
                    
                    
                    float baseAngle = atan2f(direction.y, direction.x);
                    for (int i = -2; i <= 2; i++) {
                        float angle = baseAngle + i * (PI / 12.0f);
                        Vector2 bulletDir = {cosf(angle), sinf(angle)};
                        AddBullet(enemyBullets, boss->position, bulletDir, false);
                    }
                }
                
                
                if (boss->dashCooldown <= 0 && 
                    Vector2Distance(boss->position, playerPosition) > 200 &&
                    Vector2Distance(boss->position, playerPosition) < 500) {
                    boss->isDashing = true;
                    boss->dashTimer = BOSS_DASH_DURATION;
                    boss->dashDirection = direction;
                }
            }
            break;
    }
    
    
    extern float currentPlayAreaRadius;
    Vector2 centerToPos = Vector2Subtract(boss->position, 
                                          (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y});
    float distanceToCenter = Vector2Length(centerToPos);
    
    if (distanceToCenter > currentPlayAreaRadius - boss->radius) {
        boss->position = Vector2Add(
            (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y},
            Vector2Scale(Vector2Normalize(centerToPos), 
                        currentPlayAreaRadius - boss->radius - 5.0f)
        );
    }
}

bool CheckBossHitByBullet(Boss *boss, Vector2 bulletPosition, float bulletRadius, int damage) {
    if (!boss->active || boss->isTransitioning) return false;
    
    if (CheckCollisionCircles(boss->position, boss->radius, bulletPosition, bulletRadius)) {
        boss->layerHealth -= damage;
        
        if (boss->layerHealth <= 0) {
            // Boss foi derrotado completamente
            boss->active = false;
            return 4000; // Pontuação por derrotar o boss
        }
        
        return true; // Boss foi atingido mas não derrotado
    }
    
    return false; // Sem colisão
}

void LaunchRicochetBullets(Boss *boss, Bullet **enemyBullets) {
    int numBullets = 8;  
    if (boss->currentLayer == 1) {
        numBullets = 12; 
    }
    
    for (int i = 0; i < numBullets; i++) {
        float angle = i * (2.0f * PI / numBullets);
        Vector2 bulletDir = {cosf(angle), sinf(angle)};
        
        
        AddRicochetBullet(enemyBullets, boss->position, bulletDir);
    }
}

void DrawBoss(const Boss *boss) {
    if (!boss->active) return;
    
    Vector2 pos = boss->position;
    float radius = boss->radius;
    
    
    float pulseTime = GetTime() * 2.0f;
    float pulseFactor = 1.0f + sinf(pulseTime) * 0.05f;
    
    
    Color baseColor;
    switch (boss->currentLayer) {
        case 4: baseColor = DARKGRAY; break; 
        case 3: baseColor = RED; break;      
        case 2: baseColor = YELLOW; break;   
        case 1: baseColor = PURPLE; break;   
        default: baseColor = WHITE; break;
    }
    
    
    if (boss->isTransitioning) {
        float flash = sinf(boss->transitionTimer * 20.0f);
        baseColor = flash > 0 ? WHITE : baseColor;
    }
    
    
    
    
    if (boss->currentLayer >= 4) {
        
        int sides = 6;
        for (int i = 0; i < sides; i++) {
            float angle1 = i * (2.0f * PI / sides);
            float angle2 = ((i + 1) % sides) * (2.0f * PI / sides);
            
            Vector2 point1 = {
                pos.x + cosf(angle1) * radius * 1.2f * pulseFactor,
                pos.y + sinf(angle1) * radius * 1.2f * pulseFactor
            };
            
            Vector2 point2 = {
                pos.x + cosf(angle2) * radius * 1.2f * pulseFactor,
                pos.y + sinf(angle2) * radius * 1.2f * pulseFactor
            };
            
            DrawLineEx(point1, point2, 3.0f, baseColor);
        }
    }
    
    
    if (boss->currentLayer >= 3) {
        float squareSize = radius * 1.0f * pulseFactor;
        
        
        DrawPixelRect(pos.x - squareSize/2, pos.y - squareSize/2, 
                     squareSize, squareSize, 
                     boss->currentLayer == 3 ? baseColor : Fade(baseColor, 0.5f));
    }
    
    
    if (boss->currentLayer >= 2) {
        
        DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, 
                        boss->currentLayer == 2 ? baseColor : Fade(baseColor, 0.5f));
    }
    
    
    if (boss->currentLayer >= 1) {
        
        if (boss->currentLayer == 1) {
            float innerRadius = radius * 0.4f * pulseFactor;
            float outerRadius = radius * 0.6f * pulseFactor;
            
            for (int i = 0; i < 5; i++) {
                float angle1 = i * (2.0f * PI / 5.0f);
                float angle2 = angle1 + (2.0f * PI / 10.0f);
                float angle3 = (i + 1) * (2.0f * PI / 5.0f);
                
                Vector2 outerPoint1 = {
                    pos.x + cosf(angle1) * outerRadius,
                    pos.y + sinf(angle1) * outerRadius
                };
                
                Vector2 innerPoint = {
                    pos.x + cosf(angle2) * innerRadius,
                    pos.y + sinf(angle2) * innerRadius
                };
                
                Vector2 outerPoint2 = {
                    pos.x + cosf(angle3) * outerRadius,
                    pos.y + sinf(angle3) * outerRadius
                };
                
                DrawLineEx(outerPoint1, innerPoint, 2.0f, PURPLE);
                DrawLineEx(innerPoint, outerPoint2, 2.0f, PURPLE);
            }
        }
    }
    
    
    DrawPixelCircleV(pos, radius * 0.3f * pulseFactor, WHITE);
    
    
    switch (boss->currentLayer) {
        case 4: 
            for (int i = 0; i < 6; i++) {
                float angle = i * (2.0f * PI / 6.0f);
                Vector2 gunPos = {
                    pos.x + cosf(angle) * radius * 1.3f,
                    pos.y + sinf(angle) * radius * 1.3f
                };
                
                DrawPixelCircleV(gunPos, radius * 0.15f, DARKGRAY);
            }
            break;
            
        case 3: 
            {
                float chargeEffect = 0.6f + sinf(pulseTime * 3.0f) * 0.4f;
                DrawPixelCircleV(pos, radius * 0.8f * chargeEffect, Fade(RED, 0.3f));
            }
            break;
            
        case 2: 
            {
                for (int i = 0; i < 8; i++) {
                    float angle = GetTime() * 3.0f + i * (PI / 4.0f);
                    Vector2 energyPos = {
                        pos.x + cosf(angle) * radius * 0.5f,
                        pos.y + sinf(angle) * radius * 0.5f
                    };
                    
                    DrawPixelCircleV(energyPos, radius * 0.08f, YELLOW);
                }
            }
            break;
            
        case 1: 
            {
                
                if (boss->isDashing) {
                    
                    for (int i = 1; i <= 8; i++) {
                        float alpha = 0.8f - (i * 0.09f);
                        
                        Vector2 trailPos = Vector2Subtract(
                            pos, 
                            Vector2Scale(boss->dashDirection, radius * i * 0.5f)
                        );
                        
                        DrawPixelCircleV(trailPos, radius * (0.6f - i * 0.04f), Fade(PURPLE, alpha));
                    }
                    
                    
                    DrawPixelCircleV(pos, radius * 1.3f, Fade(PURPLE, 0.3f));
                } else {
                    
                    float energyPulse = 0.7f + sinf(GetTime() * 5.0f) * 0.3f;
                    
                    
                    for (int i = 0; i < 12; i++) {
                        float angle = GetTime() * 2.0f + i * (PI / 6.0f);
                        float distance = radius * 0.6f * (1.0f + sinf(GetTime() * 1.5f + i * 0.5f) * 0.2f);
                        
                        Vector2 particlePos = {
                            pos.x + cosf(angle) * distance,
                            pos.y + sinf(angle) * distance
                        };
                        
                        DrawPixelCircleV(particlePos, radius * 0.06f * energyPulse, PURPLE);
                    }
                }
            }
            break;
    }
    
    
    float healthPercent = boss->layerHealth / boss->maxLayerHealth;
    float barWidth = radius * 2.5f;
    float barHeight = radius * 0.15f;
    
    
    DrawPixelRect(pos.x - barWidth/2, pos.y - radius * 1.5f, 
                 barWidth, barHeight, Fade(GRAY, 0.5f));
    
    
    DrawPixelRect(pos.x - barWidth/2, pos.y - radius * 1.5f, 
                 barWidth * healthPercent, barHeight, baseColor);
}