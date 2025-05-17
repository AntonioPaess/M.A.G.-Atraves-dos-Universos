#include "game.h"
#include "raylib.h"
#include "raymath.h"  // Adicionando include para funções Vector2
#include "utils.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "audio.h"
#include "render.h" // Incluindo para acesso às funções de renderização
#include <stdlib.h> // Para NULL e free()

void ResetGame(Game *game) {
    // Limpar todos os inimigos
    Enemy *currentEnemy = game->enemies.head;
    while (currentEnemy != NULL) {
        Enemy *nextEnemy = currentEnemy->next;
        free(currentEnemy);
        currentEnemy = nextEnemy;
    }
    game->enemies.head = NULL;
    game->enemies.count = 0;

    // Limpar todos os projéteis
    Bullet *currentBullet = game->bullets;
    while (currentBullet != NULL) {
        Bullet *nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }
    game->bullets = NULL;

    // Reiniciar a posição do jogador
    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Reiniciar pontuação
    game->score = 0;

    // Reiniciar timers
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 1.5f; // Intervalo inicial
    game->difficultyTimer = 0.0f;

    // Mudar o estado para jogando
    game->currentState = GAME_STATE_PLAYING;

    // Reiniciar música se necessário
    if (IsAudioDeviceReady() && game->backgroundMusic.ctxData != NULL) {
        StopMusicStream(game->backgroundMusic);
        PlayMusicStream(game->backgroundMusic);
    }
}

void UpdateDifficulty(Game *game, float deltaTime) {
    game->difficultyTimer += deltaTime;
    // A cada 10 segundos, aumenta a dificuldade (diminui o intervalo de spawn)
    if (game->difficultyTimer > 10.0f) {
        game->enemySpawnInterval *= 0.90f; // Diminui o intervalo em 10%
        if (game->enemySpawnInterval < 0.2f) { // Limite mínimo para o intervalo
            game->enemySpawnInterval = 0.2f;
        }
        game->difficultyTimer = 0.0f; // Reseta o timer de dificuldade
        // printf("Dificuldade aumentada! Novo intervalo de spawn: %.2f s\n", game->enemySpawnInterval);
    }
}

void SpawnEnemy(Game *game) {
    game->enemySpawnTimer += GetFrameTime();
    if (game->enemySpawnTimer >= game->enemySpawnInterval) {
        game->enemySpawnTimer = 0.0f;

        Vector2 spawnPosition;
        int side = GetRandomValue(0, 3); // 0: top, 1: bottom, 2: left, 3: right

        // Determinar o tipo de inimigo baseado em probabilidade e score
        EnemyType type;
        int randomType = GetRandomValue(1, 100);
        
        // Progressivamente introduzir inimigos mais difíceis com base na pontuação
        if (game->score < 1000) {
            // No início, apenas inimigos normais
            type = ENEMY_TYPE_NORMAL;
        } 
        else if (game->score < 2000) {
            // Entre 1000-2000, introduz shooters e tanks (reduzindo shooters)
            if (randomType <= 50) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 60) type = ENEMY_TYPE_SHOOTER; // Reduzido de 30% para 10%
            else if (randomType <= 85) type = ENEMY_TYPE_TANK;
            else type = ENEMY_TYPE_EXPLODER;
        }
        else if (game->score < 3000) {
            // Entre 2000-3000, introduz speeders (reduzindo shooters)
            if (randomType <= 35) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 45) type = ENEMY_TYPE_SHOOTER; // Reduzido de 20% para 10%
            else if (randomType <= 75) type = ENEMY_TYPE_TANK;
            else if (randomType <= 90) type = ENEMY_TYPE_EXPLODER;
            else type = ENEMY_TYPE_SPEEDER;
        }
        else {
            // Fase avançada acima de 3000 pontos (reduzindo shooters)
            if (randomType <= 20) type = ENEMY_TYPE_NORMAL;
            else if (randomType <= 25) type = ENEMY_TYPE_SHOOTER; // Reduzido de 20% para 5%
            else if (randomType <= 55) type = ENEMY_TYPE_TANK;
            else if (randomType <= 85) type = ENEMY_TYPE_EXPLODER;
            else type = ENEMY_TYPE_SPEEDER;
        }
        
        // Ajustar raio e velocidade com base no tipo
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
            default: // ENEMY_TYPE_NORMAL
                radius = GetRandomValue(ENEMY_RADIUS_MIN, ENEMY_RADIUS_MAX);
                speed = GetRandomValue(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) + (game->score / 1000.0f);
                break;
        }
        
        // Limitar velocidade máxima
        if (speed > ENEMY_SPEED_MAX * 2) speed = ENEMY_SPEED_MAX * 2;

        // Posição de spawn baseada no lado
        switch (side) {
            case 0: // Top
                spawnPosition = (Vector2){(float)GetRandomValue(0, SCREEN_WIDTH), -radius};
                break;
            case 1: // Bottom
                spawnPosition = (Vector2){(float)GetRandomValue(0, SCREEN_WIDTH), SCREEN_HEIGHT + radius};
                break;
            case 2: // Left
                spawnPosition = (Vector2){-radius, (float)GetRandomValue(0, SCREEN_HEIGHT)};
                break;
            case 3: // Right
                spawnPosition = (Vector2){SCREEN_WIDTH + radius, (float)GetRandomValue(0, SCREEN_HEIGHT)};
                break;
        }
        
        AddEnemy(&game->enemies, spawnPosition, radius, speed, WHITE, type);
    }
}

void HandleInput(Game *game, float deltaTime) {
    // Movimento do jogador é tratado em UpdatePlayer

    // Atirar
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        Vector2 direction = {0}; // Inicializando direction com zeros
        
        // Calculando direction com funções do raymath.h
        Vector2 diff = Vector2Subtract(mousePos, game->player.position);
        direction = Vector2Normalize(diff);
        
        // A posição inicial do projétil deve ser o centro do jogador
        Vector2 bulletStartPosition = game->player.position;

        AddBullet(&game->bullets, bulletStartPosition, direction);
        PlayGameSound(game->shootSound);
    }
}

void HandleCollisions(Game *game) {
    // Colisão Projétil -> Inimigo
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
                        currentBullet->active = false; // Projétil é consumido
                        
                        // Reduzir vida do inimigo
                        currentEnemy->health--;
                        
                        // Se o inimigo foi destruído
                        if (currentEnemy->health <= 0) {
                            // Se for um exploder, gerar projéteis em todas as direções
                            if (currentEnemy->type == ENEMY_TYPE_EXPLODER) {
                                for (int i = 0; i < 8; i++) {
                                    float angle = i * (2.0f * PI / 8.0f);
                                    Vector2 direction = {cosf(angle), sinf(angle)};
                                    AddBullet(&game->enemyBullets, currentEnemy->position, direction);
                                }
                            }
                            
                            // Remover o inimigo
                            Enemy* toRemove = currentEnemy;
                            currentEnemy = currentEnemy->next;

                            if (toRemove->prev == NULL) {
                                // É o primeiro da lista
                                game->enemies.head = toRemove->next;
                                if (game->enemies.head != NULL) {
                                    game->enemies.head->prev = NULL;
                                }
                            } else {
                                // Não é o primeiro
                                toRemove->prev->next = toRemove->next;
                                if (toRemove->next != NULL) {
                                    toRemove->next->prev = toRemove->prev;
                                }
                            }

                            free(toRemove);
                            game->enemies.count--;
                            
                            game->score += 100; // Adiciona pontos
                        } else {
                            // Inimigo ainda tem vida
                            prevEnemy = currentEnemy;
                            currentEnemy = currentEnemy->next;
                        }
                        
                        break; // Projétil só pode atingir um inimigo
                    }
                }
                prevEnemy = currentEnemy;
                if (currentEnemy) currentEnemy = currentEnemy->next;
            }
        }
        currentBullet = currentBullet->next;
    }

    // Adicionar verificação para projéteis de inimigos atingindo o jogador
    currentBullet = game->enemyBullets;
    while (currentBullet != NULL) {
        if (currentBullet->active && !game->player.isInvincible) {
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                      currentBullet->position, currentBullet->radius)) {
                PlayGameSound(game->playerExplodeSound);
                currentBullet->active = false; // Desativar o projétil
                
                // Reduzir vida
                game->player.lives--;
                
                if (game->player.lives <= 0) {
                    // Game over se não tiver mais vidas
                    game->currentState = GAME_STATE_GAME_OVER;
                    return;
                } else {
                    // Ativar invencibilidade temporária
                    game->player.isInvincible = true;
                    game->player.invincibleTimer = INVINCIBILITY_TIME;
                    game->player.blinkTimer = BLINK_FREQUENCY;
                }
                
                break;
            }
        }
        currentBullet = currentBullet->next;
    }

    // Colisão Inimigo -> Jogador
    Enemy *currentEnemy = game->enemies.head;
    Enemy *prevEnemy = NULL;
    while (currentEnemy != NULL) {
        if (currentEnemy->active && !game->player.isInvincible) {
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                      currentEnemy->position, currentEnemy->radius)) {
                PlayGameSound(game->playerExplodeSound);
                
                // Remover o inimigo que colidiu com o jogador
                Enemy* toRemove = currentEnemy;
                currentEnemy = currentEnemy->next;
                
                if (prevEnemy == NULL) {
                    game->enemies.head = toRemove->next;
                } else {
                    prevEnemy->next = toRemove->next;
                }
                
                free(toRemove);
                game->enemies.count--;
                
                // Reduzir vida
                game->player.lives--;
                
                if (game->player.lives <= 0) {
                    // Game over se não tiver mais vidas
                    game->currentState = GAME_STATE_GAME_OVER;
                    return;
                } else {
                    // Ativar invencibilidade temporária
                    game->player.isInvincible = true;
                    game->player.invincibleTimer = INVINCIBILITY_TIME;
                    game->player.blinkTimer = BLINK_FREQUENCY;
                }
                
                break;
            }
        }
        prevEnemy = currentEnemy;
        if (currentEnemy) currentEnemy = currentEnemy->next;
    }
}

void InitGame(Game *game) {
    // Inicializar o jogador
    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Inicializar lista de inimigos
    InitEnemyList(&game->enemies);
    
    // Inicializar lista de projéteis
    game->bullets = NULL;
    game->enemyBullets = NULL; // Inicializa a lista de projéteis de inimigos
    
    // Inicializar pontuação
    game->score = 0;
    
    // Inicializar estado do jogo - começa no menu principal
    game->currentState = GAME_STATE_MAIN_MENU;
    
    // Inicializar timers
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 1.5f; // Começa com 1.5 segundos entre spawns
    game->difficultyTimer = 0.0f;
    
    // Inicializar áudio
    LoadGameAudio(&game->shootSound, &game->enemyExplodeSound, &game->playerExplodeSound, &game->backgroundMusic);

    // Por padrão mostra o cursor (para o menu inicial)
    ShowCursor();
}

void UpdateGame(Game *game, float deltaTime) {
    UpdateGameMusicStream(game->backgroundMusic);

    switch (game->currentState) {
        case GAME_STATE_MAIN_MENU:
            // Mostrar cursor normal no menu
            ShowCursor();
            // Verificar cliques nos botões
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                
                // Opção "INICIAR JOGO"
                Rectangle startButton = {
                    GetScreenWidth()/2 - MeasureText("INICIAR JOGO", 30)/2,
                    GetScreenHeight()/2 + 100,
                    MeasureText("INICIAR JOGO", 30),
                    30
                };
                
                // Opção "SAIR"
                Rectangle exitButton = {
                    GetScreenWidth()/2 - MeasureText("SAIR", 30)/2,
                    GetScreenHeight()/2 + 160,
                    MeasureText("SAIR", 30),
                    30
                };
                
                if (CheckCollisionPointRec(mousePos, startButton)) {
                    // Iniciar o jogo
                    ResetGame(game);
                    game->currentState = GAME_STATE_PLAYING;
                } else if (CheckCollisionPointRec(mousePos, exitButton)) {
                    // Sair do jogo
                    CloseWindow();
                }
            }
            break;
            
        case GAME_STATE_PLAYING:
            // Esconder cursor durante o jogo (a mira será desenhada em render.c)
            HideCursor();
            // Código existente para o jogo
            HandleInput(game, deltaTime);
            UpdatePlayer(&game->player, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            UpdateEnemies(&game->enemies, game->player.position, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT, &game->bullets, &game->enemyBullets);
            UpdateBullets(&game->bullets, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            // Atualizar também os projéteis dos inimigos
            UpdateBullets(&game->enemyBullets, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            HandleCollisions(game);
            SpawnEnemy(game);
            UpdateDifficulty(game, deltaTime);
            break;

        case GAME_STATE_GAME_OVER:
            // Mostrar cursor normal na tela de game over
            ShowCursor();
            if (IsKeyPressed(KEY_R)) {
                ResetGame(game);
            }
            // Opção para voltar ao menu principal
            if (IsKeyPressed(KEY_M)) {
                game->currentState = GAME_STATE_MAIN_MENU;
            }
            break;
    }
}

// Apenas a função DrawGame
void DrawGame(const Game *game) {
    switch (game->currentState) {
        case GAME_STATE_MAIN_MENU:
            DrawMainMenu();
            break;
            
        case GAME_STATE_PLAYING:
            DrawGameplay(&game->player, &game->enemies, game->bullets, game->enemyBullets, game->score);
            break;
            
        case GAME_STATE_GAME_OVER:
            DrawGameOverScreen(game->score);
            break;
    }
}
