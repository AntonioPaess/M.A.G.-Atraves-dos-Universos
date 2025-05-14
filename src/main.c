#include "raylib.h"
#include "game.h"   // Gerenciador principal do jogo
#include "render.h" // Responsável por todo o desenho
#include "utils.h"  // Para constantes como SCREEN_WIDTH, SCREEN_HEIGHT, TARGET_FPS, e AppToggleFullscreen
#include "audio.h"  // Para gerenciamento de áudio

int main(void) {
    // Inicialização da Janela e Raylib
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "M.A.G. - O inimigo agora é outro!");
    SetTargetFPS(TARGET_FPS);
    // SetExitKey(0); // Descomente para desabilitar ESC para fechar (se o jogo tratar ESC de outra forma)

    Game game;
    InitGame(&game); // Inicializa o estado do jogo, incluindo jogador, inimigos, placar, etc.
    // LoadAudioResources(); // Carrega sons e músicas - movido para InitGame ou chamado explicitamente

    // Loop Principal do Jogo
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Verificar tecla para alternar modo de tela cheia
        if (IsKeyPressed(KEY_I)) { // Ou KEY_F11, conforme sua preferência
            AppToggleFullscreen(); // Chama a função renomeada de utils.c
        }

        // Atualizar Lógica do Jogo
        UpdateGame(&game, deltaTime);
        // UpdateMusicStreamLocal(game.backgroundMusic); // Atualiza stream de música se houver

        // Desenhar Tudo
        BeginDrawing();
            ClearBackground(BLACK); // Limpa a tela com uma cor base

            switch (game.currentState) {
                case GAME_STATE_MAIN_MENU:
                    DrawMainMenu(); // Função de render.c
                    break;
                case GAME_STATE_PLAYING:
                    // Passa os dados necessários para DrawGame de render.c
                    DrawGame(&game.player, &game.enemies, game.bullets, 
                             game.score, game.currentLevel, game.player.lives, 
                             game.enemiesRemainingInLevel); 
                    break;
                case GAME_STATE_PAUSED:
                    // Desenha o jogo por baixo e uma sobreposição de pausa
                    DrawGame(&game.player, &game.enemies, game.bullets, 
                             game.score, game.currentLevel, game.player.lives, 
                             game.enemiesRemainingInLevel);
                    DrawText("PAUSADO", SCREEN_WIDTH / 2 - MeasureText("PAUSADO", 40) / 2, SCREEN_HEIGHT / 2 - 20, 40, YELLOW);
                    DrawText("Pressione P para continuar", SCREEN_WIDTH / 2 - MeasureText("Pressione P para continuar", 20) / 2, SCREEN_HEIGHT / 2 + 30, 20, LIGHTGRAY);
                    break;
                case GAME_STATE_GAME_OVER:
                    DrawGameOverScreen(game.score, game.ranking, game.rankingCount); // Função de render.c
                    // Exibe o campo de texto para nome
                    DrawText(TextFormat("Digite seu nome: %s", game.playerNameInput), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Digite seu nome: %s", game.playerNameInput), 20) / 2, SCREEN_HEIGHT - 100, 20, WHITE);
                    break;
                case GAME_STATE_VICTORY:
                    DrawVictoryScreen(game.score); // Função de render.c
                     DrawText(TextFormat("Digite seu nome: %s", game.playerNameInput), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Digite seu nome: %s", game.playerNameInput), 20) / 2, SCREEN_HEIGHT - 100, 20, WHITE);
                    break;
                default:
                    // Estado desconhecido, talvez desenhar uma mensagem de erro
                    DrawText("ESTADO DE JOGO DESCONHECIDO!", 10, 10, 20, RED);
                    break;
            }
        EndDrawing();
    }

    // Finalização
    UnloadRenderResources(); // Descarrega fontes, texturas, etc.
    // UnloadAudioResources();  // Descarrega sons e músicas - movido para ser chamado antes de CloseAudioDevice
    // CloseAudioDevice(); // Fecha o dispositivo de áudio
    CloseWindow();        // Fecha a janela e o contexto OpenGL

    return 0;
}

