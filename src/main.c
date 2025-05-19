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

    
    UnloadGameAudio(game.shootSound, game.enemyExplodeSound, game.playerExplodeSound, game.backgroundMusic);
    CloseAudioDevice(); 
    CloseWindow();      

    return 0;
}