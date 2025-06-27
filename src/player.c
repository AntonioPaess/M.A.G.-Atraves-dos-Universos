#include "player.h"
#include "raylib.h" 
#include "raymath.h" 
#include <math.h> 
#include "audio.h"
#include "utils.h"

// Declarar a variável externa
extern float currentPlayAreaRadius;

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

void UpdatePlayer(Player *player, float deltaTime, int screenWidth, int screenHeight, Sound dashSound) {
    // Verificar se algum gamepad está conectado
    bool gamepadConnected = IsGamepadAvailable(0);
    
    // Variáveis para movimento
    Vector2 movement = {0, 0};
    
    if (gamepadConnected) {
        // Usar analógico esquerdo para movimento (L3) - como WASD
        movement.x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        movement.y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        
        // Aplicar zona morta para evitar movimento não intencional
        if (fabsf(movement.x) < 0.2f) movement.x = 0;
        if (fabsf(movement.y) < 0.2f) movement.y = 0;
    }
    
    // Adicionar suporte para teclado (mantendo compatibilidade)
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement.x += 1.0f;
    
    // Atualizar cooldown do dash se estiver em cooldown
    if (player->dashCooldown > 0) {
        player->dashCooldown -= deltaTime;
    }
    
    // Dash com botão do PlayStation (X)
    if (!player->isDashing && player->dashCooldown <= 0) {
        Vector2 dashDirection = {0};
        bool shouldDash = false;
        
        if (IsKeyPressed(KEY_SPACE)) {
            // Se usando teclado, usar direção do mouse para o dash
            Vector2 mousePos = GetMousePosition();
            dashDirection = Vector2Subtract(mousePos, player->position);
            shouldDash = true;
        } 
        else if (gamepadConnected && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            // Se usando gamepad, usar direção do analógico direito (mira) para o dash
            float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
            float axisY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
            
            // Se analógico direito está sendo usado, usar essa direção
            if (fabsf(axisX) > 0.2f || fabsf(axisY) > 0.2f) {
                dashDirection.x = axisX;
                dashDirection.y = axisY;
                shouldDash = true;
            }
            // Caso contrário usar direção do movimento
            else if (Vector2Length(movement) > 0.1f) {
                dashDirection = movement;
                shouldDash = true;
            }
        }
        
        if (shouldDash && Vector2Length(dashDirection) > 0.1f) {
            player->isDashing = true;
            player->dashTimer = DASH_DURATION;
            player->dashDirection = Vector2Normalize(dashDirection);
            
            // Tocar o som do dash
            PlayGameSound(dashSound);
        }
    }
    
    // Atualizar lógica do dash
    if (player->isDashing) {
        player->dashTimer -= deltaTime;
        
        // Quando o dash termina
        if (player->dashTimer <= 0.0f) {
            player->isDashing = false;
            // CORREÇÃO AQUI: Definir o cooldown do dash quando ele termina
            player->dashCooldown = DASH_COOLDOWN;
        }
        
        // Calcular a nova posição pretendida
        Vector2 newPosition;
        newPosition.x = player->position.x + player->dashDirection.x * DASH_SPEED * deltaTime;
        newPosition.y = player->position.y + player->dashDirection.y * DASH_SPEED * deltaTime;
        
        // Verificar se a nova posição está dentro da arena
        if (IsPointInPlayArea(newPosition)) {
            // Se estiver dentro, mover normalmente
            player->position = newPosition;
        } else {
            // Se estiver tentando sair da arena, encontrar um ponto na borda
            Vector2 center = {PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y};
            Vector2 fromCenter = Vector2Subtract(newPosition, center);
            float distance = Vector2Length(fromCenter);
            
            if (distance > 0) {
                // Normalizar e escalar para o raio da arena menos o raio do jogador
                fromCenter = Vector2Scale(
                    Vector2Normalize(fromCenter), 
                    currentPlayAreaRadius - player->radius
                );
                // Definir a posição na borda da arena
                player->position = Vector2Add(center, fromCenter);
            }
        }
    }
    else {
        // Movimento normal quando não está em dash
        if (Vector2Length(movement) > 0) {
            movement = Vector2Normalize(movement);
            
            // Verificar posição antes de mover
            Vector2 newPosition;
            newPosition.x = player->position.x + movement.x * PLAYER_SPEED * deltaTime;
            newPosition.y = player->position.y + movement.y * PLAYER_SPEED * deltaTime;
            
            // Só mover se a nova posição estiver dentro da área de jogo
            if (IsPointInPlayArea(newPosition)) {
                player->position = newPosition;
            } else {
                // Se estiver tentando sair, deslisar ao longo da borda
                // Isso permite movimento tangencial à borda
                Vector2 center = {PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y};
                Vector2 toCenter = Vector2Subtract(center, player->position);
                Vector2 tangent = {-toCenter.y, toCenter.x};
                tangent = Vector2Normalize(tangent);
                
                float dot = Vector2DotProduct(movement, tangent);
                Vector2 slideVector = Vector2Scale(tangent, dot * PLAYER_SPEED * deltaTime);
                
                newPosition = Vector2Add(player->position, slideVector);
                if (IsPointInPlayArea(newPosition)) {
                    player->position = newPosition;
                }
            }
        }
    }
    
    // Resto do código (manter invencibilidade, etc.)
    if (player->isInvincible) {
        player->invincibleTimer -= deltaTime;
        
        if (player->invincibleTimer <= 0.0f) {
            player->isInvincible = false;
            player->visible = true;
        } else {
            player->blinkTimer -= deltaTime;
            if (player->blinkTimer <= 0.0f) {
                player->visible = !player->visible;
                player->blinkTimer = BLINK_FREQUENCY;
            }
        }
    }
    
    // Manter o jogador dentro da tela
    if (player->position.x < 0) player->position.x = 0;
    if (player->position.x > screenWidth) player->position.x = screenWidth;
    if (player->position.y < 0) player->position.y = 0;
    if (player->position.y > screenHeight) player->position.y = screenHeight;
    
    // Garantir que o jogador esteja sempre dentro da área jogável
    EnsurePlayerInsidePlayArea(player);
}

// Adicionar esta função após UpdatePlayer
void EnsurePlayerInsidePlayArea(Player *player) {
    // Verificar se o jogador está fora da área de jogo
    if (!IsPointInPlayArea(player->position)) {
        // Calcular vetor do centro da arena até o jogador
        Vector2 center = {PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y};
        Vector2 fromCenter = Vector2Subtract(player->position, center);
        float distance = Vector2Length(fromCenter);
        
        if (distance > 0) {
            // Normalizar e escalar para o raio da arena menos o raio do jogador
            // Adicionar uma pequena margem de segurança (2 pixels)
            fromCenter = Vector2Scale(
                Vector2Normalize(fromCenter), 
                currentPlayAreaRadius - player->radius - 2.0f
            );
            // Definir a posição na borda interna da arena
            player->position = Vector2Add(center, fromCenter);
        }
    }
}

