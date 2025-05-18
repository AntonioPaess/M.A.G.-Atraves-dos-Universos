#include "player.h"
#include "raylib.h" // For Raylib functions like IsKeyDown, Vector2, etc.
#include "raymath.h" // Adicionar este include para as funções Vector2Length, Vector2Normalize, etc.
#include <math.h> // For math functions

void InitPlayer(Player *player, int windowWidth, int windowHeight) {
    // Inicialização existente
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
    
    // Inicializar propriedades do dash
    player->isDashing = false;
    player->dashTimer = 0.0f;
    player->dashCooldown = 0.0f;
    player->dashDirection = (Vector2){0, 0};
}

void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight) {
    // Atualizar cooldown do dash
    if (player->dashCooldown > 0.0f) {
        player->dashCooldown -= deltaTime;
        if (player->dashCooldown < 0.0f) {
            player->dashCooldown = 0.0f;
        }
    }
    
    // Verificar tecla de dash (barra de espaço)
    if (IsKeyPressed(KEY_SPACE) && player->dashCooldown <= 0.0f && !player->isDashing) {
        // Capturar direção de movimento atual
        Vector2 movement_input = {0.0f, 0.0f};
        
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement_input.y = -1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement_input.y = 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement_input.x = -1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement_input.x = 1.0f;
        
        // Se não houver entrada direcional, use a direção do mouse
        if ((movement_input.x == 0.0f && movement_input.y == 0.0f)) {
            Vector2 mousePos = GetMousePosition();
            movement_input = Vector2Subtract(mousePos, player->position);
        }
        
        // Usar direção apenas se existir alguma entrada
        if (Vector2Length(movement_input) > 0.1f) {
            player->isDashing = true;
            player->dashTimer = DASH_DURATION;
            player->dashDirection = Vector2Normalize(movement_input);
        }
    }
    
    // Processar movimento
    if (player->isDashing) {
        // Movimento de dash
        player->dashTimer -= deltaTime;
        
        // Calcular movimento durante o dash (muito mais rápido)
        Vector2 dashMovement = Vector2Scale(player->dashDirection, DASH_SPEED * deltaTime);
        Vector2 newPosition = Vector2Add(player->position, dashMovement);
        
        // Verificar se a nova posição está dentro da área circular de jogo
        extern float currentPlayAreaRadius;
        float distanceToCenter = Vector2Distance(
            newPosition,
            (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}
        );
        
        // Verificar limites da área jogável
        if (distanceToCenter <= currentPlayAreaRadius - player->radius) {
            player->position = newPosition;
        } else {
            // Colocar na borda se ultrapassar os limites
            Vector2 dirFromCenter = Vector2Normalize(Vector2Subtract(newPosition, 
                                           (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}));
            
            player->position.x = PLAY_AREA_CENTER_X + dirFromCenter.x * (currentPlayAreaRadius - player->radius);
            player->position.y = PLAY_AREA_CENTER_Y + dirFromCenter.y * (currentPlayAreaRadius - player->radius);
        }
        
        // Verificar fim do dash
        if (player->dashTimer <= 0.0f) {
            player->isDashing = false;
            player->dashCooldown = DASH_COOLDOWN; // Começar cooldown
        }
    } else {
        Vector2 movement_input = {0.0f, 0.0f};

        // Capturar entrada do teclado para movimento
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement_input.y = -1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement_input.y = 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement_input.x = -1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement_input.x = 1.0f;

        // Verifica se o movimento resultaria em posição válida antes de aplicar
        if (movement_input.x != 0.0f || movement_input.y != 0.0f) {
            if (Vector2Length(movement_input) > 0) {
                movement_input = Vector2Normalize(movement_input);
            }
            
            // Calcula a nova posição potencial
            Vector2 newPosition = {
                player->position.x + movement_input.x * PLAYER_SPEED * deltaTime,
                player->position.y + movement_input.y * PLAYER_SPEED * deltaTime
            };
            
            // Verifica se a nova posição estaria dentro da área circular, considerando o raio do jogador
            extern float currentPlayAreaRadius; // Acesso ao raio atual definido em utils.c
            
            float distanceToCenter = Vector2Distance(
                newPosition,
                (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}
            );
            
            // Só move o jogador se a nova posição estiver dentro do limite
            if (distanceToCenter <= currentPlayAreaRadius - player->radius) {
                player->position = newPosition;
            } else {
                // Se ultrapassar o limite, move o jogador até o limite exato
                Vector2 dirFromCenter = Vector2Normalize(Vector2Subtract(newPosition, 
                                                   (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}));
                
                // Posicionar o jogador exatamente no limite
                player->position.x = PLAY_AREA_CENTER_X + dirFromCenter.x * (currentPlayAreaRadius - player->radius);
                player->position.y = PLAY_AREA_CENTER_Y + dirFromCenter.y * (currentPlayAreaRadius - player->radius);
            }
        }
    }
    
    // Atualizar invencibilidade
    if (player->isInvincible) {
        // Reduzir o timer de invencibilidade
        player->invincibleTimer -= deltaTime;
        
        // Atualizar o timer de piscar para o efeito visual
        player->blinkTimer -= deltaTime;
        if (player->blinkTimer <= 0.0f) {
            player->visible = !player->visible; // Alternar visibilidade
            player->blinkTimer = BLINK_FREQUENCY; // Resetar timer
        }
        
        // Desativar invencibilidade quando o tempo acabar
        if (player->invincibleTimer <= 0.0f) {
            player->isInvincible = false;
            player->visible = true; // Garantir que o jogador fique visível
        }
    }
    
    // Atualizar o timer do escudo, se ativo
    if (player->hasShield) {
        player->shieldTimer -= deltaTime;
        if (player->shieldTimer <= 0.0f) {
            player->hasShield = false;
        }
    }
}

