#include "raylib.h"
#include "game.h"   
#include "render.h" 
#include "utils.h"  


int main(void) {
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "M.A.G. O inimigo agora é outro"); 
    SetTargetFPS(TARGET_FPS);
    

    Game game;
    InitGame(&game); 

    
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        
        if (IsKeyPressed(KEY_KP_MULTIPLY)) {
            AppToggleFullscreen();
        }

        
        UpdateGame(&game, deltaTime);

        
        BeginDrawing();
            ClearBackground(BLACK); 

            
            DrawGame(&game);
            
        EndDrawing();
    }

    
    // Descarregar todos os recursos de áudio
    UnloadGameAudio(game.shootSound, game.enemyExplodeSound, game.playerExplodeSound,
                   game.enemyNormalDeathSound, game.enemyTankDeathSound,
                   game.enemyExploderDeathSound, game.enemyShooterDeathSound, 
                   game.dashSound, // Adicione esta linha para o som do dash
                   game.backgroundMusic, game.menuMusic, game.tutorialMusic,
                   game.pauseMusic, game.gameOverMusic, game.nameEntryMusic,
                   game.bossMusic,
                   game.menuClickSound, game.powerupDamageSound, game.powerupHealSound, game.powerupShieldSound);
    CloseAudioDevice(); 
    CloseWindow();      

    return 0;
}