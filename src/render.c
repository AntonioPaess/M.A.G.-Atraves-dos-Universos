#include "render.h"
#include "raylib.h"
#include "raymath.h"  // Adicionando raymath.h para funções de vetores (também contém Lerp)
#include <stdio.h> // Para TextFormat
#include <math.h>  // Para a função sinf

// Declarações forward de todas as funções auxiliares
static void DrawPixelLine(float x1, float y1, float x2, float y2, Color color);
static void DrawPixelCircle(float centerX, float centerY, float radius, Color color);
static void DrawPixelCircleV(Vector2 center, float radius, Color color);
static void DrawPixelText(const char *text, int posX, int posY, int fontSize, Color color);
static void DrawPixelRect(float x, float y, float width, float height, Color color);
void DrawMinimalistCursor(void);

// Função auxiliar para desenhar linhas pixeladas (escopo de arquivo)
static void DrawPixelLine(float x1, float y1, float x2, float y2, Color color) {
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

void DrawGameplay(const Player *player, const EnemyList *enemies, const Bullet *bullets, const Bullet *enemyBullets, long score) {
    // Desenhar jogador como um "buraco negro"
    if (player) {
        if (player->visible) {
            // Efeito de "buraco negro" com anéis concêntricos
            static float holeTime = 0.0f;
            holeTime += GetFrameTime() * 1.5f;
            
            // Círculo externo pulsante (aura)
            float outerPulse = sinf(holeTime) * 3.0f;
            DrawPixelCircleV(player->position, player->radius + outerPulse, Fade(DARKGRAY, 0.3f));
            
            // Círculo principal mais escuro
            DrawPixelCircleV(player->position, player->radius, BLACK);
            
            // Borda brilhante
            float borderPulse = 0.6f + sinf(holeTime * 1.2f) * 0.4f;
            DrawCircleLinesV(player->position, player->radius, Fade(WHITE, borderPulse));
            
            // Efeito de "vórtice" no centro
            float vortexSize = player->radius * 0.6f;
            float vortexAngle = holeTime * 2.0f;
            
            // Desenhar linhas espirais para um efeito de vórtice
            for (int i = 0; i < 4; i++) {
                float angle = vortexAngle + i * (PI / 2.0f);
                float x1 = player->position.x + cosf(angle) * (vortexSize * 0.2f);
                float y1 = player->position.y + sinf(angle) * (vortexSize * 0.2f);
                float x2 = player->position.x + cosf(angle) * vortexSize;
                float y2 = player->position.y + sinf(angle) * vortexSize;
                
                DrawPixelLine(x1, y1, x2, y2, Fade(WHITE, 0.5f));
            }
        }
    }

    // Desenhar inimigos
    if (enemies) {
        const Enemy *currentEnemy = enemies->head;
        while (currentEnemy != NULL) {
            if (currentEnemy->active) {
                DrawPixelCircleV(currentEnemy->position, currentEnemy->radius, currentEnemy->color);
                
                // Se for um Tank, desenhar um anel pixelado para mostrar que é mais resistente
                if (currentEnemy->type == ENEMY_TYPE_TANK) {
                    // Desenhar círculos concêntricos para simular o anel
                    DrawPixelCircleV(currentEnemy->position, currentEnemy->radius - 2, Fade(BLACK, 0.5f));
                    DrawPixelCircleV(currentEnemy->position, currentEnemy->radius - 4, Fade(WHITE, 0.5f));
                }
                
                // Se for um Shooter, desenhar indicador visual pixelado
                if (currentEnemy->type == ENEMY_TYPE_SHOOTER) {
                    // Um padrão de pixels ao redor para indicar inimigo atirador
                    DrawPixelCircleV(currentEnemy->position, currentEnemy->radius + 4, Fade(YELLOW, 0.3f));
                }
            }
            currentEnemy = currentEnemy->next;
        }
    }

    // Desenhar projéteis do jogador com animação de rastro energético
    static float bulletAnimTime = 0.0f;
    bulletAnimTime += GetFrameTime() * 4.0f;
    
    if (bullets) {
        const Bullet *currentBullet = bullets;
        while (currentBullet != NULL) {
            if (currentBullet->active) {
                // Calcular a direção normalizada do projétil
                Vector2 dir = Vector2Normalize(currentBullet->velocity);
                
                // Efeito de rastro (3 círculos cada vez menores)
                for (int i = 1; i <= 3; i++) {
                    float trailDist = i * 3.0f;
                    Vector2 trailPos = {
                        currentBullet->position.x - dir.x * trailDist,
                        currentBullet->position.y - dir.y * trailDist
                    };
                    
                    float trailPulse = sinf(bulletAnimTime + i * 0.5f) * 0.2f + 0.8f;
                    float trailAlpha = 0.7f - (i * 0.2f);
                    float trailRadius = currentBullet->radius * (1.0f - (i * 0.25f)) * trailPulse;
                    
                    DrawPixelCircleV(trailPos, trailRadius, Fade(SKYBLUE, trailAlpha));
                }
                
                // Projétil principal com efeito de pulso
                float pulse = 0.8f + sinf(bulletAnimTime) * 0.2f;
                DrawPixelCircleV(currentBullet->position, currentBullet->radius * pulse, SKYBLUE);
                DrawPixelCircleV(currentBullet->position, currentBullet->radius * 0.6f * pulse, WHITE);
            }
            currentBullet = currentBullet->next;
        }
    }
    
    // Desenhar projéteis dos inimigos como quadrados giratórios
    static float enemyBulletRotation = 0.0f;
    enemyBulletRotation += GetFrameTime() * 3.0f;
    
    if (enemyBullets) {
        const Bullet *currentBullet = enemyBullets;
        while (currentBullet != NULL) {
            if (currentBullet->active) {
                // Identificar a origem do projétil para diferenciação
                bool isShooterBullet = false;
                // Assumimos que projéteis maiores são de Shooters
                if (currentBullet->radius >= BULLET_RADIUS * 1.2f) {
                    isShooterBullet = true;
                }
                
                // Tamanho base do quadrado
                float quadSize = currentBullet->radius * 2.0f;
                // Quadrados de Shooter são maiores
                if (isShooterBullet) {
                    quadSize *= 1.4f;
                }
                
                // Calcular pontos do quadrado com rotação
                float angle = enemyBulletRotation;
                Vector2 center = currentBullet->position;
                
                // Cor base
                Color bulletColor = isShooterBullet ? YELLOW : RED;
                
                // Desenhar quadrado rotacionado
                Vector2 p1, p2, p3, p4;
                p1.x = center.x + cosf(angle) * quadSize/2;
                p1.y = center.y + sinf(angle) * quadSize/2;
                
                p2.x = center.x + cosf(angle + PI/2) * quadSize/2;
                p2.y = center.y + sinf(angle + PI/2) * quadSize/2;
                
                p3.x = center.x + cosf(angle + PI) * quadSize/2;
                p3.y = center.y + sinf(angle + PI) * quadSize/2;
                
                p4.x = center.x + cosf(angle + 3*PI/2) * quadSize/2;
                p4.y = center.y + sinf(angle + 3*PI/2) * quadSize/2;
                
                // Desenhar as linhas do quadrado usando nossa função pixelada
                DrawPixelLine(p1.x, p1.y, p2.x, p2.y, bulletColor);
                DrawPixelLine(p2.x, p2.y, p3.x, p3.y, bulletColor);
                DrawPixelLine(p3.x, p3.y, p4.x, p4.y, bulletColor);
                DrawPixelLine(p4.x, p4.y, p1.x, p1.y, bulletColor);
                
                // Desenhar diagonal cruzada para shooter bullets
                if (isShooterBullet) {
                    DrawPixelLine(p1.x, p1.y, p3.x, p3.y, Fade(YELLOW, 0.7f));
                    DrawPixelLine(p2.x, p2.y, p4.x, p4.y, Fade(YELLOW, 0.7f));
                }
            }
            currentBullet = currentBullet->next;
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
        Color lifeColor = RED;
        
        // Último coração pisca quando o jogador está invencível
        if (i == player->lives - 1 && player->isInvincible) {
            float blinkRate = sinf(GetTime() * 8.0f);
            lifeColor = ColorTint(RED, Fade(WHITE, (blinkRate > 0) ? 0.7f : 0.3f));
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
        scoreColor = ColorTint(WHITE, Fade(YELLOW, scoreFlash));
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
        DrawPixelCircle(rightAlign - 20, 30, 8, Fade(RED, 0.7f));
        
        // Rótulo "ENEMIES" embaixo
        DrawPixelText("ENEMIES", GetScreenWidth() - MeasureText("ENEMIES", 16) - 20, 45, 16, Fade(LIGHTGRAY, 0.6f));
    }
    
    // Borda inferior do HUD com efeito de gradiente
    for (int x = 0; x < GetScreenWidth(); x += 2) {
        float brightness = 0.4f + sinf(x * 0.01f) * 0.1f; // Variação sutil de brilho
        DrawPixelRect(x, 59, 2, 2, Fade(WHITE, brightness * 0.3f));
    }

    // Cursor pixelado
    DrawMinimalistCursor();
}

void DrawGameOverScreen(long finalScore) {
    ClearBackground(BLACK);

    // Efeito de background dinâmico similar ao menu, mas com cores mais escuras
    static float backgroundTime = 0.0f;
    backgroundTime += GetFrameTime() * 0.3f;
    
    // Desenhar círculos de fundo com movimento suave
    for (int i = 0; i < 5; i++) {
        float radius = 35.0f + i * 20.0f;
        float xOffset = sinf(backgroundTime + i * 0.7f) * GetScreenWidth() * 0.15f;
        float yOffset = cosf(backgroundTime * 0.8f + i * 0.3f) * GetScreenHeight() * 0.1f;
        float alpha = 0.02f + (float)i * 0.003f; // Círculos mais distantes são mais transparentes
        
        DrawPixelCircle(
            GetScreenWidth() * 0.5f + xOffset,
            GetScreenHeight() * 0.4f + yOffset,
            radius,
            Fade(DARKGRAY, alpha)
        );
    }

    // Título "GAME OVER" com efeito de pulsação
    const char* gameOverText = "GAME OVER";
    int gameOverWidth = MeasureText(gameOverText, 60);
    
    static float titlePulse = 0.0f;
    titlePulse += GetFrameTime();
    float titleScale = 1.0f + sinf(titlePulse * 0.8f) * 0.05f;
    float titleAlpha = 0.8f + sinf(titlePulse * 1.2f) * 0.2f;
    
    // Desenhar um círculo de "glow" atrás do título
    DrawPixelCircle(
        GetScreenWidth()/2,
        GetScreenHeight()/3 - 30,
        gameOverWidth * 0.6f,
        Fade(RED, 0.1f)
    );
    
    // Desenhar o título
    DrawPixelText(gameOverText, 
                 GetScreenWidth()/2 - (gameOverWidth * titleScale)/2, 
                 GetScreenHeight()/3 - 30 - (60 * titleScale)/4, 
                 60 * titleScale, 
                 Fade(RED, titleAlpha));

    // Preparar texto do score com animação
    static long displayScore = 0;
    static bool scoreAnimationDone = false;
    static float scoreAnimTimer = 0.0f;
    
    // Animação de contagem do score
    if (!scoreAnimationDone) {
        scoreAnimTimer += GetFrameTime() * 3.0f; // Velocidade da animação
        displayScore = (long)(finalScore * (scoreAnimTimer > 1.0f ? 1.0f : scoreAnimTimer));
        
        if (scoreAnimTimer >= 1.0f) {
            scoreAnimationDone = true;
        }
    }
    
    // Reset da animação quando tiver um novo game over
    static long lastScore = 0;
    if (lastScore != finalScore) {
        displayScore = 0;
        scoreAnimationDone = false;
        scoreAnimTimer = 0.0f;
        lastScore = finalScore;
    }
    
    const char* scoreText = TextFormat("%ld", displayScore);
    int scoreTextWidth = MeasureText(scoreText, 100);
    
    // Rótulo "SCORE"
    const char* scoreLabel = "SCORE";
    int scoreLabelWidth = MeasureText(scoreLabel, 40);
    
    // Desenhar rótulo
    DrawPixelText(scoreLabel, 
                 GetScreenWidth()/2 - scoreLabelWidth/2, 
                 GetScreenHeight()/2 - 140, 
                 40, 
                 LIGHTGRAY);
    
    // Desenhar valor do score com efeito pulsante
    static float scorePulse = 0.0f;
    scorePulse += GetFrameTime() * 0.5f;
    float scoreScale = 1.0f + (scoreAnimationDone ? sinf(scorePulse) * 0.05f : 0.0f);
    
    // Desenhar caixa de destaque para a pontuação
    DrawPixelRect(
        GetScreenWidth()/2 - scoreTextWidth/2 - 30,
        GetScreenHeight()/2 - 110,
        scoreTextWidth + 60,
        120,
        Fade(DARKGRAY, 0.2f)
    );
    
    // Desenha a pontuação com estilo pixelado
    DrawPixelText(scoreText, 
                 GetScreenWidth()/2 - (scoreTextWidth * scoreScale)/2, 
                 GetScreenHeight()/2 - 90, 
                 100 * scoreScale, 
                 WHITE);

    // Mensagem baseada na pontuação
    const char* performanceText;
    Color performanceColor;
    
    if (finalScore < 500) {
        performanceText = "CONTINUE TENTANDO!";
        performanceColor = GRAY;
    } else if (finalScore < 1000) {
        performanceText = "BOM TRABALHO!";
        performanceColor = GREEN;
    } else if (finalScore < 3000) {
        performanceText = "INCRIVEL!";
        performanceColor = YELLOW;
    } else {
        performanceText = "MESTRE DO JOGO!";
        performanceColor = RED;
    }
    
    int performanceWidth = MeasureText(performanceText, 30);
    DrawPixelText(performanceText, 
                 GetScreenWidth()/2 - performanceWidth/2, 
                 GetScreenHeight()/2 + 20, 
                 30, 
                 performanceColor);

    // Instruções com efeito de hover
    const char* restartText = "PRESS 'R' TO RESTART";
    const char* menuText = "PRESS 'M' FOR MENU";

    int restartTextWidth = MeasureText(restartText, 30);
    int menuTextWidth = MeasureText(menuText, 30);

    // Animação para as opções
    static float menuPulse = 0.0f;
    menuPulse += GetFrameTime() * 1.5f;
    float alpha = 0.7f + sinf(menuPulse) * 0.3f;
    
    // Obter posição do mouse para efeito de hover
    Vector2 mousePos = GetMousePosition();
    
    // Calcular retângulos dos botões (para detecção de hover)
    Rectangle restartRect = {
        GetScreenWidth()/2 - restartTextWidth/2 - 20,
        GetScreenHeight()/2 + 70 - 10,
        restartTextWidth + 40,
        50
    };
    
    Rectangle menuRect = {
        GetScreenWidth()/2 - menuTextWidth/2 - 20,
        GetScreenHeight()/2 + 120 - 10,
        menuTextWidth + 40,
        50
    };
    
    // Cores para as opções com efeito de hover
    Color restartColor = GRAY;
    Color menuColor = GRAY;
    
    if (CheckCollisionPointRec(mousePos, restartRect)) {
        restartColor = WHITE;
        // Desenhar indicador de seleção
        DrawPixelCircle(GetScreenWidth()/2 - restartTextWidth/2 - 40, GetScreenHeight()/2 + 70 + 15, 6, WHITE);
    }
    
    if (CheckCollisionPointRec(mousePos, menuRect)) {
        menuColor = WHITE;
        // Desenhar indicador de seleção
        DrawPixelCircle(GetScreenWidth()/2 - menuTextWidth/2 - 40, GetScreenHeight()/2 + 120 + 15, 6, WHITE);
    }
    
    // Desenhar bordas para as opções
    DrawPixelRect(restartRect.x, restartRect.y, restartRect.width, restartRect.height, Fade(restartColor, 0.2f));
    DrawPixelRect(menuRect.x, menuRect.y, menuRect.width, menuRect.height, Fade(menuColor, 0.2f));
    
    // Desenhar texto das opções
    DrawPixelText(restartText, GetScreenWidth()/2 - restartTextWidth/2, GetScreenHeight()/2 + 70, 30, Fade(restartColor, alpha));
    DrawPixelText(menuText, GetScreenWidth()/2 - menuTextWidth/2, GetScreenHeight()/2 + 120, 30, Fade(menuColor, alpha));

    ShowCursor();
}

void DrawMainMenu(void) {
    ClearBackground(BLACK);
    
    const char *title = "M.A.G.";
    const char *subtitle = "o inimigo agora e outro";
    const char *startOption = "INICIAR JOGO";
    const char *exitOption = "SAIR";
    
    int titleWidth = MeasureText(title, 80);
    int subtitleWidth = MeasureText(subtitle, 40);
    int startWidth = MeasureText(startOption, 30);
    int exitWidth = MeasureText(exitOption, 30);
    
    // Efeito de background dinâmico (círculos pixelados que se movem lentamente)
    static float backgroundTime = 0.0f;
    backgroundTime += GetFrameTime() * 0.3f;
    
    // Desenhar 7 círculos de fundo com movimento suave
    for (int i = 0; i < 7; i++) {
        float radius = 25.0f + i * 15.0f;
        float xOffset = sinf(backgroundTime + i * 0.7f) * GetScreenWidth() * 0.2f;
        float yOffset = cosf(backgroundTime * 0.8f + i * 0.3f) * GetScreenHeight() * 0.1f;
        float alpha = 0.03f + (float)i * 0.005f; // Círculos mais distantes são mais transparentes
        
        DrawPixelCircle(
            GetScreenWidth() * 0.5f + xOffset,
            GetScreenHeight() * 0.4f + yOffset,
            radius,
            Fade(GRAY, alpha)
        );
    }
    
    // Efeito de animação para o título
    static float titlePulse = 0.0f;
    titlePulse += GetFrameTime();
    float titleScale = 1.0f + sinf(titlePulse * 0.8f) * 0.05f;
    
    // Título com um efeito de "glow" (círculo grande por baixo)
    DrawPixelCircle(
        GetScreenWidth()/2,
        GetScreenHeight()/4,
        titleWidth * 0.7f,
        Fade(DARKGRAY, 0.1f)
    );
    
    // Desenhar título com efeito de pulsação
    DrawPixelText(title, 
                 GetScreenWidth()/2 - (titleWidth * titleScale)/2, 
                 GetScreenHeight()/4 - (80 * titleScale)/4, 
                 80 * titleScale, 
                 WHITE);
    
    // Desenhar subtítulo com efeito de fade pulsante
    float subtitleAlpha = 0.6f + sinf(titlePulse * 1.2f) * 0.3f;
    DrawPixelText(subtitle, 
                 GetScreenWidth()/2 - subtitleWidth/2, 
                 GetScreenHeight()/4 + 100, 
                 40, 
                 Fade(LIGHTGRAY, subtitleAlpha));
    
    // Animação para as opções do menu
    static float menuPulse = 0.0f;
    menuPulse += GetFrameTime() * 1.5f;
    float alpha = 0.7f + sinf(menuPulse) * 0.3f;
    
    // Obter posição do mouse para efeito de hover
    Vector2 mousePos = GetMousePosition();
    
    // Calcular retângulos dos botões (para detecção de hover)
    Rectangle startRect = {
        GetScreenWidth()/2 - startWidth/2 - 20,
        GetScreenHeight()/2 + 100 - 10,
        startWidth + 40,
        50
    };
    
    Rectangle exitRect = {
        GetScreenWidth()/2 - exitWidth/2 - 20,
        GetScreenHeight()/2 + 160 - 10,
        exitWidth + 40,
        50
    };
    
    // Efeito de hover nas opções
    Color startColor = WHITE;
    Color exitColor = WHITE;
    
    if (CheckCollisionPointRec(mousePos, startRect)) {
        startColor = YELLOW;
        // Desenhar indicador de seleção (pixelado)
        DrawPixelCircle(GetScreenWidth()/2 - startWidth/2 - 40, GetScreenHeight()/2 + 100 + 15, 6, YELLOW);
    }
    
    if (CheckCollisionPointRec(mousePos, exitRect)) {
        exitColor = YELLOW;
        // Desenhar indicador de seleção (pixelado)
        DrawPixelCircle(GetScreenWidth()/2 - exitWidth/2 - 40, GetScreenHeight()/2 + 160 + 15, 6, YELLOW);
    }
    
    // Desenhar bordas para as opções, criando um efeito de "botão"
    DrawPixelRect(startRect.x, startRect.y, startRect.width, startRect.height, Fade(startColor, 0.2f));
    DrawPixelRect(exitRect.x, exitRect.y, exitRect.width, exitRect.height, Fade(exitColor, 0.2f));
    
    // Desenhar texto das opções
    DrawPixelText(startOption, GetScreenWidth()/2 - startWidth/2, GetScreenHeight()/2 + 100, 30, Fade(startColor, alpha));
    DrawPixelText(exitOption, GetScreenWidth()/2 - exitWidth/2, GetScreenHeight()/2 + 160, 30, Fade(exitColor, alpha));
    
    // Desenhar versão na parte inferior
    const char* versionText = "v1.0";
    DrawText(versionText, GetScreenWidth() - MeasureText(versionText, 20) - 10, GetScreenHeight() - 30, 20, Fade(GRAY, 0.5f));
    
    // Desenhar cursor personalizado
    Vector2 cursorPos = mousePos;
    
    // Snap para grid de pixels
    const float pixelGrid = 3.0f;
    float snapX = (int)(cursorPos.x / pixelGrid) * pixelGrid;
    float snapY = (int)(cursorPos.y / pixelGrid) * pixelGrid;
    
    // Efeito de pulsação para o cursor
    float cursorPulse = 0.8f + sinf(menuPulse * 2.0f) * 0.2f;
    
    // Desenhar blocos de pixels para o cursor
    for (float x = snapX - 3; x <= snapX + 3; x += pixelGrid) {
        for (float y = snapY - 3; y <= snapY + 3; y += pixelGrid) {
            if (fabs(x - snapX) + fabs(y - snapY) <= 6 * cursorPulse) { // Padrão "diamante" com pulsação
                Color cursorColor = WHITE;
                if (CheckCollisionPointRec(mousePos, startRect) || CheckCollisionPointRec(mousePos, exitRect)) {
                    cursorColor = YELLOW; // Cursor amarelo quando sobre um botão
                }
                DrawRectangle(x, y, pixelGrid, pixelGrid, Fade(cursorColor, 0.7f));
            }
        }
    }
}

// Função para desenhar o cursor minimalista no estilo "O"
// Adicione esta função ao arquivo render.c

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
static void DrawPixelCircle(float centerX, float centerY, float radius, Color color) {
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
static void DrawPixelCircleV(Vector2 center, float radius, Color color) {
    DrawPixelCircle(center.x, center.y, radius, color);
}

// Função para desenhar texto pixelado
static void DrawPixelText(const char *text, int posX, int posY, int fontSize, Color color) {
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
static void DrawPixelRect(float x, float y, float width, float height, Color color) {
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