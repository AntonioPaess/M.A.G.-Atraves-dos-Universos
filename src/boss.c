#include "boss.h"
#include "raymath.h"
#include "utils.h"
#include "render.h"
#include <stdlib.h>
#include <math.h>

// Constantes específicas do boss
#define BOSS_BASE_RADIUS 60.0f
#define BOSS_BASE_SPEED 80.0f

// Novas constantes para saúde em cada camada
#define BOSS_LAYER4_HEALTH 50.0f  // Hexágono (primeira forma)
#define BOSS_LAYER3_HEALTH 100.0f  // Quadrado
#define BOSS_LAYER2_HEALTH 150.0f // Círculo
#define BOSS_LAYER1_HEALTH 250.0f // Mimético (última forma)

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
    boss->currentLayer = 4; // Começa com hexágono
    boss->layerHealth = BOSS_LAYER4_HEALTH;
    boss->maxLayerHealth = BOSS_LAYER4_HEALTH;
    boss->attackTimer = 0.0f;
    boss->active = true;
    boss->isTransitioning = false;
    boss->transitionTimer = 0.0f;
    
    // Inicializar variáveis para a fase mimética
    boss->isDashing = false;
    boss->dashDirection = (Vector2){0, 0};
    boss->dashTimer = 0.0f;
    boss->dashCooldown = 0.0f;
    boss->targetPosition = position;
}

void UpdateBoss(Boss *boss, Vector2 playerPosition, float deltaTime, Bullet **enemyBullets) {
    if (!boss->active) return;
    
    // Atualizar timer de ataque
    boss->attackTimer += deltaTime;
    
    // Se estiver em transição entre camadas
    if (boss->isTransitioning) {
        boss->transitionTimer += deltaTime;
        
        // Terminar a transição após 1 segundo
        if (boss->transitionTimer >= 1.0f) {
            boss->isTransitioning = false;
            boss->transitionTimer = 0.0f;
            
            // Lançar projéteis que ricocheteiam
            LaunchRicochetBullets(boss, enemyBullets);
            
            // Configurar saúde para a nova camada
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
                    boss->dashCooldown = BOSS_DASH_COOLDOWN; // Inicializa cooldown
                    break;
            }
        }
        return; // Não fazer mais nada durante a transição
    }
    
    // Atualizar cooldown do dash
    if (boss->dashCooldown > 0) {
        boss->dashCooldown -= deltaTime;
    }
    
    // Direção ao jogador
    Vector2 direction = Vector2Normalize(Vector2Subtract(playerPosition, boss->position));
    
    // Comportamento baseado na camada atual
    switch (boss->currentLayer) {
        case 4: // Hexágono - movimento lento, tiros diretos
            // Movimento em direção ao jogador
            boss->position = Vector2Add(boss->position, 
                                      Vector2Scale(direction, BOSS_BASE_SPEED * 0.5f * deltaTime));
            
            // Ataque a cada X segundos
            if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER4) {
                boss->attackTimer = 0.0f;
                
                // Tiro simples na direção do jogador
                AddBullet(enemyBullets, boss->position, direction);
            }
            break;
            
        case 3: // Quadrado - movimento errático, explosão em área
            // Adicionar aleatoriedade à direção
            direction.x += (float)GetRandomValue(-50, 50) / 100.0f;
            direction.y += (float)GetRandomValue(-50, 50) / 100.0f;
            direction = Vector2Normalize(direction);
            
            // Movimento mais rápido
            boss->position = Vector2Add(boss->position, 
                                      Vector2Scale(direction, BOSS_BASE_SPEED * 0.7f * deltaTime));
            
            // Ataque tipo exploder
            if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER3) {
                boss->attackTimer = 0.0f;
                
                // Disparar em 8 direções como o exploder
                for (int i = 0; i < 8; i++) {
                    float angle = i * (2.0f * PI / 8.0f);
                    Vector2 bulletDir = {cosf(angle), sinf(angle)};
                    AddBullet(enemyBullets, boss->position, bulletDir);
                }
            }
            break;
            
        case 2: // Círculo - movimento rápido, tiro triplo rotacional + teletransporte
            // Movimento rápido
            boss->position = Vector2Add(boss->position, 
                                      Vector2Scale(direction, BOSS_BASE_SPEED * 0.9f * deltaTime));
            
            // Ataque frequente
            if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER2) {
                boss->attackTimer = 0.0f;
                
                // Ângulo base na direção do jogador
                float baseAngle = atan2f(direction.y, direction.x);
                
                // Disparar 3 tiros em leque
                for (int i = -1; i <= 1; i++) {
                    float angle = baseAngle + i * (PI / 4.0f);
                    Vector2 bulletDir = {cosf(angle), sinf(angle)};
                    AddBullet(enemyBullets, boss->position, bulletDir);
                }
                
                // Poder especial: teletransporte quando estiver com menos de metade da vida
                if (boss->layerHealth < boss->maxLayerHealth / 2.0f && GetRandomValue(0, 100) < 20) {
                    // Calcular nova posição (mais próxima ao jogador)
                    float teleportDistance = Vector2Distance(playerPosition, boss->position) * 0.7f;
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, teleportDistance));
                    
                    // Disparar projéteis em espiral após teleporte
                    for (int i = 0; i < 12; i++) {
                        float angle = i * (2.0f * PI / 12.0f);
                        Vector2 bulletDir = {cosf(angle), sinf(angle)};
                        AddBullet(enemyBullets, boss->position, bulletDir);
                    }
                }
            }
            break;
            
        case 1: // Mimético - imita comportamentos do jogador
            // Sistema de dash similar ao do jogador
            if (boss->isDashing) {
                // Em dash: movimento rápido na direção do dash
                boss->position = Vector2Add(boss->position, 
                                         Vector2Scale(boss->dashDirection, BOSS_DASH_SPEED * deltaTime));
                
                boss->dashTimer -= deltaTime;
                if (boss->dashTimer <= 0) {
                    boss->isDashing = false;
                    boss->dashCooldown = BOSS_DASH_COOLDOWN;
                    
                    // Disparo em todas as direções ao fim do dash
                    for (int i = 0; i < 16; i++) {
                        float angle = i * (2.0f * PI / 16.0f);
                        Vector2 bulletDir = {cosf(angle), sinf(angle)};
                        AddBullet(enemyBullets, boss->position, bulletDir);
                    }
                }
            } else {
                // Calcular um ponto aleatório ao redor do jogador como alvo
                if (GetRandomValue(0, 100) < 2) { // 2% de chance a cada frame
                    float angle = GetRandomValue(0, 360) * DEG2RAD;
                    float distance = GetRandomValue(100, 300);
                    boss->targetPosition = Vector2Add(playerPosition, 
                                                    (Vector2){cosf(angle) * distance, sinf(angle) * distance});
                }
                
                // Movimentação "inteligente" em direção ao alvo
                Vector2 toTarget = Vector2Subtract(boss->targetPosition, boss->position);
                float distToTarget = Vector2Length(toTarget);
                
                if (distToTarget > 5.0f) {
                    Vector2 moveDir = Vector2Normalize(toTarget);
                    boss->position = Vector2Add(boss->position, 
                                             Vector2Scale(moveDir, BOSS_BASE_SPEED * 1.2f * deltaTime));
                }
                
                // Atirar na direção do jogador rapidamente
                if (boss->attackTimer >= BOSS_ATTACK_INTERVAL_LAYER1) {
                    boss->attackTimer = 0.0f;
                    
                    // Atirar 5 projéteis em cone direcionado ao jogador
                    float baseAngle = atan2f(direction.y, direction.x);
                    for (int i = -2; i <= 2; i++) {
                        float angle = baseAngle + i * (PI / 12.0f);
                        Vector2 bulletDir = {cosf(angle), sinf(angle)};
                        AddBullet(enemyBullets, boss->position, bulletDir);
                    }
                }
                
                // Usar dash quando disponível e jogador a uma certa distância
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
    
    // Manter o boss dentro da área jogável
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
        
        // Verificar se a camada foi destruída
        if (boss->layerHealth <= 0) {
            boss->currentLayer--;
            
            if (boss->currentLayer > 0) {
                // Passar para próxima camada
                boss->isTransitioning = true;
                boss->transitionTimer = 0.0f;
                
                // Conceder pontos para a camada destruída (retornado ao chamador)
                switch (boss->currentLayer + 1) { // +1 porque já decrementamos
                    case 4: return 1000; // Primeira camada vale 1000
                    case 3: return 2000; // Segunda camada vale 2000
                    case 2: return 3000; // Terceira camada vale 3000
                }
            } else {
                // Boss derrotado
                boss->active = false;
                return 4000; // Última camada vale 4000
            }
        }
        
        return true; // Dano causado mas camada não destruída
    }
    
    return false; // Sem colisão
}

void LaunchRicochetBullets(Boss *boss, Bullet **enemyBullets) {
    int numBullets = 8;  // Reduzido de 16 para 8
    if (boss->currentLayer == 1) {
        numBullets = 12; // Reduzido de 24 para 12 (mantendo a proporcionalidade)
    }
    
    for (int i = 0; i < numBullets; i++) {
        float angle = i * (2.0f * PI / numBullets);
        Vector2 bulletDir = {cosf(angle), sinf(angle)};
        
        // Adicionar bala especial que pode ricocheter
        AddRicochetBullet(enemyBullets, boss->position, bulletDir);
    }
}

void DrawBoss(const Boss *boss) {
    if (!boss->active) return;
    
    Vector2 pos = boss->position;
    float radius = boss->radius;
    
    // Efeito de pulsação
    float pulseTime = GetTime() * 2.0f;
    float pulseFactor = 1.0f + sinf(pulseTime) * 0.05f;
    
    // Cor base baseada na camada atual
    Color baseColor;
    switch (boss->currentLayer) {
        case 4: baseColor = DARKGRAY; break; // Hexágono
        case 3: baseColor = RED; break;      // Quadrado
        case 2: baseColor = YELLOW; break;   // Círculo
        case 1: baseColor = PURPLE; break;   // Mimético
        default: baseColor = WHITE; break;
    }
    
    // Se estiver em transição, piscar
    if (boss->isTransitioning) {
        float flash = sinf(boss->transitionTimer * 20.0f);
        baseColor = flash > 0 ? WHITE : baseColor;
    }
    
    // Desenhar as camadas do boss (de fora para dentro)
    
    // Camada 4 (Hexágono)
    if (boss->currentLayer >= 4) {
        // Desenhar hexágono
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
    
    // Camada 3 (Quadrado)
    if (boss->currentLayer >= 3) {
        float squareSize = radius * 1.0f * pulseFactor;
        
        // Quadrado com cantos arredondados
        DrawPixelRect(pos.x - squareSize/2, pos.y - squareSize/2, 
                     squareSize, squareSize, 
                     boss->currentLayer == 3 ? baseColor : Fade(baseColor, 0.5f));
    }
    
    // Camada 2 (Círculo)
    if (boss->currentLayer >= 2) {
        // Círculo interno
        DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, 
                        boss->currentLayer == 2 ? baseColor : Fade(baseColor, 0.5f));
    }
    
    // Camada 1 (Mimético)
    if (boss->currentLayer >= 1) {
        // Nova forma: estrela de 5 pontas
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
    
    // Núcleo (sempre visível)
    DrawPixelCircleV(pos, radius * 0.3f * pulseFactor, WHITE);
    
    // Efeitos baseados na camada atual
    switch (boss->currentLayer) {
        case 4: // Hexágono - Canhões nas pontas
            for (int i = 0; i < 6; i++) {
                float angle = i * (2.0f * PI / 6.0f);
                Vector2 gunPos = {
                    pos.x + cosf(angle) * radius * 1.3f,
                    pos.y + sinf(angle) * radius * 1.3f
                };
                
                DrawPixelCircleV(gunPos, radius * 0.15f, DARKGRAY);
            }
            break;
            
        case 3: // Quadrado - Efeito de carga explosiva
            {
                float chargeEffect = 0.6f + sinf(pulseTime * 3.0f) * 0.4f;
                DrawPixelCircleV(pos, radius * 0.8f * chargeEffect, Fade(RED, 0.3f));
            }
            break;
            
        case 2: // Círculo - Efeito de energia giratória
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
            
        case 1: // Mimético - Efeito de dash e teleporte
            {
                // Efeito de aura para o dash
                if (boss->isDashing) {
                    // Rastro de dash com várias bolinhas roxas (similar ao do player)
                    for (int i = 1; i <= 8; i++) {
                        float alpha = 0.8f - (i * 0.09f);
                        
                        Vector2 trailPos = Vector2Subtract(
                            pos, 
                            Vector2Scale(boss->dashDirection, radius * i * 0.5f)
                        );
                        
                        DrawPixelCircleV(trailPos, radius * (0.6f - i * 0.04f), Fade(PURPLE, alpha));
                    }
                    
                    // Aura externa
                    DrawPixelCircleV(pos, radius * 1.3f, Fade(PURPLE, 0.3f));
                } else {
                    // Efeito de energia misteriosa quando não está em dash
                    float energyPulse = 0.7f + sinf(GetTime() * 5.0f) * 0.3f;
                    
                    // Partículas orbitando
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
    
    // Barra de vida do boss
    float healthPercent = boss->layerHealth / boss->maxLayerHealth;
    float barWidth = radius * 2.5f;
    float barHeight = radius * 0.15f;
    
    // Fundo da barra
    DrawPixelRect(pos.x - barWidth/2, pos.y - radius * 1.5f, 
                 barWidth, barHeight, Fade(GRAY, 0.5f));
    
    // Vida atual
    DrawPixelRect(pos.x - barWidth/2, pos.y - radius * 1.5f, 
                 barWidth * healthPercent, barHeight, baseColor);
}