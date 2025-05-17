#include "raylib.h"
#include "game.h"   // Gerenciador principal do jogo (já inclui player, enemy, bullet)
#include "render.h" // Responsável por todo o desenho
#include "utils.h"  // Para constantes como SCREEN_WIDTH, SCREEN_HEIGHT, TARGET_FPS, e AppToggleFullscreen
// audio.h é incluído através de game.h

int main(void) {
    // Inicialização da Janela e Raylib
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "O Clone"); // Novo título
    SetTargetFPS(TARGET_FPS);
    // SetExitKey(KEY_NULL); // Desabilitar ESC para fechar se quiser tratar manualmente

    Game game;
    InitGame(&game); // Inicializa o estado do jogo

    // Loop Principal do Jogo
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Verificar tecla para alternar modo de tela cheia
        if (IsKeyPressed(KEY_F11)) { // Usar F11 como padrão para fullscreen
            AppToggleFullscreen();
        }

        // Atualizar Lógica do Jogo
        UpdateGame(&game, deltaTime);

        // Desenhar Tudo
        BeginDrawing();
            ClearBackground(BLACK); // Fundo preto padrão

            // Desenha o jogo baseado no estado atual
            DrawGame(&game);
            
        EndDrawing();
    }

    // Finalização
    UnloadGameAudio(game.shootSound, game.enemyExplodeSound, game.playerExplodeSound, game.backgroundMusic);
    CloseAudioDevice(); // Importante!
    CloseWindow();      // Fecha a janela e o contexto OpenGL

    return 0;
}