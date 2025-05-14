#include "render.h"
#include "raylib.h"
#include <stdio.h> // Para TextFormat

// Definições de cores e fontes (podem ser movidas para utils.h ou um config.h)
#define TEXT_COLOR LIGHTGRAY
#define SCORE_COLOR GOLD
#define LIVES_COLOR RED
#define LEVEL_COLOR GREEN
#define GAME_OVER_COLOR MAROON
#define VICTORY_COLOR DARKGREEN

Font gameFont;

void InitRenderResources(void) {
    // Carregar fontes, texturas, etc.
    // Exemplo: gameFont = LoadFont("path/to/your/font.ttf");
    // Por enquanto, usaremos a fonte padrão da Raylib se nenhuma for carregada.
}

void UnloadRenderResources(void) {
    // Descarregar fontes, texturas, etc.
    // Exemplo: UnloadFont(gameFont);
}

void DrawHUD(int lives, int score, int level, int enemiesRemaining) {
    DrawText(TextFormat("Vidas: %d", lives), 10, 10, 20, LIVES_COLOR);
    DrawText(TextFormat("Pontos: %d", score), GetScreenWidth() - MeasureText(TextFormat("Pontos: %d", score), 20) - 10, 10, 20, SCORE_COLOR);
    DrawText(TextFormat("Nível: %d", level), 10, 30, 20, LEVEL_COLOR);
    DrawText(TextFormat("Inimigos: %d", enemiesRemaining), GetScreenWidth() - MeasureText(TextFormat("Inimigos: %d", enemiesRemaining), 20) - 10, 30, 20, WHITE);
}

void DrawGame(const Player *player, const EnemyList *enemies, const Bullet *bullets, int score, int level, int lives, int enemiesRemaining) {
    ClearBackground(BLACK);

    // Desenhar jogador
    if (player) {
        DrawRectangle(player->position.x, player->position.y, player->size.x, player->size.y, BLUE);

        // Desenhar linha de mira do mouse ao jogador
        Vector2 mousePos = GetMousePosition();
        Vector2 playerCenter = {
            player->position.x + player->size.x / 2,
            player->position.y + player->size.y / 2
        };
        DrawLine(playerCenter.x, playerCenter.y, mousePos.x, mousePos.y, ColorAlpha(RED, 0.5f));
    }

    // Desenhar inimigos
    if (enemies) {
        const Enemy *current = enemies->head;
        while (current != NULL) {
            DrawRectangle(current->position.x, current->position.y, current->size.x, current->size.y, RED); // Corrigido
            current = current->next;
        }
    }

    // Desenhar projéteis
    if (bullets) {
        const Bullet *current = bullets;
        while (current != NULL) {
            DrawRectangle(current->position.x, current->position.y, current->size.x, current->size.y, YELLOW); // Corrigido
            current = current->next;
        }
    }

    DrawHUD(lives, score, level, enemiesRemaining);
}

void DrawGameOverScreen(int finalScore, ScoreEntry *ranking, int rankingSize) {
    ClearBackground(DARKGRAY);
    const char* gameOverText = "GAME OVER";
    int textWidth = MeasureText(gameOverText, 80);
    DrawText(gameOverText, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 4, 80, GAME_OVER_COLOR);

    DrawText(TextFormat("Pontuação Final: %d", finalScore), GetScreenWidth() / 2 - MeasureText(TextFormat("Pontuação Final: %d", finalScore), 40) / 2, GetScreenHeight() / 2 - 40, 40, LIGHTGRAY);

    DrawText("Ranking:", 50, GetScreenHeight() / 2 + 20, 30, LIGHTGRAY);
    for (int i = 0; i < rankingSize && i < 5; ++i) { // Exibe os 5 primeiros
        DrawText(TextFormat("%d. %s - %d", i + 1, ranking[i].name, ranking[i].score), 60, GetScreenHeight() / 2 + 60 + (i * 25), 20, LIGHTGRAY);
    }
    DrawText("Pressione ENTER para reiniciar ou ESC para sair", GetScreenWidth()/2 - MeasureText("Pressione ENTER para reiniciar ou ESC para sair", 20)/2, GetScreenHeight() - 50, 20, WHITE);
}

void DrawVictoryScreen(int finalScore) {
    ClearBackground(SKYBLUE);
    const char* victoryText = "VITÓRIA!";
    int textWidth = MeasureText(victoryText, 80);
    DrawText(victoryText, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 3, 80, VICTORY_COLOR);

    DrawText(TextFormat("Você sobreviveu a todas as hordas!"), GetScreenWidth() / 2 - MeasureText("Você sobreviveu a todas as hordas!", 30) / 2, GetScreenHeight() / 2, 30, DARKGREEN);
    DrawText(TextFormat("Pontuação Final: %d", finalScore), GetScreenWidth() / 2 - MeasureText(TextFormat("Pontuação Final: %d", finalScore), 40) / 2, GetScreenHeight() / 2 + 50, 40, DARKGREEN);
    DrawText("Pressione ENTER para jogar novamente ou ESC para sair", GetScreenWidth()/2 - MeasureText("Pressione ENTER para jogar novamente ou ESC para sair", 20)/2, GetScreenHeight() - 50, 20, DARKBLUE);
}

// Implementação de DrawMainMenu (se necessário)
void DrawMainMenu(void) {
    ClearBackground(DARKBLUE);
    DrawText("M.A.G. - O Peixe Cósmico", GetScreenWidth() / 2 - MeasureText("M.A.G. - O Peixe Cósmico", 40) / 2, GetScreenHeight() / 4, 40, GOLD);
    DrawText("Pressione ENTER para iniciar", GetScreenWidth() / 2 - MeasureText("Pressione ENTER para iniciar", 20) / 2, GetScreenHeight() / 2, 20, LIGHTGRAY);
    DrawText("Setas/WASD para mover, J para atirar", GetScreenWidth() / 2 - MeasureText("Setas/WASD para mover, J para atirar", 20) / 2, GetScreenHeight() / 2 + 40, 20, LIGHTGRAY);
}

