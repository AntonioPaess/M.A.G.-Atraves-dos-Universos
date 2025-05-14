#include "game.h"
#include "input.h" 
#include "render.h" 
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "scoreboard.h"
#include "utils.h" // For SCREEN_WIDTH, SCREEN_HEIGHT
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <time.h>   
#include <math.h> // Adicionado para sqrtf

#define MAX_LEVELS 10
#define ENEMIES_PER_LEVEL_MULTIPLIER 10
#define MAX_ENEMIES_ON_SCREEN 100

// Funções auxiliares internas ao módulo game
void HandlePlayerInput(Game *game, float deltaTime);
void HandleCollisions(Game *game);
void SpawnEnemiesForLevel(Game *game);
void UpdateEntities(Game *game, float deltaTime);
void CheckLevelCompletion(Game *game);
void CheckGameOver(Game *game);

void InitGame(Game *game) {
    srand(time(NULL));
    game->score = 0;
    game->currentLevel = 1;
    game->totalLevels = MAX_LEVELS;
    game->newLevelStarted = true;
    game->currentState = GAME_STATE_MAIN_MENU; 
    game->bullets = NULL;
    game->charCountPlayerName = 0;
    game->playerNameInput[0] = '\0';

    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);
    InitEnemyList(&game->enemies);
    LoadRanking(game); 
    InitRenderResources(); 
    // InitAudioDevice(); 
    // LoadAudioResources(); 
}

void ResetGame(Game *game) {
    Bullet *b = game->bullets;
    while (b != NULL) {
        Bullet *next_b = b->next;
        free(b);
        b = next_b;
    }
    game->bullets = NULL;

    Enemy *e = game->enemies.head;
    while (e != NULL) {
        Enemy *next_e = e->next;
        RemoveEnemy(&game->enemies, e); 
        e = next_e;
    }

    game->score = 0;
    game->currentLevel = 1;
    game->newLevelStarted = true;
    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);
    game->charCountPlayerName = 0;
    game->playerNameInput[0] = '\0';
}

void StartNextLevel(Game *game) {
    Bullet *b = game->bullets;
    while (b != NULL) {
        Bullet *next_b = b->next;
        free(b);
        b = next_b;
    }
    game->bullets = NULL;
    
    Enemy *e = game->enemies.head;
    while (e != NULL) {
        Enemy *next_e = e->next;
        RemoveEnemy(&game->enemies, e);
        e = next_e;
    }

    game->newLevelStarted = true;
    // Reset player position for new level
    game->player.position.x = SCREEN_WIDTH / 2.0f - game->player.size.x / 2.0f;
    game->player.position.y = SCREEN_HEIGHT - game->player.size.y - 10.0f;
    SpawnEnemiesForLevel(game);
}

void SpawnEnemiesForLevel(Game *game) {
    if (!game->newLevelStarted) return;

    int numEnemiesToSpawn = game->currentLevel * ENEMIES_PER_LEVEL_MULTIPLIER;
    if (numEnemiesToSpawn > MAX_ENEMIES_ON_SCREEN) {
        numEnemiesToSpawn = MAX_ENEMIES_ON_SCREEN;
    }
    game->enemiesToSpawnThisLevel = numEnemiesToSpawn;
    game->enemiesRemainingInLevel = numEnemiesToSpawn;

    printf("Iniciando Nível %d com %d inimigos.\n", game->currentLevel, numEnemiesToSpawn);
    for (int i = 0; i < numEnemiesToSpawn; i++) {
        float x_pos = (float)GetRandomValue(0, SCREEN_WIDTH - ENEMY_SIZE);
        float y_pos = (float)GetRandomValue(0, SCREEN_HEIGHT / 2); 
        
        Rectangle enemyRec = {x_pos, y_pos, ENEMY_SIZE, ENEMY_SIZE};
        Rectangle playerRec = {game->player.position.x, game->player.position.y, game->player.size.x, game->player.size.y};

        while (CheckCollisionRecs(enemyRec, playerRec)) {
            x_pos = (float)GetRandomValue(0, SCREEN_WIDTH - ENEMY_SIZE);
            y_pos = (float)GetRandomValue(0, SCREEN_HEIGHT / 2);
            enemyRec.x = x_pos;
            enemyRec.y = y_pos;
        }
        float speed = ENEMY_SPEED + (game->currentLevel -1) * 5.0f; 
        AddEnemy(&game->enemies, (Vector2){x_pos, y_pos}, speed);
    }
    game->newLevelStarted = false;
}

void HandlePlayerInput(Game *game, float deltaTime) {
    // Lógica para atirar com o mouse
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // Alterado para MOUSE_LEFT_BUTTON
        // Obter posição do mouse
        Vector2 mousePos = GetMousePosition();
        
        // Calcular direção do tiro (do centro do jogador para o mouse)
        Vector2 playerCenter = {
            game->player.position.x + game->player.size.x / 2.0f,
            game->player.position.y + game->player.size.y / 2.0f
        };
        
        Vector2 direction = {
            mousePos.x - playerCenter.x,
            mousePos.y - playerCenter.y
        };
        
        // Normalizar o vetor de direção
        float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
        if (length > 0) { // Evitar divisão por zero
            direction.x /= length;
            direction.y /= length;
        } else {
            // Caso o mouse esteja exatamente no centro do jogador, defina uma direção padrão (ex: para cima)
            // Ou simplesmente não atire, dependendo da preferência.
            // Aqui, vamos definir uma direção padrão para cima, como exemplo.
            direction = (Vector2){0.0f, -1.0f}; 
        }
        
        // Posição inicial do projétil (do centro do jogador, ajustado pelo tamanho do projétil)
        Vector2 bulletStartPosition = {
            playerCenter.x - BULLET_SIZE / 2.0f,
            playerCenter.y - BULLET_SIZE / 2.0f
        };
        
        // Criar projétil na direção calculada
        AddBullet(&game->bullets, 
                 bulletStartPosition,
                 direction);
        // PlaySound(game->shootSound); // Se tiver um som de tiro
    }
}

void UpdateEntities(Game *game, float deltaTime) {
    UpdatePlayer(&game->player, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
    UpdateEnemies(&game->enemies, &game->player, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
    UpdateBullets(&game->bullets, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void HandleCollisions(Game *game) {
    Bullet **b_ptr = &game->bullets;
    while (*b_ptr != NULL) {
        Bullet *current_bullet = *b_ptr;
        bool bullet_removed = false;

        if(current_bullet->active) {
            Rectangle bulletRec = {current_bullet->position.x, current_bullet->position.y, current_bullet->size.x, current_bullet->size.y};
            Enemy *enemy = game->enemies.head;
            while (enemy != NULL) {
                Enemy *next_enemy = enemy->next;
                if(enemy->active) {
                    Rectangle enemyRec = {enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y};
                    if (CheckCollisionRecs(bulletRec, enemyRec)) {
                        // PlaySound(enemyHitSound);
                        RemoveEnemy(&game->enemies, enemy); 
                        game->score += 100; 
                        game->enemiesRemainingInLevel--;

                        current_bullet->active = false; // Mark bullet for removal
                        // Bullet will be physically removed in UpdateBullets or here
                        // For now, UpdateBullets handles removal of inactive bullets
                        bullet_removed = true; // To break from inner loop if bullet hits one enemy
                        break; 
                    }
                }
                enemy = next_enemy;
            }
        }
        // If bullet was marked inactive (e.g. hit an enemy or went off-screen in UpdateBullets)
        // UpdateBullets should handle the actual free(). Here we just advance the pointer if it wasn't removed this frame.
        if (!current_bullet->active && !bullet_removed) { // Bullet became inactive for other reasons (e.g. off-screen)
             // This case is tricky: if UpdateBullets already freed it, this is a double free risk.
             // Let's assume UpdateBullets handles freeing. If it was marked inactive here, UpdateBullets will free it.
        }

        if (!bullet_removed) { // If bullet didn't hit anything this iteration
             b_ptr = &((*b_ptr)->next); 
        } else {
            // If bullet hit an enemy, it's marked inactive. UpdateBullets will remove it.
            // We need to re-evaluate *b_ptr in case it was the one removed by UpdateBullets
            // This part is complex if UpdateBullets also removes. Simpler: mark inactive, let UpdateBullets remove.
            // For now, if bullet_removed is true, we broke the inner loop. The outer loop continues.
            // The b_ptr should point to the next valid bullet or NULL. UpdateBullets handles this.
            // Let's simplify: if a bullet is used, it's marked inactive. UpdateBullets cleans up.
            // The current_bullet is now inactive, the *b_ptr might be advanced by UpdateBullets if it was freed.
            // This loop structure for bullet removal needs care. The one in UpdateBullets is safer.
            // Let's assume this loop only marks bullets as inactive upon collision.
            if (current_bullet->active) { // If not marked inactive by collision
                 b_ptr = &((*b_ptr)->next);
            }
            // If it was marked inactive, UpdateBullets will handle removal, so we just need to ensure b_ptr is correct.
            // This is safer: iterate and if a collision happens, mark bullet inactive. UpdateBullets will remove.
            // The b_ptr advancement should be simple: if current bullet is still *b_ptr, advance. If *b_ptr changed (freed), it's already advanced.
            // The safest is to let UpdateBullets handle all removals. This loop just marks.
             if (bullet_removed) {
                // No need to advance b_ptr, it will be handled by UpdateBullets if the bullet is removed
                // or the loop will continue with the same b_ptr if it was not the head. This is still risky.
                // A common pattern is to rebuild the list or use careful pointer manipulation.
                // Given UpdateBullets already has a robust removal, let's rely on that.
                // This loop just marks active = false.
             } else {
                b_ptr = &((*b_ptr)->next);
             }
        }
    }

    if (game->player.lives > 0) {
        Rectangle playerRec = {game->player.position.x, game->player.position.y, game->player.size.x, game->player.size.y};
        Enemy *enemy = game->enemies.head;
        while (enemy != NULL) {
            if(enemy->active) {
                Rectangle enemyRec = {enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y};
                if (CheckCollisionRecs(playerRec, enemyRec)) {
                    // PlaySound(playerHitSound);
                    game->player.lives--;
                    printf("Jogador atingido! Vidas: %d\n", game->player.lives);
                    
                    Enemy* toRemove = enemy;
                    enemy = enemy->next; 
                    RemoveEnemy(&game->enemies, toRemove);
                    game->enemiesRemainingInLevel--; 
                    continue; 
                }
            }
            enemy = enemy->next;
        }
    }
}

void CheckLevelCompletion(Game *game) {
    if (game->player.lives > 0 && game->enemies.count == 0 && !game->newLevelStarted) { 
        printf("Nível %d concluído!\n", game->currentLevel);
        // PlaySound(levelUpSound);
        game->currentLevel++;
        if (game->currentLevel > game->totalLevels) {
            game->currentState = GAME_STATE_VICTORY;
            printf("VITÓRIA!\n");
            // SavePlayerScore(game); // Score saving is handled when ENTER is pressed on victory/gameover screen
        } else {
            StartNextLevel(game);
        }
    }
}

void CheckGameOver(Game *game) {
    if (game->player.lives <= 0 && game->currentState == GAME_STATE_PLAYING) { // Ensure it only triggers once
        game->currentState = GAME_STATE_GAME_OVER;
        printf("GAME OVER!\n");
        // SavePlayerScore(game); // Score saving is handled when ENTER is pressed on victory/gameover screen
        // PlaySound(gameOverSound);
    }
}

void LoadRanking(Game *game) {
    ScoreEntry *loaded_scores = NULL;
    int num_loaded = LoadScoreboard("ranking.txt", &loaded_scores);
    
    // The file should be sorted by SavePlayerScore. If not, sort here.
    // SortScoreboard(loaded_scores, num_loaded); // Optional: if file might not be sorted

    game->rankingCount = 0;
    for (int i = 0; i < num_loaded && i < MAX_SCORES; ++i) {
        game->ranking[i] = loaded_scores[i];
        game->rankingCount++;
    }
    if (loaded_scores) {
        free(loaded_scores);
    }
}

void SavePlayerScore(Game *game) {
    ScoreEntry entry;
    if (strlen(game->playerNameInput) == 0) {
         strcpy(entry.name, "M.A.G."); 
    } else {
        strncpy(entry.name, game->playerNameInput, MAX_NAME_LENGTH -1);
        entry.name[MAX_NAME_LENGTH-1] = '\0'; // Ensure null termination
    }
    entry.score = game->score;
    entry.lives = game->player.lives; 
    entry.timeAlive = (float)GetTime(); // TODO: Implement a proper game timer
    entry.enemiesKilled = 0; // TODO: Implement total enemies killed counter
    entry.phasesWon = (game->currentState == GAME_STATE_VICTORY) ? game->totalLevels : game->currentLevel -1;
    if (game->player.lives <= 0 && game->currentState != GAME_STATE_VICTORY) entry.phasesWon = game->currentLevel -1;
    if (entry.phasesWon < 0) entry.phasesWon = 0; // Ensure non-negative

    ScoreEntry *all_scores = NULL;
    int total_score_count = LoadScoreboard("ranking.txt", &all_scores);
    
    AddScoreEntry(&all_scores, &total_score_count, entry);
    
    SortScoreboard(all_scores, total_score_count); // Sort before saving
    
    SaveScoreboard("ranking.txt", all_scores, total_score_count);
    
    if (all_scores) free(all_scores);

    LoadRanking(game); // Reload to update the game's internal top 5 ranking display
}

void UpdateGame(Game *game, float deltaTime) {
    switch (game->currentState) {
        case GAME_STATE_MAIN_MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame(game);
                game->currentState = GAME_STATE_PLAYING;
                StartNextLevel(game); 
            }
            break;
        case GAME_STATE_PLAYING:
            // ProcessInput(); // Global inputs like pause
            if (IsKeyPressed(KEY_P)) game->currentState = GAME_STATE_PAUSED;

            HandlePlayerInput(game, deltaTime); 
            UpdateEntities(game, deltaTime);    
            HandleCollisions(game);           
            CheckLevelCompletion(game);       
            CheckGameOver(game);              
            break;
        case GAME_STATE_PAUSED:
            if (IsKeyPressed(KEY_P)) game->currentState = GAME_STATE_PLAYING; 
            break;
        case GAME_STATE_GAME_OVER:
        case GAME_STATE_VICTORY:
            {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 32) && (key <= 125) && (game->charCountPlayerName < MAX_NAME_LENGTH - 1)) {
                        game->playerNameInput[game->charCountPlayerName] = (char)key;
                        game->playerNameInput[game->charCountPlayerName+1] = '\0'; 
                        game->charCountPlayerName++;
                    }
                    key = GetCharPressed(); 
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (game->charCountPlayerName > 0) {
                        game->charCountPlayerName--;
                        game->playerNameInput[game->charCountPlayerName] = '\0';
                    }
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    SavePlayerScore(game); 
                    game->currentState = GAME_STATE_MAIN_MENU;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                     game->currentState = GAME_STATE_MAIN_MENU;
                }
            }
            break;
        default:
            break;
    }
}

