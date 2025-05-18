#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "audio.h"
#include "render.h"
#include <stdlib.h>

// Declarar acesso às variáveis externas definidas em utils.c
extern float currentPlayAreaRadius;

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
    
    // Limpar projéteis inimigos
    currentBullet = game->enemyBullets;
    while (currentBullet != NULL) {
        Bullet *nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }
    game->enemyBullets = NULL;

    // Reiniciar a posição do jogador
    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Reiniciar pontuação
    game->score = 0;

    // Reiniciar timers
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 1.5f; // Intervalo inicial
    game->difficultyTimer = 0.0f;
    game->shootCooldown = 0.0f;  // Reseta o cooldown de tiro

    // Mudar o estado para jogando
    game->currentState = GAME_STATE_PLAYING;

    // Reiniciar música se necessário
    if (IsAudioDeviceReady() && game->backgroundMusic.ctxData != NULL) {
        StopMusicStream(game->backgroundMusic);
        PlayMusicStream(game->backgroundMusic);
    }
    
    // Limpar power-ups
    ClearPowerups(&game->powerups);
    
    // Resetar contadores para power-ups
    game->enemiesKilled = 0;
    game->nextPowerupAt = 10;
    game->increasedDamage = false;

    // Resetar boss
    game->bossActive = false;
    game->enemiesKilledSinceBoss = 0;
    game->showBossMessage = false;
    game->bossMessageTimer = 0.0f;
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
    // Se o boss estiver ativo, não spawna novos inimigos
    if (game->bossActive) return;

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
            case 0: // Top (acima da área jogável)
                spawnPosition = (Vector2){
                    (float)GetRandomValue((int)PLAY_AREA_LEFT, (int)PLAY_AREA_RIGHT), 
                    PLAY_AREA_TOP - radius - 10.0f
                };
                break;
            case 1: // Bottom (abaixo da área jogável)
                spawnPosition = (Vector2){
                    (float)GetRandomValue((int)PLAY_AREA_LEFT, (int)PLAY_AREA_RIGHT), 
                    PLAY_AREA_BOTTOM + radius + 10.0f
                };
                break;
            case 2: // Left (à esquerda da área jogável)
                spawnPosition = (Vector2){
                    PLAY_AREA_LEFT - radius - 10.0f, 
                    (float)GetRandomValue((int)PLAY_AREA_TOP, (int)PLAY_AREA_BOTTOM)
                };
                break;
            case 3: // Right (à direita da área jogável)
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
    // Se o boss estiver ativo, não spawna novos inimigos
    if (game->bossActive) return;

    // Calcular quantos inimigos faltam para atingir o máximo
    int enemiesToSpawn = MAX_ENEMIES - game->enemies.count;
    
    for (int i = 0; i < enemiesToSpawn; i++) {
        // Lógica para escolher tipo de inimigo (manter igual)
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

        // Para posição de spawn, gerar um ângulo aleatório e posicionar fora do círculo
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float spawnDistance = PLAY_AREA_RADIUS + radius + 20.0f; // Um pouco além da borda
        
        Vector2 spawnPosition = {
            PLAY_AREA_CENTER_X + cosf(angle) * spawnDistance,
            PLAY_AREA_CENTER_Y + sinf(angle) * spawnDistance
        };
        
        AddEnemy(&game->enemies, spawnPosition, radius, speed, WHITE, type);
    }
}

void HandleInput(Game *game, float deltaTime) {
    // Atualizar o cooldown de tiro
    if (game->shootCooldown > 0) {
        game->shootCooldown -= deltaTime;
    }

    // Atirar quando o botão esquerdo do mouse é pressionado OU segurado
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && game->shootCooldown <= 0) {
        Vector2 mousePos = GetMousePosition();
        Vector2 direction = {0};
        
        // Calculando direction com funções do raymath.h
        Vector2 diff = Vector2Subtract(mousePos, game->player.position);
        direction = Vector2Normalize(diff);
        
        // A posição inicial do projétil deve ser o centro do jogador
        Vector2 bulletStartPosition = game->player.position;

        if (game->increasedDamage) {
            // Versão com dano aumentado (projétil maior)
            AddBulletWithProps(&game->bullets, bulletStartPosition, direction, BULLET_RADIUS * 1.5f, 2);
        } else {
            // Versão normal
            AddBullet(&game->bullets, bulletStartPosition, direction);
        }
        PlayGameSound(game->shootSound);
        
        // Definir cooldown para o próximo tiro (0.2 segundos é uma boa cadência)
        game->shootCooldown = 0.2f;
    }
}

void HandleCollisions(Game *game) {
    // Colisão Projétil -> Inimigo
    Bullet *currentBullet = game->bullets;
    while (currentBullet != NULL) {
        if (currentBullet->active) {
            // Verificação de colisão com inimigos regulares (código existente)
            Enemy *currentEnemy = game->enemies.head;
            Enemy *prevEnemy = NULL;
            while (currentEnemy != NULL) {
                if (currentEnemy->active) {
                    if (CheckCollisionCircles(currentBullet->position, currentBullet->radius,
                                              currentEnemy->position, currentEnemy->radius)) {
                        PlayGameSound(game->enemyExplodeSound);
                        currentBullet->active = false; // Projétil é consumido
                        
                        // Usar o dano do projétil (padrão é 1)
                        currentEnemy->health -= currentBullet->damage;
                        
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
                            
                            // Incrementar contador de inimigos mortos
                            game->enemiesKilled++;
                            
                            // Incrementar contador para spawn do boss
                            game->enemiesKilledSinceBoss++;

                            // Verificar se é hora de spawnar o boss
                            if (game->enemiesKilledSinceBoss >= 50 && !game->bossActive) {
                                // Calcular posição de spawn (fora da área visível)
                                float angle = GetRandomValue(0, 360) * DEG2RAD;
                                float spawnDist = currentPlayAreaRadius + 100.0f;
                                Vector2 spawnPos = {
                                    PLAY_AREA_CENTER_X + cosf(angle) * spawnDist,
                                    PLAY_AREA_CENTER_Y + sinf(angle) * spawnDist
                                };
                                
                                // Inicializar o boss
                                InitBoss(&game->boss, spawnPos);
                                game->bossActive = true;
                                game->enemiesKilledSinceBoss = 0;
                                
                                // Mostrar mensagem
                                game->showBossMessage = true;
                                game->bossMessageTimer = 0.0f;
                                
                                // Som especial para o boss
                                PlayGameSound(game->enemyExplodeSound); // Ou um som específico para o boss
                            }

                            // Verificar se atingiu um milestone para poder spawnar power-ups
                            if (game->enemiesKilled == game->nextPowerupAt) {
                                // Calcular posições para os três power-ups equidistantes
                                float angle1 = GetRandomValue(0, 360) * DEG2RAD;
                                float angle2 = (angle1 + 120.0f) * DEG2RAD;
                                float angle3 = (angle1 + 240.0f) * DEG2RAD;
                                
                                float distance = currentPlayAreaRadius * 0.5f; // 50% do raio da área
                                
                                // Posições para os três power-ups
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
                                
                                // Adicionar os três tipos de power-up
                                AddPowerup(&game->powerups, pos1, POWERUP_DAMAGE);
                                AddPowerup(&game->powerups, pos2, POWERUP_HEAL);
                                AddPowerup(&game->powerups, pos3, POWERUP_SHIELD);
                                
                                // Atualizar para o próximo milestone
                                if (game->nextPowerupAt < 50) {
                                    game->nextPowerupAt += 10;
                                } else {
                                    game->nextPowerupAt += 25;
                                }
                            }

                            // Adicionar os pontos originais
                            game->score += 100;
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

    // Verificar colisão entre projéteis do jogador e o boss
    if (game->bossActive && game->boss.active) {
        currentBullet = game->bullets;  // Reinicializar currentBullet para reutilizar
        
        while (currentBullet != NULL) {
            if (currentBullet->active) {
                // Usar a função específica para verificar colisão com o boss
                if (CheckBossHitByBullet(&game->boss, currentBullet->position, currentBullet->radius, currentBullet->damage)) {
                    // Efeito sonoro
                    PlayGameSound(game->enemyExplodeSound);
                    
                    // Desativar projétil
                    currentBullet->active = false;
                    
                    // Verificar se o boss foi derrotado totalmente ou apenas uma camada
                    if (!game->boss.active) {
                        game->bossActive = false;
                        game->score += 4000; // Pontuação final por derrotar completamente o boss
                    } 
                    else if (game->boss.isTransitioning) {
                        // Conceder pontos conforme a camada destruída
                        switch (game->boss.currentLayer + 1) { // +1 porque já decrementamos
                            case 4: game->score += 1000; break; // Primeira camada (hexágono)
                            case 3: game->score += 2000; break; // Segunda camada (quadrado)
                            case 2: game->score += 3000; break; // Terceira camada (círculo)
                        }
                    }
                }
            }
            currentBullet = currentBullet->next;
        }
        
        // Colisão entre jogador e boss (apenas se o jogador não estiver invencível ou em dash)
        if (!game->player.isInvincible && !game->player.isDashing) {
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                     game->boss.position, game->boss.radius * 0.9f)) {
                // Verificar se o jogador tem escudo
                if (game->player.hasShield) {
                    // Efeito sonoro de escudo
                    PlayGameSound(game->enemyExplodeSound);
                    
                    // Desativar o escudo após absorver um ataque
                    game->player.hasShield = false;
                } else {
                    // Dano ao jogador
                    PlayGameSound(game->playerExplodeSound);
                    game->player.lives--;
                    
                    if (game->player.lives <= 0) {
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

    // Colisão Projétil Inimigo -> Jogador
    currentBullet = game->enemyBullets;
    while (currentBullet != NULL) {
        // MODIFICAR: Ignorar colisões se o jogador estiver em dash
        if (currentBullet->active && !game->player.isDashing) {  // Adicionar verificação de dash
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                      currentBullet->position, currentBullet->radius)) {
                
                // Verificar se o jogador tem escudo
                if (game->player.hasShield) {
                    // Efeito sonoro de escudo
                    PlayGameSound(game->playerExplodeSound); // Pode ser substituído por um som específico de escudo
                    
                    // Repelir o projétil em vez de destruí-lo
                    Vector2 repelDirection = Vector2Normalize(
                        Vector2Subtract(currentBullet->position, game->player.position)
                    );
                    
                    // Duplicar a velocidade para um efeito mais dramático
                    currentBullet->velocity = Vector2Scale(repelDirection, Vector2Length(currentBullet->velocity) * 1.5f);
                    
                    // Desativar o escudo após repelir um ataque
                    game->player.hasShield = false;
                } else if (!game->player.isInvincible) {  // Adicione a verificação de invencibilidade AQUI
                    // Comportamento normal apenas se não tiver escudo E não estiver invencível
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
        Enemy *nextEnemy = currentEnemy->next;
        
        // MODIFICAR: Ignorar colisões se o jogador estiver em dash
        if (currentEnemy->active && !game->player.isDashing) {  // Adicionar verificação de dash
            if (CheckCollisionCircles(game->player.position, game->player.radius,
                                      currentEnemy->position, currentEnemy->radius)) {
                
                // Verificar se o jogador tem escudo
                if (game->player.hasShield) {
                    // Efeito sonoro de escudo
                    PlayGameSound(game->enemyExplodeSound);
                    
                    // Repelir o inimigo
                    Vector2 repelDirection = Vector2Normalize(
                        Vector2Subtract(currentEnemy->position, game->player.position)
                    );
                    
                    // Aplicar uma força de repulsão mais forte
                    currentEnemy->velocity = Vector2Scale(repelDirection, currentEnemy->speed * 5.0f);
                    
                    // Desativar o escudo após repelir um ataque
                    game->player.hasShield = false;
                    
                    // IMPORTANTE: Adicione este prevEnemy = currentEnemy para atualizar o ponteiro
                    prevEnemy = currentEnemy;
                    currentEnemy = nextEnemy;
                    continue; // Pula para o próximo inimigo sem executar o código abaixo
                } else if (!game->player.isInvincible) {
                    // Comportamento normal quando não tem escudo E não está invencível
                    PlayGameSound(game->playerExplodeSound);
                    
                    // Remover o inimigo que colidiu com o jogador
                    if (prevEnemy == NULL) {
                        game->enemies.head = nextEnemy;
                    } else {
                        prevEnemy->next = nextEnemy;
                    }
                    
                    free(currentEnemy);
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
                }
                
                // Se houve colisão, não precisa verificar mais
                break;
            }
        }
        prevEnemy = currentEnemy;
        currentEnemy = nextEnemy;
    }
}

void InitGame(Game *game) {
    // Inicializar o jogador
    InitPlayer(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Inicializar a área de jogo com tamanho adequado
    InitPlayArea();
    
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
    game->enemySpawnInterval = 1.5f;
    game->difficultyTimer = 0.0f;
    game->shootCooldown = 0.0f;  // Inicializa o cooldown de tiro
    
    // Inicializar áudio
    LoadGameAudio(&game->shootSound, &game->enemyExplodeSound, &game->playerExplodeSound, &game->backgroundMusic);

    // Por padrão mostra o cursor (para o menu inicial)
    ShowCursor();
    
    // Inicializar contadores para power-ups
    game->enemiesKilled = 0;
    game->nextPowerupAt = 10;  // Primeiro power-up após 10 inimigos
    
    // Inicializar lista de power-ups
    InitPowerups(&game->powerups);
    
    // Estado inicial do dano
    game->increasedDamage = false;

    // Inicializar boss
    game->bossActive = false;
    game->enemiesKilledSinceBoss = 0;
    game->showBossMessage = false;
    game->bossMessageTimer = 0.0f;
}

void UpdateGame(Game *game, float deltaTime) {
    UpdateGameMusicStream(game->backgroundMusic);

    switch (game->currentState) {
        case GAME_STATE_MAIN_MENU:
            // Mostrar cursor normal no menu
            ShowCursor();
            // Modificar para ir para o tutorial em vez do jogo diretamente
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || 
                IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || 
                IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) || 
                IsKeyPressed(KEY_SPACE) || 
                IsKeyPressed(KEY_ENTER) ||
                GetKeyPressed() != 0) {
                
                // Ir para o tutorial em vez do jogo diretamente
                game->currentState = GAME_STATE_TUTORIAL;
            }
            break;
            
        case GAME_STATE_TUTORIAL:
            // Mostrar cursor normal no tutorial
            ShowCursor();
            // Passar para o jogo ao pressionar espaço
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                // Inicializar o jogo quando sair do tutorial
                ResetGame(game);
                game->currentState = GAME_STATE_PLAYING;
            }
            break;
            
        case GAME_STATE_PLAYING:
            // Esconder cursor durante o jogo
            HideCursor();
            
            // Verificar tecla de pausa
            if (IsKeyPressed(KEY_P)) {
                game->currentState = GAME_STATE_PAUSED;
                break; // Sair do case sem processar o resto do jogo
            }
            
            // Atualizar a área de jogo dinâmica - remover o valor 0
            UpdateDynamicPlayArea(deltaTime, game->score); // Passar a pontuação real para potenciais usos futuros
            
            // Resto do código existente
            HandleInput(game, deltaTime);
            UpdatePlayer(&game->player, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            UpdateEnemies(&game->enemies, game->player.position, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT, &game->bullets, &game->enemyBullets);
            UpdateBullets(&game->bullets, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            // Atualizar também os projéteis dos inimigos
            UpdateBullets(&game->enemyBullets, deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
            HandleCollisions(game);
            SpawnEnemy(game);
            UpdateDifficulty(game, deltaTime);

            // Atualizar power-ups
            UpdatePowerups(&game->powerups, deltaTime);

            // Verificar colisão do jogador com power-ups
            PowerupType collectedType;
            if (CheckPowerupCollision(&game->powerups, game->player.position, game->player.radius, &collectedType)) {
                // Aplicar efeito baseado no tipo de power-up coletado
                switch (collectedType) {
                    case POWERUP_DAMAGE:
                        // Aumentar dano (aplicado ao gerar projéteis)
                        game->increasedDamage = true;
                        
                        // Reduzir uma vida (se tiver mais que 1)
                        if (game->player.lives > 1) {
                            game->player.lives--;
                        }
                        break;
                        
                    case POWERUP_HEAL:
                        // Restaurar todas as vidas (máximo 3)
                        game->player.lives = 3;
                        break;
                        
                    case POWERUP_SHIELD:
                        // Ativar o escudo por 15 segundos
                        game->player.hasShield = true;
                        game->player.shieldTimer = 15.0f;
                        break;
                }
            }

            // Atualizar boss
            if (game->bossActive && game->boss.active) {
                UpdateBoss(&game->boss, game->player.position, deltaTime, &game->enemyBullets);
            }

            if (game->showBossMessage) {
                game->bossMessageTimer += deltaTime;
                if (game->bossMessageTimer >= 3.0f) {
                    game->showBossMessage = false;
                }
            }
            break;

        case GAME_STATE_PAUSED:
            // Mostrar cursor no menu de pausa
            ShowCursor();
            
            // Voltar para o jogo
            if (IsKeyPressed(KEY_P)) {
                game->currentState = GAME_STATE_PLAYING;
            }
            
            // Voltar para o menu principal
            if (IsKeyPressed(KEY_M)) {
                game->currentState = GAME_STATE_MAIN_MENU;
            }
            
            // Reiniciar jogo
            if (IsKeyPressed(KEY_R)) {
                ResetGame(game);
                game->currentState = GAME_STATE_PLAYING;
            }
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
            
        case GAME_STATE_TUTORIAL:
            DrawTutorialScreen();
            break;
            
        case GAME_STATE_PLAYING:
            // Desenhar gameplay normal
            DrawGameplay(&game->player, &game->enemies, game->bullets, game->enemyBullets, game->powerups, game->score);
            
            // Desenhar boss se ativo
            if (game->bossActive && game->boss.active) {
                DrawBoss(&game->boss);
            }
            
            // Mostrar mensagem do boss se necessário
            if (game->showBossMessage) {
                float alpha = fminf(1.0f, game->bossMessageTimer) * 
                            fminf(1.0f, (3.0f - game->bossMessageTimer));
                
                const char *message = "BOSS APPROACHING";
                int fontSize = 60;
                int textWidth = MeasureText(message, fontSize);
                
                DrawText(message, 
                        GetScreenWidth()/2 - textWidth/2, 
                        GetScreenHeight()/2 - fontSize/2, 
                        fontSize, 
                        Fade(RED, alpha));
            }
            break;
            
        case GAME_STATE_PAUSED:
            // Primeiro desenhar o jogo normalmente
            DrawGameplay(&game->player, &game->enemies, game->bullets, game->enemyBullets, game->powerups, game->score);
            
            // Desenhar boss se ativo
            if (game->bossActive && game->boss.active) {
                DrawBoss(&game->boss);
            }
            
            // Depois desenhar o menu de pausa por cima
            DrawPauseMenu();
            break;
            
        case GAME_STATE_GAME_OVER:
            DrawGameOverScreen(game->score);
            break;
    }
}
