#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h" // Adicionado para Vector2 e outras definições da Raylib, se necessário
#include <stdbool.h>

#define PLAYER_SPEED 200    // velocidade do jogador (pixels por segundo)
#define PLAYER_SIZE 32      // tamanho (largura/altura) do jogador em pixels
#define PLAYER_LIVES 3      // vidas iniciais do jogador

#define PLAYER_WIDTH PLAYER_SIZE
#define PLAYER_HEIGHT PLAYER_SIZE

typedef struct {
    Vector2 position;      // Posição do jogador (top-left), usando Vector2 da Raylib
    Vector2 size;          // Tamanho do jogador (dimensões)
    int lives;             // Vidas restantes
    Vector2 last_direction; // Última direção de movimento (para atirar)
    // Adicionar outros atributos específicos do jogador se necessário
    // Por exemplo: float shootCooldown; Texture2D sprite;
} Player;

// Inicializa o jogador em posição inicial.
void InitPlayer(Player *player, int windowWidth, int windowHeight);

// Atualiza a lógica do jogador, incluindo movimento baseado em input.
// A verificação de input (IsKeyDown) permanece aqui por enquanto, mas poderia ser movida para input.c
// e o resultado passado para updatePlayer se uma maior abstração for desejada.
void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight);

// A função drawPlayer foi removida daqui. A renderização será feita pelo módulo render.c
// void drawPlayer(Player *player); // Removido

#endif // PLAYER_H
