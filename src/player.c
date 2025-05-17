#include "player.h"
#include "raylib.h" // For Raylib functions like IsKeyDown, Vector2, etc.
#include <math.h> // For math functions

void InitPlayer(Player *player, int windowWidth, int windowHeight) {
    // Inicializar o jogador como um círculo no centro da tela
    player->position = (Vector2){
        (float)windowWidth / 2.0f,
        (float)windowHeight / 2.0f
    };
    player->radius = PLAYER_RADIUS;
    player->color = WHITE; // Cor branca para o estilo minimalista
    
    // Inicializar o sistema de vidas
    player->lives = 3;
    player->isInvincible = false;
    player->invincibleTimer = 0.0f;
    player->blinkTimer = 0.0f;
    player->visible = true;
}

void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight) {
    Vector2 movement_input = {0.0f, 0.0f};

    // Capturar entrada do teclado para movimento
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement_input.y = -1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement_input.y = 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement_input.x = -1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement_input.x = 1.0f;

    // Aplicar movimento considerando a velocidade e delta time
    player->position.x += movement_input.x * PLAYER_SPEED * deltaTime;
    player->position.y += movement_input.y * PLAYER_SPEED * deltaTime;

    // Manter o jogador dentro dos limites da tela, considerando o raio
    if (player->position.x - player->radius < 0) 
        player->position.x = player->radius;
    
    if (player->position.y - player->radius < 0) 
        player->position.y = player->radius;
    
    if (player->position.x + player->radius > windowWidth) 
        player->position.x = windowWidth - player->radius;
    
    if (player->position.y + player->radius > windowHeight) 
        player->position.y = windowHeight - player->radius;
    
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
}

