#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "audio.h"
#include "render.h"
#include "scoreboard.h"
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include "narrative_text.h"


extern float currentPlayAreaRadius;

void ResetGame(Game *game) {
    
    Enemy *currentEnemy = game->enemies.head;
    while (currentEnemy != NULL) {
        Enemy *nextEnemy = currentEnemy->next;
        free(currentEnemy);
        currentEnemy = nextEnemy;
    }
    game->enemies.head = NULL;
    game->enemies.count = 0;

    
    Bullet *currentBullet = game->bullets;
    while (currentBullet != NULL) {
        Bullet *nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }
    game->bullets = NULL;
    
    
    currentBullet = game->enemyBullets;
    while (currentBullet != NULL) {
        Bullet *nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }
    game->enemyBullets = NULL;

    
    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);

    
    game->score = 0;

    
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 1.5f; 
    game->difficultyTimer = 0.0f;
    game->shootCooldown = 0.0f;  

    
    game->currentState = GAME_STATE_PLAYING;

    
    if (IsAudioDeviceReady() && game->backgroundMusic.ctxData != NULL) {
        StopMusicStream(game->backgroundMusic);
        PlayMusicStream(game->backgroundMusic);
    }
    
    
    ClearPowerups(&game->powerups);
    
    
    game->enemiesKilled = 0;
    game->nextPowerupAt = 10;
    game->increasedDamage = false;

    
    game->bossActive = false;
    game->enemiesKilledSinceBoss = 0;
    game->showBossMessage = false;
    game->bossMessageTimer = 0.0f;

    
    game->gameTime = 0.0f;
    game->showGameSummary = false;
}

void UpdateDifficulty(Game *game, float deltaTime) {
    game->difficultyTimer += deltaTime;
    
    if (game->difficultyTimer > 10.0f) {
        game->enemySpawnInterval *= 0.90f; 
        if (game->enemySpawnInterval < 0.2f) { 
            game->enemySpawnInterval = 0.2f;
        }
        game->difficultyTimer = 0.0f; 
        
    }
}

void SpawnEnemy(Game *game) {
    
    if (game->bossActive) return;

    game->enemySpawnTimer += GetFrameTime();
    if (game->enemySpawnTimer >= game->enemySpawnInterval) {
        game->enemySpawnTimer = 0.0f;

        Vector2 spawnPosition;
        int side = GetRandomValue(0, 3); 

        
        EnemyType type;
        int randomType = GetRandomValue(1, 100);
        
        
        if (game->score < 1000) {
            
            type = ENEMY_TYPE_NORMAL;
        } 
        else if (game->score < 2000) {
            
            if (randomType <= 50) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 60) type = ENEMY_TYPE_SHOOTER; 
            else if (randomType <= 85) type = ENEMY_TYPE_TANK;
            else type = ENEMY_TYPE_EXPLODER;
        }
        else if (game->score < 3000) {
            
            if (randomType <= 35) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 45) type = ENEMY_TYPE_SHOOTER; 
            else if (randomType <= 75) type = ENEMY_TYPE_TANK;
            else if (randomType <= 90) type = ENEMY_TYPE_EXPLODER;
            else type = ENEMY_TYPE_SPEEDER;
        }
        else {
            
            if (randomType <= 20) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 25) type = ENEMY_TYPE_SHOOTER; 
            else if (randomType <= 55) type = ENEMY_TYPE_TANK;
            else if (randomType <= 85) type = ENEMY_TYPE_EXPLODER;
            else type = ENEMY_TYPE_SPEEDER;
        }
        
        
        float radius;
        float speed;
        
        switch (type) {
            case ENEMY_TYPE_SPEEDER:
                radius = ENEMY_RADIUS_MIN;
                speed = ENEMY_SPEED_MAX + (game->score / 1000.0f);
                break;
            case ENEMY_TYPE_TANK:
                radius = ENEMY_RADIUS_MAX;
                speed = ENEMY_SPEED_MIN + (game->score / 2000.0f);
                break;
            case ENEMY_TYPE_EXPLODER:
                radius = ENEMY_RADIUS_MIN + 5.0f;
                speed = ENEMY_SPEED_MIN + (ENEMY_SPEED_MAX / 2.0f) + (game->score / 1500.0f);
                break;
            case ENEMY_TYPE_SHOOTER:
                radius = ENEMY_RADIUS_MIN + 3.0f;
                speed = ENEMY_SPEED_MIN + (ENEMY_SPEED_MAX / 3.0f) + (game->score / 1800.0f);
                break;
            default: 
                radius = GetRandomValue(ENEMY_RADIUS_MIN, ENEMY_RADIUS_MAX);
                speed = GetRandomValue(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) + (game->score / 1000.0f);
                break;
        }
        
        
        if (speed > ENEMY_SPEED_MAX * 2) speed = ENEMY_SPEED_MAX * 2;

        
        switch (side) {
            case 0: 
                spawnPosition = (Vector2){
                    (float)GetRandomValue((int)PLAY_AREA_LEFT, (int)PLAY_AREA_RIGHT), 
                    PLAY_AREA_TOP - radius - 10.0f
                };
                break;
            case 1: 
                spawnPosition = (Vector2){
                    (float)GetRandomValue((int)PLAY_AREA_LEFT, (int)PLAY_AREA_RIGHT), 
                    PLAY_AREA_BOTTOM + radius + 10.0f
                };
                break;
            case 2: 
                spawnPosition = (Vector2){
                    PLAY_AREA_LEFT - radius - 10.0f, 
                    (float)GetRandomValue((int)PLAY_AREA_TOP, (int)PLAY_AREA_BOTTOM)
                };
                break;
            case 3: 
                spawnPosition = (Vector2){
                    PLAY_AREA_RIGHT + radius + 10.0f, 
                    (float)GetRandomValue((int)PLAY_AREA_TOP, (int)PLAY_AREA_BOTTOM)
                };
                break;
        }
        
        AddEnemy(&game->enemies, spawnPosition, radius, speed, WHITE, type);
    }
}

void SpawnEnemies(Game *game) {
    
    if (game->bossActive) return;

    
    int enemiesToSpawn = MAX_ENEMIES - game->enemies.count;
    
    for (int i = 0; i < enemiesToSpawn; i++) {
        
        EnemyType type;
        int randomType = GetRandomValue(1, 100);
        
        
        if (game->score < 1000) {
            
            type = ENEMY_TYPE_NORMAL;
        } 
        else if (game->score < 2000) {
            
            if (randomType <= 50) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 60) type = ENEMY_TYPE_SHOOTER; 
            else if (randomType <= 85) type = ENEMY_TYPE_TANK;
            else type = ENEMY_TYPE_EXPLODER;
        }
        else if (game->score < 3000) {
            
            if (randomType <= 35) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 45) type = ENEMY_TYPE_SHOOTER; 
            else if (randomType <= 75) type = ENEMY_TYPE_TANK;
            else if (randomType <= 90) type = ENEMY_TYPE_EXPLODER;
            else type = ENEMY_TYPE_SPEEDER;
        }
        else {
            
            if (randomType <= 20) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 25) type = ENEMY_TYPE_SHOOTER; 
            else if (randomType <= 55) type = ENEMY_TYPE_TANK;
            else if (randomType <= 85) type = ENEMY_TYPE_EXPLODER;
            else type = ENEMY_TYPE_SPEEDER;
        }
        
        
        float radius;
        float speed;
        
        switch (type) {
            case ENEMY_TYPE_SPEEDER:
                radius = ENEMY_RADIUS_MIN;
                speed = ENEMY_SPEED_MAX + (game->score / 1000.0f);
                break;
            case ENEMY_TYPE_TANK:
                radius = ENEMY_RADIUS_MAX;
                speed = ENEMY_SPEED_MIN + (game->score / 2000.0f);
                break;
            case ENEMY_TYPE_EXPLODER:
                radius = ENEMY_RADIUS_MIN + 5.0f;
                speed = ENEMY_SPEED_MIN + (ENEMY_SPEED_MAX / 2.0f) + (game->score / 1500.0f);
                break;
            case ENEMY_TYPE_SHOOTER:
                radius = ENEMY_RADIUS_MIN + 3.0f;
                speed = ENEMY_SPEED_MIN + (ENEMY_SPEED_MAX / 3.0f) + (game->score / 1800.0f);
                break;
            default: 
                radius = GetRandomValue(ENEMY_RADIUS_MIN, ENEMY_RADIUS_MAX);
                speed = GetRandomValue(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) + (game->score / 1000.0f);
                break;
        }
        
        
        if (speed > ENEMY_SPEED_MAX * 2) speed = ENEMY_SPEED_MAX * 2;

        
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float spawnDistance = PLAY_AREA_RADIUS + radius + 20.0f; 
        
        Vector2 spawnPosition = {
            PLAY_AREA_CENTER_X + cosf(angle) * spawnDistance,
            PLAY_AREA_CENTER_Y + sinf(angle) * spawnDistance
        };
        
        AddEnemy(&game->enemies, spawnPosition, radius, speed, WHITE, type);
    }
}

void HandleInput(Game *game, float deltaTime) {
    
    if (game->shootCooldown > 0) {
        game->shootCooldown -= deltaTime;
    }

    
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && game->shootCooldown <= 0) {
        Vector2 mousePos = GetMousePosition();
        Vector2 direction = {0};
        
        
        Vector2 diff = Vector2Subtract(mousePos, game->player.position);
        direction = Vector2Normalize(diff);
        
        
        Vector2 bulletStartPosition = game->player.position;

        if (game->increasedDamage) {
            
            AddBulletWithProps(&game->bullets, bulletStartPosition, direction, BULLET_RADIUS * 1.5f, 2);
        } else {
            
            AddBullet(&game->bullets, bulletStartPosition, direction);
        }
        PlayGameSound(game->shootSound);
        
        
        game->shootCooldown = 0.2f;
    }
}

void HandleCollisions(Game *game) {
    
    Bullet *currentBullet = game->bullets;
    while (currentBullet != NULL) {
        if (currentBullet->active) {
            
            Enemy *currentEnemy = game->enemies.head;
            Enemy *prevEnemy = NULL;
            while (currentEnemy != NULL) {
                if (currentEnemy->active) {
                    if (CheckCollisionCircles(currentBullet->position, currentBullet->radius,
                                              currentEnemy->position, currentEnemy->radius)) {
                        PlayGameSound(game->enemyExplodeSound);
                        currentBullet->active = false; 
                        
                        
                        currentEnemy->health -= currentBullet->damage;
                        
                        
                        if (currentEnemy->health <= 0) {
                            
                            if (currentEnemy->type == ENEMY_TYPE_EXPLODER) {
                                for (int i = 0; i < 8; i++) {
                                    float angle = i * (2.0f * PI / 8.0f);
                                    Vector2 direction = {cosf(angle), sinf(angle)};
                                    AddBullet(&game->enemyBullets, currentEnemy->position, direction);
                                }
                            }
                            
                            
                            Enemy* toRemove = currentEnemy;
                            currentEnemy = currentEnemy->next;

                            if (toRemove->prev == NULL) {
                                
                                game->enemies.head = toRemove->next;
                                if (game->enemies.head != NULL) {
                                    game->enemies.head->prev = NULL;
                                }
                            } else {
                                
                                toRemove->prev->next = toRemove->next;
                                if (toRemove->next != NULL) {
                                    toRemove->next->prev = toRemove->prev;
                                }
                            }

                            free(toRemove);
                            game->enemies.count--;
                            
                            
                            game->enemiesKilled++;
                            
                            
                            if (game->enemiesKilled % 10 == 0 && game->enemiesKilled > 0) {
                                const char* milestoneText = GetKillMilestoneText();
                                ShowScreenText(milestoneText, 
                                              (Vector2){GetScreenWidth()/2, GetScreenHeight()/2 - 50}, 
                                              25, GOLD, 3.0f, true);
                            }
                            
                            
                            game->enemiesKilledSinceBoss++;

                            
                            if (game->enemiesKilledSinceBoss >= 50 && !game->bossActive) {
                                
                                float angle = GetRandomValue(0, 360) * DEG2RAD;
                                float spawnDist = currentPlayAreaRadius + 100.0f;
                                Vector2 spawnPos = {
                                    PLAY_AREA_CENTER_X + cosf(angle) * spawnDist,
                                    PLAY_AREA_CENTER_Y + sinf(angle) * spawnDist
                                };
                                
                                
                                InitBoss(&game->boss, spawnPos);
                                game->bossActive = true;
                                game->enemiesKilledSinceBoss = 0;
                                
                                
                                game->showBossMessage = true;
                                game->bossMessageTimer = 0.0f;
                                
                                
                                const char* bossAppearText = GetBossAppearText();
                                ShowScreenText(bossAppearText, 
                                              (Vector2){GetScreenWidth()/2, GetScreenHeight()/2 - 80}, 
                                              30, RED, 3.0f, true);
                                
                                
                                PlayGameSound(game->enemyExplodeSound);
                            }

                            
                            if (game->enemiesKilled == game->nextPowerupAt) {
                                
                                float angle1 = GetRandomValue(0, 360) * DEG2RAD;
                                float angle2 = (angle1 + 120.0f) * DEG2RAD;
                                float angle3 = (angle1 + 240.0f) * DEG2RAD;
                                
                                float distance = currentPlayAreaRadius * 0.5f; 
                                
                                
                                Vector2 pos1 = {
                                    PLAY_AREA_CENTER_X + cosf(angle1) * distance,
                                    PLAY_AREA_CENTER_Y + sinf(angle1) * distance
                                };
                                
                                Vector2 pos2 = {
                                    PLAY_AREA_CENTER_X + cosf(angle2) * distance,
                                    PLAY_AREA_CENTER_Y + sinf(angle2) * distance
                                };
                                
                                Vector2 pos3 = {
                                    PLAY_AREA_CENTER_X + cosf(angle3) * distance,
                                    PLAY_AREA_CENTER_Y + sinf(angle3) * distance
                                };
                                
                                
                                AddPowerup(&game->powerups, pos1, POWERUP_DAMAGE);
                                AddPowerup(&game->powerups, pos2, POWERUP_HEAL);
                                AddPowerup(&game->powerups, pos3, POWERUP_SHIELD);
                                
                                
                                if (game->nextPowerupAt < 50) {
                                    game->nextPowerupAt += 10;
                                } else {
                                    game->nextPowerupAt += 25;
                                }
                            }

                            
                            game->score += 100;
                        } else {
                            
                            prevEnemy = currentEnemy;
                            currentEnemy = currentEnemy->next;
                        }
                        
                        break; 
                    }
                }
                prevEnemy = currentEnemy;
                if (currentEnemy) currentEnemy = currentEnemy->next;
            }
        }
        currentBullet = currentBullet->next;
    }

    
    if (game->bossActive && game->boss.active) {
        currentBullet = game->bullets;  
        
        while (currentBullet != NULL) {
            if (currentBullet->active) {
                
                if (CheckBossHitByBullet(&game->boss, currentBullet->position, currentBullet->radius, currentBullet->damage)) {
                    
                    PlayGameSound(game->enemyExplodeSound);
                    
                    
                    currentBullet->active = false;
                    
                    
                    if (!game->boss.active) {
                        game->bossActive = false;
                        game->score += 4000; 
                        
                        
                        const char* bossDefeatText = GetBossDefeatText();
                        ShowScreenText(bossDefeatText, 
                                      (Vector2){GetScreenWidth()/2, GetScreenHeight()/2 - 80}, 
                                      30, RED, 4.0f, true);
                    } 
                    else if (game->boss.isTransitioning) {
                        
                        switch (game->boss.currentLayer + 1) { 
                            case 4: game->score += 1000; break; 
                            case 3: game->score += 2000; break; 
                            case 2: game->score += 3000; break; 
                        }
                    }
                }
            }
            currentBullet = currentBullet->next;
        }
        
        
        if (!game->player.isInvincible && !game->player.isDashing) {
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                     game->boss.position, game->boss.radius * 0.9f)) {
                
                if (game->player.hasShield) {
                    
                    PlayGameSound(game->enemyExplodeSound);
                    
                    
                    game->player.hasShield = false;
                } else {
                    
                    PlayGameSound(game->playerExplodeSound);
                    game->player.lives--;
                    
                    
                    const char* damageText = GetDamageText();
                    ShowScreenText(damageText, 
                                  (Vector2){game->player.position.x, game->player.position.y - 30}, 
                                  30, RED, 1.0f, true);
                    
                    if (game->player.lives <= 0) {
                        
                        game->showGameSummary = true;
                        game->currentState = GAME_STATE_GAME_OVER;
                        return;
                    } else {
                        game->player.isInvincible = true;
                        game->player.invincibleTimer = INVINCIBILITY_TIME;
                        game->player.blinkTimer = BLINK_FREQUENCY;
                    }
                }
            }
        }
    }

    
    currentBullet = game->enemyBullets;
    while (currentBullet != NULL) {
        
        if (currentBullet->active && !game->player.isDashing) {  
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                      currentBullet->position, currentBullet->radius)) {
                
                
                if (game->player.hasShield) {
                    
                    PlayGameSound(game->playerExplodeSound); 
                    
                    
                    Vector2 repelDirection = Vector2Normalize(
                        Vector2Subtract(currentBullet->position, game->player.position)
                    );
                    
                    
                    currentBullet->velocity = Vector2Scale(repelDirection, Vector2Length(currentBullet->velocity) * 1.5f);
                    
                    
                    game->player.hasShield = false;
                } else if (!game->player.isInvincible) {  
                    
                    PlayGameSound(game->playerExplodeSound);
                    currentBullet->active = false; 
                    
                    
                    game->player.lives--;
                    
                    
                    const char* damageText = GetDamageText();
                    ShowScreenText(damageText, 
                                  (Vector2){game->player.position.x, game->player.position.y - 30}, 
                                  30, RED, 1.0f, true);
                    
                    if (game->player.lives <= 0) {
                        
                        game->showGameSummary = true;
                        game->currentState = GAME_STATE_GAME_OVER;
                        return;
                    } else {
                        
                        game->player.isInvincible = true;
                        game->player.invincibleTimer = INVINCIBILITY_TIME;
                        game->player.blinkTimer = BLINK_FREQUENCY;
                    }
                }
                
                break;
            }
        }
        currentBullet = currentBullet->next;
    }

    
    Enemy *currentEnemy = game->enemies.head;
    Enemy *prevEnemy = NULL;
    while (currentEnemy != NULL) {
        Enemy *nextEnemy = currentEnemy->next;
        
        
        if (currentEnemy->active && !game->player.isDashing) {  
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                      currentEnemy->position, currentEnemy->radius)) {
                
                
                if (game->player.hasShield) {
                    
                    PlayGameSound(game->enemyExplodeSound);
                    
                    
                    Vector2 repelDirection = Vector2Normalize(
                        Vector2Subtract(currentEnemy->position, game->player.position)
                    );
                    
                    
                    currentEnemy->velocity = Vector2Scale(repelDirection, currentEnemy->speed * 5.0f);
                    
                    
                    game->player.hasShield = false;
                    
                    
                    prevEnemy = currentEnemy;
                    currentEnemy = nextEnemy;
                    continue; 
                } else if (!game->player.isInvincible) {
                    
                    PlayGameSound(game->playerExplodeSound);
                    
                    
                    if (prevEnemy == NULL) {
                        game->enemies.head = nextEnemy;
                    } else {
                        prevEnemy->next = nextEnemy;
                    }
                    
                    free(currentEnemy);
                    game->enemies.count--;
                    
                    
                    game->player.lives--;
                    
                    
                    const char* damageText = GetDamageText();
                    ShowScreenText(damageText, 
                                  (Vector2){game->player.position.x, game->player.position.y - 30}, 
                                  30, RED, 1.0f, true);
                    
                    if (game->player.lives <= 0) {
                        
                        game->showGameSummary = true;
                        game->currentState = GAME_STATE_GAME_OVER;
                        return;
                    } else {
                        
                        game->player.isInvincible = true;
                        game->player.invincibleTimer = INVINCIBILITY_TIME;
                        game->player.blinkTimer = BLINK_FREQUENCY;
                    }
                }
                
                
                break;
            }
        }
        prevEnemy = currentEnemy;
        currentEnemy = nextEnemy;
    }
}

void InitGame(Game *game) {
    
    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    
    InitPlayArea();
    
    
    InitEnemyList(&game->enemies);
    
    
    game->bullets = NULL;
    game->enemyBullets = NULL; 
    
    
    game->score = 0;
    
    
    game->currentState = GAME_STATE_MAIN_MENU;
    
    
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 1.5f;
    game->difficultyTimer = 0.0f;
    game->shootCooldown = 0.0f;  
    
    
    LoadGameAudio(&game->shootSound, &game->enemyExplodeSound, &game->playerExplodeSound, &game->backgroundMusic);

    
    ShowCursor();
    
    
    game->enemiesKilled = 0;
    game->nextPowerupAt = 10;  
    
    
    InitPowerups(&game->powerups);
    
    
    game->increasedDamage = false;

    
    game->bossActive = false;
    game->enemiesKilledSinceBoss = 0;
    game->showBossMessage = false;
    game->bossMessageTimer = 0.0f;

    
    InitScoreboard();
    
    
    memset(game->playerName, 0, MAX_NAME_LENGTH);
    game->nameLength = 0;
    game->isHighScore = false;

    
    game->gameTime = 0.0f;
    game->showGameSummary = false;
    game->currentSortType = SORT_BY_SCORE;

    InitNarrativeText();
}

void UpdateGame(Game *game, float deltaTime) {
    switch (game->currentState) {
        case GAME_STATE_MAIN_MENU:
            
            ShowCursor();
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || 
                IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || 
                IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) || 
                IsKeyPressed(KEY_SPACE) || 
                IsKeyPressed(KEY_ENTER) ||
                GetKeyPressed() != 0) {
                
                
                game->currentState = GAME_STATE_TUTORIAL;
            }
            break;
            
        case GAME_STATE_TUTORIAL:
            
            ShowCursor();
            
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                
                ResetGame(game);
                game->currentState = GAME_STATE_PLAYING;
            }
            break;
            
        case GAME_STATE_PLAYING:
            
            {
                static GameState previousState = GAME_STATE_MAIN_MENU;
                if (previousState != GAME_STATE_PLAYING) {
                    const char* introText = GetIntroText();
                    ShowScreenText(introText, 
                                  (Vector2){GetScreenWidth()/2, GetScreenHeight()/2 - 100}, 
                                  25, WHITE, 5.0f, true);
                }
                previousState = game->currentState;
            }
            
            
            game->gameTime += deltaTime;
            
            
            HideCursor();
            
            
            if (IsKeyPressed(KEY_P)) {
                game->currentState = GAME_STATE_PAUSED;
                break; 
            }
            
            
            UpdateDynamicPlayArea(deltaTime, game->score); 
            
            
            HandleInput(game, deltaTime);
            UpdatePlayer(&game->player, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            UpdateEnemies(&game->enemies, game->player.position, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT, &game->bullets, &game->enemyBullets);
            UpdateBullets(&game->bullets, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            
            UpdateBullets(&game->enemyBullets, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            HandleCollisions(game);
            SpawnEnemy(game);
            UpdateDifficulty(game, deltaTime);

            
            UpdatePowerups(&game->powerups, deltaTime);

            
            PowerupType collectedType;
            if (CheckPowerupCollision(&game->powerups, game->player.position, game->player.radius, &collectedType)) {
                
                switch (collectedType) {
                    case POWERUP_DAMAGE:
                        
                        game->increasedDamage = true;
                        
                        
                        if (game->player.lives > 1) {
                            game->player.lives--;
                        }
                        break;
                        
                    case POWERUP_HEAL:
                        
                        game->player.lives = 3;
                        break;
                        
                    case POWERUP_SHIELD:
                        
                        game->player.hasShield = true;
                        game->player.shieldTimer = 15.0f;
                        break;
                }
            }

            
            if (game->bossActive && game->boss.active) {
                UpdateBoss(&game->boss, game->player.position, deltaTime, &game->enemyBullets);
            }

            if (game->showBossMessage) {
                game->bossMessageTimer += deltaTime;
                if (game->bossMessageTimer >= 3.0f) {
                    game->showBossMessage = false;
                }
            }
            
            
            if (GetRandomValue(0, 1800) == 1) { 
                const char* jokeText = GetRandomJokeText();
                ShowScreenText(jokeText, 
                             (Vector2){GetScreenWidth()/2, GetScreenHeight()/2 + 100}, 
                             22, SKYBLUE, 4.0f, true);
            }
            
            
            if (game->player.lives == 1 && GetRandomValue(0, 600) == 0) { 
                const char* wisdomText = GetCosmicWisdomText();
                ShowScreenText(wisdomText, 
                              (Vector2){GetScreenWidth()/2, 100}, 
                              24, PURPLE, 5.0f, true);
            }
            break;

        case GAME_STATE_PAUSED:
            
            ShowCursor();
            
            
            if (IsKeyPressed(KEY_P)) {
                game->currentState = GAME_STATE_PLAYING;
            }
            
            
            if (IsKeyPressed(KEY_M)) {
                game->currentState = GAME_STATE_MAIN_MENU;
            }
            
            
            if (IsKeyPressed(KEY_R)) {
                ResetGame(game);
                game->currentState = GAME_STATE_PLAYING;
            }
            break;

        case GAME_STATE_GAME_OVER:
            
            ShowCursor();
            
            
            if (IsKeyPressed(KEY_R)) {
                ResetGame(game);
                game->currentState = GAME_STATE_PLAYING;
            } else if (IsKeyPressed(KEY_S)) {
                
                game->currentState = GAME_STATE_ENTER_NAME;
                memset(game->playerName, 0, MAX_NAME_LENGTH);
                game->nameLength = 0;
            } else if (IsKeyPressed(KEY_M)) {
                
                game->currentState = GAME_STATE_MAIN_MENU;
            }
            
            else if (game->showGameSummary && 
                    (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || 
                     IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
                game->showGameSummary = false;
            }
            break;
            
        case GAME_STATE_ENTER_NAME: {
            
            int key = GetCharPressed();
            
            
            while (key > 0) {
                
                if ((key >= 32) && (key <= 125) && (game->nameLength < MAX_NAME_LENGTH - 1)) {
                    game->playerName[game->nameLength] = (char)key;
                    game->playerName[game->nameLength + 1] = '\0'; 
                    game->nameLength++;
                }
                
                key = GetCharPressed(); 
            }
            
            
            if (IsKeyPressed(KEY_BACKSPACE) && game->nameLength > 0) {
                game->nameLength--;
                game->playerName[game->nameLength] = '\0';
            }
            
            
            if (IsKeyPressed(KEY_ENTER) && game->nameLength > 0) {
                
                AddScore(game->playerName, game->score, game->enemiesKilled, game->gameTime);
                SaveScoreboard(); 
                
                
                game->currentState = GAME_STATE_SCOREBOARD;
            }
            
            
            
            break;
        }
            
        case GAME_STATE_SCOREBOARD:
            
            ShowCursor();
            
            
            if (IsKeyPressed(KEY_M)) {
                game->currentState = GAME_STATE_MAIN_MENU;
            }
            
            
            break;
    }

    
    UpdateScreenTexts(deltaTime);
}

void DrawGame(Game *game) {
    
    switch (game->currentState) {
        case GAME_STATE_MAIN_MENU:
            DrawMainMenu();
            DrawMinimalistCursor(); 
            break;
            
        case GAME_STATE_TUTORIAL:
            DrawTutorialScreen();
            DrawMinimalistCursor(); 
            break;
            
        case GAME_STATE_PLAYING:
            
            DrawGameplay(&game->player, &game->enemies, game->bullets, 
                         game->enemyBullets, game->powerups, game->score);
            
            
            if (game->bossActive && game->boss.active) {
                DrawBoss(&game->boss);
            }
            
            
            if (game->showBossMessage) {
                const char *message = "BOSS APARECEU!";
                int fontSize = 40;
                int textWidth = MeasureText(message, fontSize);
                
                
                DrawRectangle(GetScreenWidth()/2 - textWidth/2 - 20, 
                              GetScreenHeight()/2 - fontSize/2 - 20,
                              textWidth + 40, 
                              fontSize + 40, 
                              Fade(BLACK, 0.7f));
                              
                DrawText(message, 
                         GetScreenWidth()/2 - textWidth/2, 
                         GetScreenHeight()/2 - fontSize/2, 
                         fontSize, RED);
            }
            
            DrawScreenTexts();
            break;
            
        case GAME_STATE_PAUSED:
            
            DrawGameplay(&game->player, &game->enemies, game->bullets, 
                         game->enemyBullets, game->powerups, game->score);
            
            
            DrawPauseMenu();
            DrawMinimalistCursor(); 
            break;
            
        case GAME_STATE_GAME_OVER:
            if (game->showGameSummary) {
                
                DrawGameSummary(game->score, game->enemiesKilled, game->gameTime);
            } else {
                
                DrawGameOverScreen(game->score);
            }
            DrawMinimalistCursor(); 
            break;
            
        case GAME_STATE_ENTER_NAME:
            DrawNameEntryScreen(game);
            break;
            
        case GAME_STATE_SCOREBOARD:
            RenderScoreboardScreen();
            DrawMinimalistCursor(); 
            break;
    }
}
