#include "render.h"
#include "raylib.h"
#include "raymath.h"
#include "powerup.h" // Adicione esta linha
#include <stdio.h>
#include <math.h>

// Estrutura para gerenciar explosões
typedef struct {
    Vector2 position;
    float radius;
    float lifeTime;
    float currentTime;
    Color color;
    bool active;
    float rotation;
} Explosion;

// Array de explosões ativas
#define MAX_EXPLOSIONS 32
static Explosion explosions[MAX_EXPLOSIONS] = {0};

// Declarações forward de todas as funções auxiliares
void DrawPixelLine(float x1, float y1, float x2, float y2, Color color);
void DrawPixelCircle(float centerX, float centerY, float radius, Color color);
void DrawPixelCircleV(Vector2 center, float radius, Color color);
void DrawPixelText(const char *text, int posX, int posY, int fontSize, Color color);
void DrawPixelRect(float x, float y, float width, float height, Color color);
void DrawMinimalistCursor(void);
void DrawPlayAreaBorder(void);  // Nova declaração adicionada aqui

// Função auxiliar para desenhar linhas pixeladas (escopo de arquivo)
void DrawPixelLine(float x1, float y1, float x2, float y2, Color color) {
    Vector2 start = {x1, y1};
    Vector2 end = {x2, y2};
    Vector2 delta = {end.x - start.x, end.y - start.y};
    float length = sqrtf(delta.x * delta.x + delta.y * delta.y);
    Vector2 dir = {delta.x / length, delta.y / length};
    
    // Tamanho de cada "pixel" na linha
    float pixelSize = 2.0f;
    int pixelCount = (int)(length / pixelSize);
    
    for (int i = 0; i < pixelCount; i++) {
        float x = start.x + dir.x * i * pixelSize;
        float y = start.y + dir.y * i * pixelSize;
        DrawRectangle(x, y, pixelSize, pixelSize, color);
    }
}

// Não há mais InitRenderResources ou UnloadRenderResources se não usarmos fontes customizadas

// Função para desenhar o jogo durante a gameplay
void DrawGameplay(const Player *player, const EnemyList *enemies, const Bullet *bullets, const Bullet *enemyBullets, const Powerup *powerups, long score) {
    // Desenhar a borda da área de jogo primeiro
    DrawPlayAreaBorder();
    
    // Desenhar jogador como um círculo minimalista
    if (player && player->visible) {
        // Efeito de pulsação sutil
        static float pulseTime = 0.0f;
        pulseTime += GetFrameTime() * 2.0f;
        float pulseFactor = 1.0f + sinf(pulseTime) * 0.05f;
        
        // Círculo externo (branco)
        DrawPixelCircleV(player->position, player->radius * pulseFactor, WHITE);
        
        // Círculo interno (preto - cria o efeito de anel)
        DrawPixelCircleV(player->position, player->radius * 0.7f * pulseFactor, BLACK);
        
        // Desenhar escudo se ativo
        if (player->hasShield) {
            float shieldPulse = 1.0f + sinf(pulseTime * 1.5f) * 0.1f;
            float shieldRatio = player->shieldTimer / 15.0f; // 15 segundos é o tempo total
            Color shieldColor = Fade(BLUE, 0.3f + 0.2f * shieldRatio);
            
            // Escudo externo (azul, semi-transparente)
            DrawPixelCircleV(player->position, player->radius * 1.5f * shieldPulse, shieldColor);
            
            // Linha rotativa do escudo para mostrar que está ativo
            static float shieldRotation = 0.0f;
            shieldRotation += GetFrameTime() * 2.0f;
            
            for (int i = 0; i < 8; i++) {
                float angle = shieldRotation + i * (PI/4);
                Vector2 shieldPoint = {
                    player->position.x + cosf(angle) * player->radius * 1.5f * shieldPulse,
                    player->position.y + sinf(angle) * player->radius * 1.5f * shieldPulse
                };
                DrawPixelCircleV(shieldPoint, 3.0f, BLUE);
            }
        }
        
        // Adicionar efeito de rastro se estiver em dash
        if (player->isDashing) {
            // Rastro de dash com várias bolinhas vermelhas
            for (int i = 1; i <= 8; i++) { // Aumentado para 8 bolinhas para mais densidade
                float alpha = 0.8f - (i * 0.09f); // Ajustado para manter visibilidade
                
                // Calcular posição base do rastro
                Vector2 baseTrailPos = Vector2Subtract(
                    player->position, 
                    Vector2Scale(player->dashDirection, player->radius * i * 0.6f)
                );
                
                // Círculo principal no rastro
                DrawPixelCircleV(baseTrailPos, player->radius * (0.7f - i * 0.05f), Fade(RED, alpha));
                
                // Bolinhas adicionais ao redor do rastro principal (efeito de partículas)
                if (i < 6) { // Apenas nas primeiras posições para não ficar excessivo
                    for (int j = 0; j < 3; j++) {
                        float angle = GetTime() * 5.0f + i * 0.5f + j * 2.0f; // Ângulo variável para movimento
                        float offset = player->radius * 0.4f * (1.0f - i * 0.1f); // Distância diminui com o tempo
                        
                        Vector2 particlePos = {
                            baseTrailPos.x + cosf(angle) * offset,
                            baseTrailPos.y + sinf(angle) * offset
                        };
                        
                        // Tamanho variável para as bolinhas secundárias
                        float particleSize = player->radius * (0.3f - i * 0.03f);
                        DrawPixelCircleV(particlePos, particleSize, Fade(RED, alpha * 0.8f));
                    }
                }
            }
            
            // Aura em volta do jogador (agora vermelha)
            DrawPixelCircleV(player->position, player->radius * 1.3f, Fade(RED, 0.3f));
        }
    }

    // Substituir todo o bloco de desenho de inimigos por uma chamada à nossa função especializada
    DrawEnemies(enemies);

    // Desenhar projéteis como círculos simples
    if (bullets) {
        const Bullet *currentBullet = bullets;
        while (currentBullet) {
            if (currentBullet->active) {
                // Projétil do jogador: círculo branco simples
                DrawPixelCircleV(currentBullet->position, currentBullet->radius, WHITE);
                
                // Pequeno rastro (minimalista)
                Vector2 trail = Vector2Subtract(currentBullet->position, 
                                           Vector2Scale(currentBullet->velocity, 0.02f));
                DrawPixelCircleV(trail, currentBullet->radius * 0.6f, (Color){255, 255, 255, 120});
            }
            currentBullet = currentBullet->next;
        }
    }

    // Modificação dos projéteis inimigos - substitua a seção existente (linhas ~88-116)
    if (enemyBullets) {
        const Bullet *currentBullet = enemyBullets;
        while (currentBullet) {
            if (currentBullet->active) {
                // Posição do projétil
                Vector2 pos = currentBullet->position;
                float radius = currentBullet->radius;
                
                // Animação de rotação para o quadrado
                static float rotationTime = 0.0f;
                rotationTime += GetFrameTime() * 4.0f;
                float rotation = rotationTime + pos.x * 0.01f; // Variação por posição
                
                // Tamanho do quadrado projetil inimigo
                float squareSize = radius * 1.8f;
                
                // Desenhar um quadrado rotativo como projétil inimigo
                Color enemyBulletColor = RED; // Cor vermelha para destaque
                
                // Calcular os quatro cantos do quadrado rotacionado
                for (int i = 0; i < 4; i++) {
                    float angle1 = rotation + i * (PI / 2);
                    float angle2 = rotation + ((i + 1) % 4) * (PI / 2);
                    
                    Vector2 corner1 = {
                        pos.x + cosf(angle1) * squareSize * 0.7f,
                        pos.y + sinf(angle1) * squareSize * 0.7f
                    };
                    
                    Vector2 corner2 = {
                        pos.x + cosf(angle2) * squareSize * 0.7f,
                        pos.y + sinf(angle2) * squareSize * 0.7f
                    };
                    
                    // Desenhar o lado do quadrado
                    DrawLineEx(corner1, corner2, radius * 0.4f, enemyBulletColor);
                }
                
                // Centro do projétil
                DrawCircleV(pos, radius * 0.5f, enemyBulletColor);
                
                // Rastro do projétil (mais dramático que o do jogador)
                Vector2 trail = Vector2Subtract(pos, Vector2Scale(currentBullet->velocity, 0.03f));
                DrawCircleV(trail, radius * 0.4f, Fade(enemyBulletColor, 0.6f));
                Vector2 trail2 = Vector2Subtract(pos, Vector2Scale(currentBullet->velocity, 0.06f));
                DrawCircleV(trail2, radius * 0.3f, Fade(enemyBulletColor, 0.3f));
            }
            currentBullet = currentBullet->next;
        }
    }

    // Desenhar power-ups
    if (powerups) {
        const Powerup *currentPowerup = powerups;
        while (currentPowerup) {
            if (currentPowerup->active) {
                // Efeito de pulsação
                float pulse = 0.8f + sinf(GetTime() * 3.0f) * 0.2f;
                
                // Core baseada no tipo
                Color color;
                if (currentPowerup->type == POWERUP_DAMAGE) {
                    color = RED;  // Vermelho para dano aumentado
                } else if (currentPowerup->type == POWERUP_HEAL) {
                    color = GREEN;  // Verde para cura
                } else {
                    color = BLUE;  // Azul para escudo
                }
                
                // Desenhar círculo externo
                DrawPixelCircleV(currentPowerup->position, currentPowerup->radius * pulse, color);
                
                // Desenhar círculo interno
                DrawPixelCircleV(currentPowerup->position, currentPowerup->radius * 0.7f * pulse, BLACK);
                
                // Símbolo interno
                if (currentPowerup->type == POWERUP_DAMAGE) {
                    // Símbolo de "+" para dano
                    DrawPixelLine(
                        currentPowerup->position.x - currentPowerup->radius * 0.4f,
                        currentPowerup->position.y,
                        currentPowerup->position.x + currentPowerup->radius * 0.4f,
                        currentPowerup->position.y,
                        color
                    );
                    DrawPixelLine(
                        currentPowerup->position.x,
                        currentPowerup->position.y - currentPowerup->radius * 0.4f,
                        currentPowerup->position.x,
                        currentPowerup->position.y + currentPowerup->radius * 0.4f,
                        color
                    );
                } else if (currentPowerup->type == POWERUP_HEAL) {
                    // Símbolo de coração para cura
                    DrawPixelCircleV(
                        currentPowerup->position, 
                        currentPowerup->radius * 0.3f * pulse, 
                        color
                    );
                } else {
                    // Símbolo de escudo (círculo com raios)
                    float shieldRadius = currentPowerup->radius * 0.3f * pulse;
                    DrawPixelCircleV(currentPowerup->position, shieldRadius, color);
                    
                    // Raios do escudo
                    for (int i = 0; i < 4; i++) {
                        float angle = GetTime() * 2.0f + i * (PI/2);
                        float rayLength = currentPowerup->radius * 0.5f * pulse;
                        
                        Vector2 rayEnd = {
                            currentPowerup->position.x + cosf(angle) * rayLength,
                            currentPowerup->position.y + sinf(angle) * rayLength
                        };
                        
                        DrawPixelLine(
                            currentPowerup->position.x, 
                            currentPowerup->position.y,
                            rayEnd.x, 
                            rayEnd.y, 
                            color
                        );
                    }
                }
                
                // Efeito de brilho externo
                DrawPixelCircleV(
                    currentPowerup->position, 
                    currentPowerup->radius * 1.2f * pulse, 
                    Fade(color, 0.3f)
                );
            }
            currentPowerup = currentPowerup->next;
        }
    }

    // ===== HUD APRIMORADO =====
    
    // Área de fundo com gradiente sutil
    Color topColor = Fade(BLACK, 0.85f);
    Color bottomColor = Fade(BLACK, 0.5f);
    
    for (int y = 0; y < 60; y++) {
        float alpha = Lerp(topColor.a, bottomColor.a, y/60.0f);
        DrawPixelRect(0, y, GetScreenWidth(), 1, Fade(BLACK, alpha));
    }
    
    // Separador central estilizado
    float centerX = GetScreenWidth() / 2;
    for (int i = 10; i < 50; i += 3) {
        DrawPixelCircle(centerX, 30, 1, Fade(WHITE, 0.1f + (i % 2) * 0.1f));
    }
    
    // === SEÇÃO ESQUERDA: VIDAS (MELHORADA) ===
    
    // Indicador de vidas com ícones mais elaborados e maiores
    static float heartPulse = 0.0f;
    heartPulse += GetFrameTime() * 1.2f;
    
    // Desenhar texto "LIVES" maior e mais destacado
    DrawPixelText("LIVES", 20, 24, 30, Fade(WHITE, 0.9f));
    
    // Largura do texto "LIVES" para posicionar os corações ao lado
    int livesTextWidth = MeasureText("LIVES", 30);
    
    // Contêiner para as vidas ao lado do texto
    DrawPixelRect(30 + livesTextWidth, 20, player->lives * 42 + 10, 28, Fade(WHITE, 0.15f));
    
    // Corações pulsantes maiores representando vidas, agora ao lado do texto
    for (int i = 0; i < player->lives; i++) {
        float pulse = 0.8f + sinf(heartPulse + i * 0.8f) * 0.2f;
        Color lifeColor = RED; // Alterado para vermelho
        
        // Último coração pisca quando o jogador está invencível
        if (i == player->lives - 1 && player->isInvincible) {
            float blinkRate = sinf(GetTime() * 8.0f);
            lifeColor = Fade(RED, (blinkRate > 0) ? 0.7f : 0.3f); // Manter vermelho mesmo ao piscar
        }
        
        // Desenhar coração estilizado maior (dois círculos e um triângulo invertido)
        float heartX = 50 + livesTextWidth + i * 40; // Posicionado após o texto LIVES
        float heartY = 34; // Mesma altura do texto LIVES
        float heartSize = 9.0f * pulse; // Corações maiores
        
        // Círculos maiores para formar o coração
        DrawPixelCircle(heartX - heartSize/2, heartY, heartSize, lifeColor);
        DrawPixelCircle(heartX + heartSize/2, heartY, heartSize, lifeColor);
        
        // Parte triangular mais visível
        DrawPixelLine(heartX - heartSize, heartY, heartX, heartY + heartSize*1.5, lifeColor);
        DrawPixelLine(heartX + heartSize, heartY, heartX, heartY + heartSize*1.5, lifeColor);
        
        // Adicionar efeito de brilho nas extremidades
        DrawPixelCircle(heartX, heartY + heartSize*1.5, 2, Fade(WHITE, 0.7f * pulse));
    }
    
    // === SEÇÃO CENTRAL: PONTUAÇÃO ===
    
    // Pontuação com efeito pulsante quando aumenta
    static long lastScore = 0;
    static float scoreFlash = 0.0f;
    
    if (score > lastScore) {
        scoreFlash = 1.0f;
        lastScore = score;
    }
    
    scoreFlash = scoreFlash > 0.0f ? scoreFlash - GetFrameTime() * 2.0f : 0.0f;
    
    // Texto de pontuação com brilho quando aumenta
    const char* scoreText = TextFormat("%ld", score);
    Color scoreColor = WHITE;
    
    if (scoreFlash > 0.0f) {
        scoreColor = Fade(WHITE, 0.7f + scoreFlash * 0.3f); // Efeito de brilho mantendo monocromático
    }
    
    DrawPixelText(scoreText, centerX - MeasureText(scoreText, 38)/2, 15, 38, scoreColor);
    
    // Rótulo "SCORE" menor e mais sutil abaixo
    DrawPixelText("SCORE", centerX - MeasureText("SCORE", 16)/2, 45, 16, Fade(LIGHTGRAY, 0.6f));
    
    // === SEÇÃO DIREITA: INIMIGOS ===
    
    if (enemies) {
        // Contador de inimigos com ícone
        const char* enemiesText = TextFormat("%d", enemies->count);
        int textWidth = MeasureText(enemiesText, 28);
        int rightAlign = GetScreenWidth() - 20 - textWidth;
        
        // Desenhar texto com alinhamento à direita
        DrawPixelText(enemiesText, rightAlign, 20, 28, Fade(WHITE, 0.9f));
        
        // Desenhar ícone de inimigo (círculo simples)
        DrawPixelCircle(rightAlign - 20, 30, 8, Fade(RED, 0.7f)); // Alterado para vermelho
        
        // Rótulo "ENEMIES" embaixo
        DrawPixelText("ENEMIES", GetScreenWidth() - MeasureText("ENEMIES", 16) - 20, 45, 16, Fade(LIGHTGRAY, 0.6f));
    }
    
    // Borda inferior do HUD com efeito de gradiente
    for (int x = 0; x < GetScreenWidth(); x += 2) {
        float brightness = 0.4f + sinf(x * 0.01f) * 0.1f;
        DrawPixelRect(x, 59, 2, 2, Fade(WHITE, brightness * 0.3f));
    }

    // === BARRA DE COOLDOWN DO DASH NA PARTE INFERIOR ===
    // Mostrar a barra de cooldown do dash na parte inferior da tela
    float dashCooldownRatio = player->dashCooldown / DASH_COOLDOWN;

    // Configurações da barra
    int barWidth = 200;
    int barHeight = 10;
    int barX = GetScreenWidth() / 2 - barWidth / 2;
    int barY = GetScreenHeight() - 30;
    
    // Cor pulsante baseada no cooldown
    Color dashBarColor = RED;
    float alpha = 0.7f + sinf(GetTime() * 4.0f) * 0.3f;
    
    // Texto informativo "DASH"
    DrawPixelText("DASH", barX, barY - 15, 16, Fade(WHITE, 0.6f));
    
    // Contorno da barra (fundo)
    DrawPixelRect(barX - 2, barY - 2, barWidth + 4, barHeight + 4, Fade(WHITE, 0.3f));
    
    // Fundo escuro da barra
    DrawPixelRect(barX, barY, barWidth, barHeight, Fade(BLACK, 0.6f));
    
    // Parte preenchida da barra (representa disponibilidade do dash)
    if (!player->isDashing) {
        int fillWidth = barWidth * (1.0f - dashCooldownRatio);
        DrawPixelRect(barX, barY, fillWidth, barHeight, Fade(dashBarColor, alpha));
        
        // Brilho no fim da barra para indicar progresso
        if (fillWidth > 0 && fillWidth < barWidth) {
            DrawPixelRect(barX + fillWidth - 3, barY, 6, barHeight, Fade(WHITE, alpha * 0.8f));
        }
    } else {
        // Efeito especial durante o dash (barra piscante)
        DrawPixelRect(barX, barY, barWidth, barHeight, Fade(RED, alpha * 1.5f));
    }
    
    // Indicador de disponibilidade - mostrar READY quando estiver carregado
    if (dashCooldownRatio <= 0.0f && !player->isDashing) {
        DrawPixelText("READY", barX + barWidth - MeasureText("READY", 14), barY - 15, 14, 
                     Fade(RED, 0.8f + sinf(GetTime() * 5.0f) * 0.2f));
    }

    // Cursor pixelado
    DrawMinimalistCursor();
}

// Menu principal melhorado com animações
void DrawMainMenu(void) {
    ClearBackground(BLACK);
    
    // Variáveis de animação
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    
    // Efeito de partículas no fundo
    for (int i = 0; i < 50; i++) {
        float x = sinf(animTime * 0.5f + i * 0.3f) * GetScreenWidth() * 0.5f + GetScreenWidth() * 0.5f;
        float y = cosf(animTime * 0.3f + i * 0.2f) * GetScreenHeight() * 0.5f + GetScreenHeight() * 0.5f;
        float size = 2.0f + sinf(animTime + i) * 1.5f;
        
        // Cores alternadas entre vermelho e branco
        Color particleColor = (i % 3 == 0) ? 
                             Fade(RED, 0.2f + sinf(animTime + i) * 0.1f) : 
                             Fade(WHITE, 0.1f + sinf(animTime + i * 0.7f) * 0.05f);
                             
        DrawPixelCircle(x, y, size, particleColor);
    }
    
    // Borda animada similar à área de jogo
    float borderRadius = fminf(GetScreenWidth(), GetScreenHeight()) * 0.4f;
    
    for (float angle = 0; angle < 360.0f; angle += 5.0f) {
        float rad = angle * DEG2RAD;
        float pulseEffect = sinf(animTime * 2.0f + angle * 0.05f) * 10.0f;
        float finalRadius = borderRadius + pulseEffect;
        
        Vector2 pointOnCircle = {
            GetScreenWidth() / 2 + cosf(rad) * finalRadius,
            GetScreenHeight() / 2 + sinf(rad) * finalRadius
        };
        
        Color pointColor = (int)(angle + animTime * 30.0f) % 30 < 15 ? RED : WHITE;
        DrawPixelCircle(pointOnCircle.x, pointOnCircle.y, 2.0f, Fade(pointColor, 0.3f + sinf(animTime + angle * 0.01f) * 0.1f));
    }
    
    // Título com efeito de pulsação e glow
    const char *title = "M.A.G.";
    int titleWidth = MeasureText(title, 120);
    
    // Glow externo (várias camadas para efeito de brilho)
    float titlePulse = 1.0f + sinf(animTime * 3.0f) * 0.1f;
    for (int i = 10; i > 0; i -= 2) {
        DrawText(title, 
                GetScreenWidth()/2 - titleWidth/2 - i, 
                GetScreenHeight()/3 - i, 
                120, 
                Fade(RED, 0.05f * i * titlePulse));
    }
    
    // Título principal
    DrawText(title, 
            GetScreenWidth()/2 - titleWidth/2, 
            GetScreenHeight()/3, 
            120, 
            Fade(WHITE, 0.8f + sinf(animTime * 4.0f) * 0.2f));
    
    // Subtítulo com fade
    const char *subtitle = "O inimigo agora é outro";
    int subtitleWidth = MeasureText(subtitle, 30);
    DrawText(subtitle, 
            GetScreenWidth()/2 - subtitleWidth/2, 
            GetScreenHeight()/3 + 100, 
            30, 
            Fade(WHITE, 0.5f + sinf(animTime * 2.0f) * 0.2f));
    
    // Opção para iniciar com animação
    const char *startOption = "PRESS ANY KEY TO START";  // Texto alterado
    int startWidth = MeasureText(startOption, 30);
    
    // Efeito de pulsação
    float startPulse = 0.6f + sinf(animTime * 5.0f) * 0.4f;
    DrawText(startOption, 
            GetScreenWidth()/2 - startWidth/2, 
            GetScreenHeight()/2 + 200, 
            30, 
            Fade(RED, startPulse));
    
    // Cursor minimalista
    DrawMinimalistCursor();
}

// Game over com efeitos de explosão e animações
void DrawGameOverScreen(long finalScore) {
    ClearBackground(BLACK);
    
    // Controle de animação
    static float animTime = 0.0f;
    static float explosionRadius = 0.0f;
    static float scoreCounter = 0.0f;
    static bool animInit = false;
    
    // Inicializar animação
    if (!animInit) {
        animTime = 0.0f;
        explosionRadius = 0.0f;
        scoreCounter = 0.0f;
        animInit = true;
    }
    
    // Atualizar contadores com incremento menor
    animTime += GetFrameTime() * 0.8f; // Reduzido para menor carga de CPU
    
    // Efeito de explosão pixelada expandindo do centro (simplificado)
    if (explosionRadius < GetScreenWidth() * 1.2f) {
        explosionRadius = GetScreenWidth() * (1.0f - expf(-animTime * 2.0f));
    }
    
    // Contador animado da pontuação (simplificado)
    if (scoreCounter < finalScore) {
        scoreCounter = fminf(finalScore, scoreCounter + finalScore * GetFrameTime() * 1.5f);
    }
    
    // Desenhar ondas de explosão (reduzido de 3 para 1)
    float waveTime = animTime;
    float waveRadius = explosionRadius * 0.8f;
    float alpha = expf(-waveTime * 1.2f) * 0.4f;
    
    DrawPixelCircle(
        GetScreenWidth() / 2,
        GetScreenHeight() / 2,
        waveRadius,
        Fade(RED, alpha)
    );
    
    // Partículas espalhadas (reduzido de 40 para 15)
    for (int i = 0; i < 15; i++) {
        float angle = i * 24.0f; // Maior espaçamento
        float speed = 50.0f + (i % 3) * 25.0f; // Menos variações
        float distance = animTime * speed;
        
        if (distance < GetScreenWidth()) {
            float x = GetScreenWidth()/2 + cosf(angle * DEG2RAD) * distance;
            float y = GetScreenHeight()/2 + sinf(angle * DEG2RAD) * distance;
            
            // Simplificados
            float size = 4.0f;
            float alpha = 0.8f - distance / GetScreenWidth();
            
            DrawPixelCircle(x, y, size, Fade((i % 2 == 0) ? RED : WHITE, alpha));
        }
    }

    // Texto "GAME OVER" com efeito de shake simplificado
    const char* gameOverText = "GAME OVER";
    int gameOverWidth = MeasureText(gameOverText, 80);
    
    // Efeito de tremor reduzido
    float shakeAmount = 5.0f * expf(-animTime * 2.0f);
    float offsetX = sinf(animTime * 10.0f) * shakeAmount; // Frequência reduzida
    float offsetY = cosf(animTime * 12.0f) * shakeAmount;
    
    // Remover camadas de glow e usar apenas uma
    DrawText(gameOverText, 
            GetScreenWidth()/2 - gameOverWidth/2 + offsetX - 4, 
            GetScreenHeight()/3 + offsetY - 4, 
            80, 
            Fade(RED, 0.2f));
    
    // Texto principal
    DrawText(gameOverText, 
            GetScreenWidth()/2 - gameOverWidth/2 + offsetX, 
            GetScreenHeight()/3 + offsetY, 
            80, 
            WHITE); // Simplificado sem fade dinâmico

    // Pontuação - texto único sem efeitos de glow
    const char* scoreText = TextFormat("%ld", (long)scoreCounter);
    int scoreWidth = MeasureText(scoreText, 100);
    
    DrawText(scoreText, 
            GetScreenWidth()/2 - scoreWidth/2, 
            GetScreenHeight()/2, 
            100, 
            WHITE);

    // Instruções com efeito pulsante simplificado
    const char* restartText = "PRESS R TO RESTART";
    float restartPulse = 0.6f + sinf(animTime * 2.0f) * 0.4f; // Frequência reduzida
    
    if (animTime > 1.0f) {
        DrawText(restartText, 
                GetScreenWidth()/2 - MeasureText(restartText, 24)/2, 
                GetScreenHeight() - 140, // Ajustado para cima para dar espaço
                24, 
                Fade(RED, restartPulse));
                
        // Nova opção para voltar ao menu principal
        const char* menuText = "PRESS M FOR MAIN MENU";
        float menuPulse = 0.6f + sinf(animTime * 2.0f + 0.5f) * 0.4f; // Pulso ligeiramente fora de fase
        
        DrawText(menuText, 
                GetScreenWidth()/2 - MeasureText(menuText, 24)/2, 
                GetScreenHeight() - 100, 
                24, 
                Fade(WHITE, menuPulse));
    }

    // Cursor minimalista
    DrawMinimalistCursor();
    
    // Resetar a animação se a tela for trocada
    if (IsKeyPressed(KEY_R)) {
        animInit = false;
    }
}





void DrawMinimalistCursor(void) {
    HideCursor();
    Vector2 mousePos = GetMousePosition();
    Color cursorColor = WHITE;
    
    // Configurações de animação
    static float animTime = 0.0f;
    animTime += GetFrameTime() * 2.0f;
    
    // Tamanho maior para o quadrado com efeito de pulsação
    float baseSize = 18.0f; // Aumentado de 14 para 18
    float pulseEffect = sinf(animTime) * 1.5f;
    float size = baseSize + pulseEffect;
    
    // Desenhar um quadrado não totalmente fechado (quatro cantos)
    float cornerSize = size / 3.0f; // Tamanho dos cantos
    
    // Canto superior esquerdo
    DrawPixelLine(mousePos.x - size/2, mousePos.y - size/2, mousePos.x - size/2 + cornerSize, mousePos.y - size/2, cursorColor);
    DrawPixelLine(mousePos.x - size/2, mousePos.y - size/2, mousePos.x - size/2, mousePos.y - size/2 + cornerSize, cursorColor);
    
    // Canto superior direito
    DrawPixelLine(mousePos.x + size/2 - cornerSize, mousePos.y - size/2, mousePos.x + size/2, mousePos.y - size/2, cursorColor);
    DrawPixelLine(mousePos.x + size/2, mousePos.y - size/2, mousePos.x + size/2, mousePos.y - size/2 + cornerSize, cursorColor);
    
    // Canto inferior esquerdo
    DrawPixelLine(mousePos.x - size/2, mousePos.y + size/2 - cornerSize, mousePos.x - size/2, mousePos.y + size/2, cursorColor);
    DrawPixelLine(mousePos.x - size/2, mousePos.y + size/2, mousePos.x - size/2 + cornerSize, mousePos.y + size/2, cursorColor);
    
    // Canto inferior direito
    DrawPixelLine(mousePos.x + size/2 - cornerSize, mousePos.y + size/2, mousePos.x + size/2, mousePos.y + size/2, cursorColor);
    DrawPixelLine(mousePos.x + size/2, mousePos.y + size/2 - cornerSize, mousePos.x + size/2, mousePos.y + size/2, cursorColor);
    
    // Animação do X central - rotação leve
    float crossSize = size / 4.0f;
    float rotation = sinf(animTime * 0.5f) * 0.2f; // Rotação sutil
    
    // Calcular pontos do X com rotação
    float cs = cosf(rotation);
    float sn = sinf(rotation);
    
    // Primeiro traço do X
    float x1 = -crossSize * cs - (-crossSize) * sn;
    float y1 = -crossSize * sn + (-crossSize) * cs;
    float x2 = crossSize * cs - crossSize * sn;
    float y2 = crossSize * sn + crossSize * cs;
    
    // Segundo traço do X
    float x3 = crossSize * cs - (-crossSize) * sn;
    float y3 = crossSize * sn + (-crossSize) * cs;
    float x4 = -crossSize * cs - crossSize * sn;
    float y4 = -crossSize * sn + crossSize * cs;
    
    // Desenhar os traços do X com o efeito pixelado
    DrawPixelLine(mousePos.x + x1, mousePos.y + y1, mousePos.x + x2, mousePos.y + y2, cursorColor);
    DrawPixelLine(mousePos.x + x3, mousePos.y + y3, mousePos.x + x4, mousePos.y + y4, cursorColor);
}
// Adicione estas funções logo após a função DrawPixelLine

// Função para desenhar círculos pixelados
void DrawPixelCircle(float centerX, float centerY, float radius, Color color) {
    // Tamanho do pixel (quanto maior, mais "pixelado")
    const float pixelSize = 3.0f;
    
    // Calcular limites do círculo
    int minX = (int)((centerX - radius) / pixelSize) * pixelSize;
    int minY = (int)((centerY - radius) / pixelSize) * pixelSize;
    int maxX = (int)((centerX + radius) / pixelSize) * pixelSize + pixelSize;
    int maxY = (int)((centerY + radius) / pixelSize) * pixelSize + pixelSize;
    
    // Para cada "pixel" na área do círculo, verificar se está dentro do círculo
    for (float x = minX; x <= maxX; x += pixelSize) {
        for (float y = minY; y <= maxY; y += pixelSize) {
            // Testar se o centro do "pixel" está dentro do círculo
            float dx = (x + pixelSize/2) - centerX;
            float dy = (y + pixelSize/2) - centerY;
            if (dx*dx + dy*dy <= radius*radius) {
                DrawRectangle(x, y, pixelSize, pixelSize, color);
            }
        }
    }
}

// Versão para Vector2
void DrawPixelCircleV(Vector2 center, float radius, Color color) {
    DrawPixelCircle(center.x, center.y, radius, color);
}

// Função para desenhar texto pixelado
void DrawPixelText(const char *text, int posX, int posY, int fontSize, Color color) {
    // Desenhar o texto normal, mas em uma resolução menor (para ficar pixelado)
    const float scaleFactor = 0.8f;
    int scaledFontSize = (int)(fontSize * scaleFactor);
    
    // Desenhar o texto em posições "snapped" para o grid de pixels
    const float pixelGrid = 2.0f;
    int snapX = (int)(posX / pixelGrid) * pixelGrid;
    int snapY = (int)(posY / pixelGrid) * pixelGrid;
    
    DrawText(text, snapX, snapY, scaledFontSize, color);
}

// Função para desenhar retângulos pixelados
void DrawPixelRect(float x, float y, float width, float height, Color color) {
    const float pixelSize = 3.0f;
    
    int minX = (int)(x / pixelSize) * pixelSize;
    int minY = (int)(y / pixelSize) * pixelSize;
    int maxX = (int)((x + width) / pixelSize) * pixelSize;
    int maxY = (int)((y + height) / pixelSize) * pixelSize;
    
    for (float px = minX; px <= maxX; px += pixelSize) {
        for (float py = minY; py <= maxY; py += pixelSize) {
            DrawRectangle(px, py, pixelSize, pixelSize, color);
        }
    }
}

// Implementação das formas de inimigos em render.c
// Adicionar após a verificação de inimigos ativos

void DrawEnemies(const EnemyList *enemies) {
    if (enemies) {
        const Enemy *currentEnemy = enemies->head;
        while (currentEnemy != NULL) {
            if (currentEnemy->active) {
                // Propriedades base
                float radius = currentEnemy->radius;
                Vector2 pos = currentEnemy->position;
                
                // Efeito de pulsação
                static float pulseTime = 0.0f;
                pulseTime += GetFrameTime() * 2.0f;
                float pulseFactor = 1.0f + sinf(pulseTime + currentEnemy->position.x * 0.01f) * 0.1f;
                
                // Design específico por tipo de inimigo
                switch (currentEnemy->type) {
                    case ENEMY_TYPE_NORMAL:
                        // Inimigo Normal - Quadrado pixelado com cantos arredondados
                        {
                            // Tamanho do quadrado
                            float squareSize = radius * 1.8f * pulseFactor;
                            
                            // Desenhar o quadrado como uma série de retângulos pixelados
                            DrawPixelRect(pos.x - squareSize/2, pos.y - squareSize/2, squareSize, squareSize, WHITE);
                            DrawPixelRect(pos.x - squareSize*0.7f/2, pos.y - squareSize*0.7f/2, squareSize*0.7f, squareSize*0.7f, BLACK);
                            DrawPixelRect(pos.x - squareSize*0.4f/2, pos.y - squareSize*0.4f/2, squareSize*0.4f, squareSize*0.4f, WHITE);
                            
                            // Ponto central pixelado
                            DrawPixelCircleV(pos, radius * 0.15f, WHITE);
                        }
                        break;
                        
                    case ENEMY_TYPE_SPEEDER:
                        // Speeder - Versão pixelada de um foguete
                        {
                            // Direção baseada na velocidade
                            Vector2 dir = Vector2Normalize(currentEnemy->velocity);
                            if (Vector2Length(currentEnemy->velocity) < 0.1f) {
                                dir = (Vector2){0, -1}; 
                            }
                            
                            // Corpo do foguete (círculo principal azul)
                            DrawPixelCircleV(pos, radius * pulseFactor, SKYBLUE);
                            DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, BLACK);
                            
                            // Ponta do foguete
                            Vector2 tipPos = Vector2Add(pos, Vector2Scale(dir, radius * 0.9f));
                            DrawPixelCircleV(tipPos, radius * 0.4f, SKYBLUE);
                            
                            // Asas laterais
                            Vector2 perpendicular = (Vector2){-dir.y, dir.x};
                            
                            // Asa esquerda
                            Vector2 leftWing = Vector2Add(pos, Vector2Scale(perpendicular, radius * 0.8f));
                            DrawPixelCircleV(leftWing, radius * 0.3f, SKYBLUE);
                            
                            // Asa direita
                            Vector2 rightWing = Vector2Subtract(pos, Vector2Scale(perpendicular, radius * 0.8f));
                            DrawPixelCircleV(rightWing, radius * 0.3f, SKYBLUE);
                            
                            // Efeito de propulsão (chamas pixeladas)
                            if (Vector2Length(currentEnemy->velocity) > 50.0f) {
                                Vector2 exhaustPos = Vector2Subtract(pos, Vector2Scale(dir, radius * 0.9f));
                                float exhaustPulse = 0.7f + sinf(GetTime() * 10.0f) * 0.3f;
                                
                                DrawPixelCircleV(exhaustPos, radius * 0.5f * exhaustPulse, Fade(SKYBLUE, 0.7f));
                                DrawPixelCircleV(Vector2Subtract(exhaustPos, Vector2Scale(dir, radius * 0.3f)), 
                                              radius * 0.3f * exhaustPulse, Fade(SKYBLUE, 0.5f));
                                DrawPixelCircleV(Vector2Subtract(exhaustPos, Vector2Scale(dir, radius * 0.6f)), 
                                              radius * 0.2f * exhaustPulse, Fade(SKYBLUE, 0.3f));
                            }
                        }
                        break;
                        
                    case ENEMY_TYPE_TANK:
                        // Tank - Versão pixelada mais robusta
                        {
                            // Corpo principal
                            DrawPixelCircleV(pos, radius * 1.3f * pulseFactor, DARKGRAY);
                            DrawPixelCircleV(pos, radius * 0.9f * pulseFactor, BLACK);
                            DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, GRAY);
                            
                            // Torre - usando pontos pixelados em vez de octágono suave
                            float towerRotation = GetTime() * 0.5f;
                            int towerPoints = 8;
                            
                            for (int i = 0; i < towerPoints; i++) {
                                float angle = towerRotation + i * (2.0f * PI / towerPoints);
                                Vector2 point = {
                                    pos.x + cosf(angle) * radius * 0.6f * pulseFactor,
                                    pos.y + sinf(angle) * radius * 0.6f * pulseFactor
                                };
                                
                                DrawPixelCircleV(point, radius * 0.15f, DARKGRAY);
                            }
                            
                            // Canhão principal pixelado
                            Vector2 dir = Vector2Normalize(currentEnemy->velocity);
                            if (Vector2Length(currentEnemy->velocity) < 0.1f) {
                                dir = (Vector2){1, 0};
                            }
                            
                            // Desenhar o canhão como uma série de círculos pixelados
                            for (float t = 0; t <= 1.0f; t += 0.2f) {
                                Vector2 pointOnCannon = {
                                    pos.x + dir.x * radius * 1.5f * t,
                                    pos.y + dir.y * radius * 1.5f * t
                                };
                                float pointSize = radius * 0.25f * (1.0f - t * 0.3f);
                                DrawPixelCircleV(pointOnCannon, pointSize, DARKGRAY);
                            }
                            
                            // Ponta do canhão
                            Vector2 cannonTip = Vector2Add(pos, Vector2Scale(dir, radius * 1.5f));
                            DrawPixelCircleV(cannonTip, radius * 0.15f, LIGHTGRAY);
                            
                            // Trilhas de tanque pixeladas
                            Vector2 perp = (Vector2){-dir.y, dir.x};
                            
                            // Trilha esquerda
                            for (float t = -0.5f; t <= 0.5f; t += 0.2f) {
                                Vector2 trackPoint = {
                                    pos.x + dir.x * radius * t - perp.x * radius * 0.8f,
                                    pos.y + dir.y * radius * t - perp.y * radius * 0.8f
                                };
                                DrawPixelCircleV(trackPoint, radius * 0.12f, DARKGRAY);
                            }
                            
                            // Trilha direita
                            for (float t = -0.5f; t <= 0.5f; t += 0.2f) {
                                Vector2 trackPoint = {
                                    pos.x + dir.x * radius * t + perp.x * radius * 0.8f,
                                    pos.y + dir.y * radius * t + perp.y * radius * 0.8f
                                };
                                DrawPixelCircleV(trackPoint, radius * 0.12f, DARKGRAY);
                            }
                            
                            // Núcleo pixelado
                            DrawPixelCircleV(pos, radius * 0.2f, LIGHTGRAY);
                        }
                        break;
                        
                    case ENEMY_TYPE_EXPLODER:
                        // Exploder - Versão pixelada da estrela explosiva
                        {
                            // Corpo base
                            DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, RED);
                            DrawPixelCircleV(pos, radius * 0.5f * pulseFactor, BLACK);
                            
                            // Pontas da estrela como pontos pixelados
                            int spikes = 8;
                            float spikeAngle = 2.0f * PI / spikes;
                            float rotation = GetTime() * 3.0f;
                            
                            for (int i = 0; i < spikes; i++) {
                                float angle = rotation + i * spikeAngle;
                                float spikePulse = 0.8f + sinf(GetTime() * 5.0f + i) * 0.2f;
                                
                                // Desenhar linhas pixeladas para as pontas
                                Vector2 innerPoint = {
                                    pos.x + cosf(angle) * radius * 0.5f,
                                    pos.y + sinf(angle) * radius * 0.5f
                                };
                                
                                Vector2 outerPoint = {
                                    pos.x + cosf(angle) * radius * 1.3f * spikePulse,
                                    pos.y + sinf(angle) * radius * 1.3f * spikePulse
                                };
                                
                                DrawPixelLine(innerPoint.x, innerPoint.y, outerPoint.x, outerPoint.y, RED);
                                DrawPixelCircleV(outerPoint, radius * 0.15f, RED);
                            }
                            
                            // Núcleo instável pixelado
                            float corePulse = 0.7f + sinf(GetTime() * 8.0f) * 0.3f;
                            DrawPixelCircleV(pos, radius * 0.3f * corePulse, RED);
                            
                            // Brilho de perigo
                            if (corePulse > 0.9f) {
                                DrawPixelCircleV(pos, radius * pulseFactor * 1.5f, Fade(RED, corePulse * 0.2f));
                            }
                        }
                        break;
                        
                    case ENEMY_TYPE_SHOOTER:
                        // Shooter - Versão pixelada do canhão giratório
                        {
                            // Corpo central pixelado
                            DrawPixelCircleV(pos, radius * 0.8f * pulseFactor, YELLOW);
                            DrawPixelCircleV(pos, radius * 0.65f * pulseFactor, BLACK);
                            DrawPixelCircleV(pos, radius * 0.5f * pulseFactor, YELLOW);
                            
                            // Canhões giratórios pixelados
                            float rotationSpeed = GetTime() * 1.5f + currentEnemy->shootTimer * 3.0f;
                            int numCannons = 5;
                            
                            for (int i = 0; i < numCannons; i++) {
                                float angle = rotationSpeed + i * (2.0f * PI / numCannons);
                                
                                Vector2 cannonBase = {
                                    pos.x + cosf(angle) * radius * 0.5f,
                                    pos.y + sinf(angle) * radius * 0.5f
                                };
                                
                                Vector2 cannonTip = {
                                    pos.x + cosf(angle) * (radius * 1.1f * pulseFactor),
                                    pos.y + sinf(angle) * (radius * 1.1f * pulseFactor)
                                };
                                
                                // Desenhar canhão como linha pixelada
                                DrawPixelLine(cannonBase.x, cannonBase.y, cannonTip.x, cannonTip.y, YELLOW);
                                
                                // Ponta do canhão com brilho
                                if (currentEnemy->shootTimer > 0.5f) {
                                    float chargeRatio = (currentEnemy->shootTimer - 0.5f) / 0.5f;
                                    DrawPixelCircleV(cannonTip, radius * 0.18f * chargeRatio, 
                                                 Fade(WHITE, 0.4f + chargeRatio * 0.6f));
                                }
                            }
                            
                            // Núcleo com efeito de carga
                            float corePulse = 0.7f;
                            if (currentEnemy->shootTimer > 0.3f) {
                                corePulse = 0.7f + sinf(GetTime() * 8.0f) * 0.3f * 
                                          ((currentEnemy->shootTimer - 0.3f) / 0.7f);
                            }
                            
                            DrawPixelCircleV(pos, radius * 0.35f * corePulse, Fade(YELLOW, 0.7f + corePulse * 0.3f));
                            
                            // Efeito de carga pixelado
                            if (currentEnemy->shootTimer > 0.7f) {
                                float chargeRatio = (currentEnemy->shootTimer - 0.7f) / 0.3f;
                                DrawPixelCircleV(pos, radius * pulseFactor * (1.0f + chargeRatio * 0.2f), 
                                             Fade(YELLOW, 0.2f * chargeRatio));
                            }
                        }
                        break;
                }
            }
            else if (currentEnemy->isDying) {
                DrawEnemyDeathAnimation(currentEnemy);  // Mudou de DrawEnemyDeathAnimationPixelated para DrawEnemyDeathAnimation
            }
            
            currentEnemy = currentEnemy->next;
        }
    }
}

// Versão pixelada da animação de morte
void DrawEnemyDeathAnimation(const Enemy *enemy) {  // Mudou de DrawEnemyDeathAnimationPixelated para DrawEnemyDeathAnimation
    float completionRatio = enemy->deathTimer / DEATH_ANIMATION_DURATION;
    Vector2 pos = enemy->position;
    float radius = enemy->radius;
    
    switch (enemy->type) {
        case ENEMY_TYPE_NORMAL:
            // Quadrados expandindo
            for (int i = 0; i < 3; i++) {
                float expandRatio = completionRatio * (1.0f + i * 0.3f);
                float alpha = (1.0f - completionRatio) * (1.0f - (float)i * 0.3f);
                float size = radius * expandRatio * 2.0f;
                DrawPixelRect(pos.x - size/2, pos.y - size/2, size, size, Fade(WHITE, alpha));
            }
            break;
            
        case ENEMY_TYPE_SPEEDER:
            // Fragmentos pixelados
            if (Vector2Length(enemy->velocity) > 0) {
                Vector2 velDir = Vector2Normalize(enemy->velocity);
                
                for (int i = 0; i < 5; i++) {
                    float distance = radius * (0.5f + i * 0.5f) * completionRatio * 3.0f;
                    Vector2 particlePos = Vector2Add(pos, Vector2Scale(velDir, distance));
                    
                    float size = radius * (1.0f - completionRatio) * (1.0f - i * 0.15f);
                    DrawPixelCircleV(particlePos, size, Fade(SKYBLUE, (1.0f - completionRatio) * (1.0f - i * 0.2f)));
                }
            }
            break;
            
        case ENEMY_TYPE_TANK:
            // Círculos concêntricos pixelados
            for (int i = 0; i < 3; i++) {
                float ringRatio = 1.0f - completionRatio * 0.7f;
                float ringRadius = radius * ringRatio * (1.0f + i * 0.3f);
                float alpha = (1.0f - completionRatio) * (1.0f - (float)i * 0.2f);
                
                DrawPixelCircleV(pos, ringRadius, Fade(DARKGRAY, alpha));
            }
            
            // Fragmentos voando
            for (int i = 0; i < 8; i++) {
                float angle = i * PI/4.0f;
                float distance = radius * completionRatio * 2.0f;
                
                Vector2 fragmentPos = {
                    pos.x + cosf(angle) * distance,
                    pos.y + sinf(angle) * distance
                };
                
                DrawPixelCircleV(fragmentPos, radius * (1.0f - completionRatio) * 0.2f, 
                               Fade(LIGHTGRAY, 1.0f - completionRatio));
            }
            break;
            
        case ENEMY_TYPE_EXPLODER:
            // Ondas de explosão pixeladas
            for (int i = 0; i < 3; i++) {
                float waveTime = completionRatio - (float)i * 0.15f;
                
                if (waveTime > 0 && waveTime < 1.0f) {
                    float waveRadius = radius * waveTime * 3.0f;
                    float alpha = (1.0f - waveTime) * 0.8f;
                    
                    DrawPixelCircleV(pos, waveRadius, Fade(RED, alpha));
                    DrawPixelCircleV(pos, waveRadius * 0.8f, Fade(BLACK, alpha));
                }
            }
            
            // Partículas pixeladas
            for (int i = 0; i < 15; i++) {
                float angle = i * 24.0f * DEG2RAD; // Para ficar mais pixel art, posições fixas
                float dist = radius * completionRatio * (1.0f + (i % 5) * 0.4f);
                
                Vector2 particlePos = {
                    pos.x + cosf(angle) * dist,
                    pos.y + sinf(angle) * dist
                };
                
                Color particleColor = (i % 2 == 0) ? RED : ORANGE;
                DrawPixelCircleV(particlePos, radius * 0.2f * (1.0f - completionRatio), 
                               Fade(particleColor, 1.0f - completionRatio));
            }
            break;
            
        case ENEMY_TYPE_SHOOTER:
            // Círculos concêntricos pixelados
            for (int i = 0; i < 4; i++) {
                float ringRatio = 0.3f + (float)i * 0.2f;
                float ringRadius = radius * (ringRatio + completionRatio * 1.8f);
                float alpha = (1.0f - completionRatio) * (1.0f - (float)i * 0.2f);
                
                DrawPixelCircleV(pos, ringRadius, Fade(YELLOW, alpha));
                DrawPixelCircleV(pos, ringRadius * 0.8f, Fade(BLACK, alpha));
            }
            
            // "Último tiro" pixelado
            for (int i = 0; i < 4; i++) {
                float angle = i * PI/2.0f + completionRatio * PI;
                float distance = radius * completionRatio * 3.0f;
                
                Vector2 bulletPos = {
                    pos.x + cosf(angle) * distance,
                    pos.y + sinf(angle) * distance
                };
                
                DrawPixelCircleV(bulletPos, radius * 0.2f * (1.0f - completionRatio), 
                               Fade(YELLOW, 1.0f - completionRatio));
            }
            break;
    }
}

// Adicione esta função no arquivo

void DrawPlayAreaBorder(void) {
    // Variável estática para controlar a animação
    static float borderAnimTime = 0.0f;
    borderAnimTime += GetFrameTime() * 15.0f; // Velocidade da animação
    
    // Densidade maior de pontos para a borda
    float pointSpacing = 2.0f;
    
    // Usar currentPlayAreaRadius em vez de PLAY_AREA_RADIUS
    extern float currentPlayAreaRadius;  // Declarar a variável externa
    
    // Desenhar borda da área jogável circular com estilo pixelado e colorido
    for (float angle = 0; angle < 360.0f; angle += pointSpacing) {
        float rad = angle * DEG2RAD;
        
        // Adicionar efeito de onda na borda (pulsação suave)
        float waveEffect = sinf((angle + borderAnimTime) * DEG2RAD * 3) * 3.0f;
        float radiusWithEffect = currentPlayAreaRadius + waveEffect;
        
        Vector2 pointOnCircle = {
            PLAY_AREA_CENTER_X + cosf(rad) * radiusWithEffect,
            PLAY_AREA_CENTER_Y + sinf(rad) * radiusWithEffect
        };
        
        // Alternar entre branco e vermelho
        Color pointColor;
        float segment = fmodf(angle + borderAnimTime, 30.0f); // Segmentos de 30 graus
        
        if (segment < 15.0f) {
            pointColor = RED;
        } else {
            pointColor = WHITE;
        }
        
        // Brilho pulsante para destacar mais a borda
        float brightness = 0.5f + sinf(borderAnimTime * 0.1f + angle * 0.02f) * 0.5f;
        
        // Desenhar ponto maior para borda mais destacada
        DrawPixelCircleV(pointOnCircle, 3.0f, Fade(pointColor, brightness));
    }
    
    // Efeito de brilho em pontos da borda (usar currentPlayAreaRadius)
    for (int i = 0; i < 8; i++) {
        float sparkleAngle = borderAnimTime * 0.2f + i * 45.0f;
        float sparkleRad = sparkleAngle * DEG2RAD;
        
        Vector2 sparklePos = {
            PLAY_AREA_CENTER_X + cosf(sparkleRad) * currentPlayAreaRadius,
            PLAY_AREA_CENTER_Y + sinf(sparkleRad) * currentPlayAreaRadius
        };
        
        float glow = fabsf(sinf(borderAnimTime * 0.3f + i));
        DrawPixelCircleV(sparklePos, 5.0f * glow, Fade(WHITE, glow * 0.8f));
    }
}

// Tela de tutorial com explicações sobre mecânicas do jogo
void DrawTutorialScreen(void) {
    ClearBackground(BLACK);
    
    // Variáveis de animação
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    float pulse = 0.7f + sinf(animTime * 3.0f) * 0.3f;
    
    // Título com efeito de pulsação
    const char *title = "TUTORIAL";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, 
             GetScreenWidth()/2 - titleWidth/2, 
             60, 
             60, 
             Fade(WHITE, pulse));
    
    // Adicione a borda estilizada como na área do jogo
    DrawPlayAreaBorder();
    
    // Área de conteúdo central
    int startY = 150;
    int lineHeight = 40;
    int sectionSpacing = 30;
    int currentY = startY;
    Color titleColor = RED;
    Color textColor = WHITE;
    
    // ----- SEÇÃO DE CONTROLES -----
    DrawText("CONTROLES", GetScreenWidth()/2 - MeasureText("CONTROLES", 30)/2, currentY, 30, titleColor);
    currentY += lineHeight;
    
    DrawText("Mover: WASD ou Setas", GetScreenWidth()/2 - MeasureText("Mover: WASD ou Setas", 20)/2, currentY, 20, textColor);
    currentY += lineHeight;
    
    DrawText("Atirar: Clique Esquerdo do Mouse", GetScreenWidth()/2 - MeasureText("Atirar: Clique Esquerdo do Mouse", 20)/2, currentY, 20, textColor);
    currentY += lineHeight;
    
    DrawText("Dash: Barra de Espaço", GetScreenWidth()/2 - MeasureText("Dash: Barra de Espaço", 20)/2, currentY, 20, textColor);
    currentY += lineHeight + sectionSpacing;
    
    // ----- SEÇÃO DE POWER-UPS -----
    DrawText("POWER-UPS", GetScreenWidth()/2 - MeasureText("POWER-UPS", 30)/2, currentY, 30, titleColor);
    currentY += lineHeight;
    
    // Power-up de Dano (Vermelho)
    DrawPixelCircle(GetScreenWidth()/2 - 300, currentY + 10, 15 * pulse, RED);
    DrawText("Dano Aumentado: Projéteis maiores e mais fortes (custa 1 vida)", 
             GetScreenWidth()/2 - 270, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight;
    
    // Power-up de Cura (Verde)
    DrawPixelCircle(GetScreenWidth()/2 - 300, currentY + 10, 15 * pulse, GREEN);
    DrawText("Cura: Recupera todas as vidas", 
             GetScreenWidth()/2 - 270, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight;
    
    // Power-up de Escudo (Azul)
    DrawPixelCircle(GetScreenWidth()/2 - 300, currentY + 10, 15 * pulse, BLUE);
    DrawText("Escudo: repele um projétil", 
             GetScreenWidth()/2 - 270, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight + sectionSpacing;
    
    // ----- SEÇÃO DE MECÂNICAS -----
    DrawText("MECÂNICAS ESPECIAIS", GetScreenWidth()/2 - MeasureText("MECÂNICAS ESPECIAIS", 30)/2, currentY, 30, titleColor);
    currentY += lineHeight;
    
    DrawText("Área do jogo: Expande e contrai a cada 10 segundos (após 1000 pontos)", 
             GetScreenWidth()/2 - MeasureText("Área do jogo: Expande e contrai a cada 10 segundos (após 1000 pontos)", 20)/2, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight;
    
    DrawText("Dash: Torna você invulnerável e deixa um rastro de fogo", 
             GetScreenWidth()/2 - MeasureText("Dash: Torna você invulnerável e deixa um rastro de fogo", 20)/2, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight;
    
    DrawText("Inimigos: Diferentes tipos com comportamentos únicos", 
             GetScreenWidth()/2 - MeasureText("Inimigos: Diferentes tipos com comportamentos únicos", 20)/2, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight + sectionSpacing * 2;
    
    // Instruções para continuar
    const char *startText = "PRESS SPACE TO START GAME";
    DrawText(startText, 
             GetScreenWidth()/2 - MeasureText(startText, 30)/2, 
             currentY, 
             30, 
             Fade(RED, 0.5f + sinf(animTime * 5.0f) * 0.5f));
    
    // Cursor minimalista
    DrawMinimalistCursor();
}

// Função para desenhar o menu de pausa
void DrawPauseMenu(void) {
    // Desenhar o jogo escurecido ao fundo
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    
    // Variáveis de animação
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    float pulse = 0.7f + sinf(animTime * 3.0f) * 0.3f;
    
    // Título com efeito de pulsação
    const char *title = "PAUSA";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, 
             GetScreenWidth()/2 - titleWidth/2, 
             GetScreenHeight()/4, 
             60, 
             Fade(WHITE, pulse));
    
    // Desenhar instruções
    int startY = GetScreenHeight()/2;
    int lineHeight = 40;
    Color textColor = WHITE;
    
    // Opções do menu
    const char* resumeText = "PRESSIONE P PARA CONTINUAR";
    const char* menuText = "PRESSIONE M PARA MENU PRINCIPAL";
    const char* restartText = "PRESSIONE R PARA REINICIAR";
    
    float resumePulse = 0.6f + sinf(animTime * 2.0f) * 0.4f;
    float menuPulse = 0.6f + sinf(animTime * 2.0f + 0.5f) * 0.4f;
    float restartPulse = 0.6f + sinf(animTime * 2.0f + 1.0f) * 0.4f;
    
    DrawText(resumeText, 
            GetScreenWidth()/2 - MeasureText(resumeText, 24)/2, 
            startY, 
            24, 
            Fade(GREEN, resumePulse));
    
    DrawText(menuText, 
            GetScreenWidth()/2 - MeasureText(menuText, 24)/2, 
            startY + lineHeight, 
            24, 
            Fade(WHITE, menuPulse));
    
    DrawText(restartText, 
            GetScreenWidth()/2 - MeasureText(restartText, 24)/2, 
            startY + lineHeight * 2, 
            24, 
            Fade(RED, restartPulse));
    
    // Desenhar a borda da área de jogo atrás do menu para efeito visual
    DrawPlayAreaBorder();
    
    // Cursor minimalista
    DrawMinimalistCursor();
}