#include "render.h"
#include "raylib.h"
#include "raymath.h"
#include "powerup.h"
#include "game.h" 
#include "scoreboard.h" 
#include <stdio.h>
#include <math.h>
#include <string.h>


typedef struct {
    Vector2 position;
    float radius;
    float lifeTime;
    float currentTime;
    Color color;
    bool active;
    float rotation;
} Explosion;


#define MAX_EXPLOSIONS 32
static Explosion explosions[MAX_EXPLOSIONS] = {0};


void DrawPixelLine(float x1, float y1, float x2, float y2, Color color);
void DrawPixelCircle(float centerX, float centerY, float radius, Color color);
void DrawPixelCircleV(Vector2 center, float radius, Color color);
void DrawPixelText(const char *text, int posX, int posY, int fontSize, Color color);
void DrawPixelRect(float x, float y, float width, float height, Color color);
void DrawMinimalistCursor(void);
void DrawPlayAreaBorder(void);
void DrawHUD(long score, int enemyCount, int playerLives);  // Adicionei o número de vidas aqui


extern ScoreEntry GetScoreAt(int index);
extern int GetScoreCount(void);


void DrawPixelLine(float x1, float y1, float x2, float y2, Color color) {
    Vector2 start = {x1, y1};
    Vector2 end = {x2, y2};
    Vector2 delta = {end.x - start.x, end.y - start.y};
    float length = sqrtf(delta.x * delta.x + delta.y * delta.y);
    Vector2 dir = {delta.x / length, delta.y / length};
    
    
    float pixelSize = 2.0f;
    int pixelCount = (int)(length / pixelSize);
    
    for (int i = 0; i < pixelCount; i++) {
        float x = start.x + dir.x * i * pixelSize;
        float y = start.y + dir.y * i * pixelSize;
        DrawRectangle(x, y, pixelSize, pixelSize, color);
    }
}




void DrawGameplay(const Player *player, const EnemyList *enemies, const Bullet *bullets, const Bullet *enemyBullets, const Powerup *powerups, long score) {
    // Desenhar HUD primeiro - agora passando o número de vidas do jogador
    DrawHUD(score, enemies->count, player->lives);
    
    // Usar scissor mode para limitar o desenho da área de jogo
    float hudHeight = 60.0f;
    BeginScissorMode(0, hudHeight, GetScreenWidth(), GetScreenHeight() - hudHeight);
    
    // Desenhar a área de jogo
    DrawPlayAreaBorder();
    
    // Desenhar player
    if (player && player->visible) {
        
        static float pulseTime = 0.0f;
        pulseTime += GetFrameTime() * 2.0f;
        float pulseFactor = 1.0f + sinf(pulseTime) * 0.05f;
        
        
        DrawPixelCircleV(player->position, player->radius * pulseFactor, WHITE);
        
        
        DrawPixelCircleV(player->position, player->radius * 0.7f * pulseFactor, BLACK);
        
        
        if (player->hasShield) {
            float shieldPulse = 1.0f + sinf(pulseTime * 1.5f) * 0.1f;
            float shieldRatio = player->shieldTimer / 15.0f; 
            Color shieldColor = Fade(BLUE, 0.3f + 0.2f * shieldRatio);
            
            
            DrawPixelCircleV(player->position, player->radius * 1.5f * shieldPulse, shieldColor);
            
            
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
        
        
        if (player->isDashing) {
            
            for (int i = 1; i <= 8; i++) { 
                float alpha = 0.8f - (i * 0.09f); 
                
                
                Vector2 baseTrailPos = Vector2Subtract(
                    player->position, 
                    Vector2Scale(player->dashDirection, player->radius * i * 0.6f)
                );
                
                
                DrawPixelCircleV(baseTrailPos, player->radius * (0.7f - i * 0.05f), Fade(RED, alpha));
                
                
                if (i < 6) { 
                    for (int j = 0; j < 3; j++) {
                        float angle = GetTime() * 5.0f + i * 0.5f + j * 2.0f; 
                        float offset = player->radius * 0.4f * (1.0f - i * 0.1f); 
                        
                        Vector2 particlePos = {
                            baseTrailPos.x + cosf(angle) * offset,
                            baseTrailPos.y + sinf(angle) * offset
                        };
                        
                        
                        float particleSize = player->radius * (0.3f - i * 0.03f);
                        DrawPixelCircleV(particlePos, particleSize, Fade(RED, alpha * 0.8f));
                    }
                }
            }
            
            
            DrawPixelCircleV(player->position, player->radius * 1.3f, Fade(RED, 0.3f));
        }
    }

    
    DrawEnemies(enemies);

    
    if (bullets) {
        const Bullet *currentBullet = bullets;
        while (currentBullet) {
            if (currentBullet->active) {
                
                DrawPixelCircleV(currentBullet->position, currentBullet->radius, WHITE);
                
                
                Vector2 trail = Vector2Subtract(currentBullet->position, 
                                           Vector2Scale(currentBullet->velocity, 0.02f));
                DrawPixelCircleV(trail, currentBullet->radius * 0.6f, (Color){255, 255, 255, 120});
            }
            currentBullet = currentBullet->next;
        }
    }

    
    if (enemyBullets) {
        const Bullet *currentBullet = enemyBullets;
        while (currentBullet) {
            if (currentBullet->active) {
                
                Vector2 pos = currentBullet->position;
                float radius = currentBullet->radius;
                
                
                static float rotationTime = 0.0f;
                rotationTime += GetFrameTime() * 4.0f;
                float rotation = rotationTime + pos.x * 0.01f; 
                
                
                float squareSize = radius * 1.8f;
                
                
                Color enemyBulletColor = RED; 
                
                
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
                    
                    
                    DrawLineEx(corner1, corner2, radius * 0.4f, enemyBulletColor);
                }
                
                
                DrawCircleV(pos, radius * 0.5f, enemyBulletColor);
                
                
                Vector2 trail = Vector2Subtract(pos, Vector2Scale(currentBullet->velocity, 0.03f));
                DrawCircleV(trail, radius * 0.4f, Fade(enemyBulletColor, 0.6f));
                Vector2 trail2 = Vector2Subtract(pos, Vector2Scale(currentBullet->velocity, 0.06f));
                DrawCircleV(trail2, radius * 0.3f, Fade(enemyBulletColor, 0.3f));
            }
            currentBullet = currentBullet->next;
        }
    }

    
    if (powerups) {
        const Powerup *currentPowerup = powerups;
        while (currentPowerup) {
            if (currentPowerup->active) {
                
                float pulse = 0.8f + sinf(GetTime() * 3.0f) * 0.2f;
                
                
                Color color;
                if (currentPowerup->type == POWERUP_DAMAGE) {
                    color = RED;  
                } else if (currentPowerup->type == POWERUP_HEAL) {
                    color = GREEN;  
                } else {
                    color = BLUE;  
                }
                
                
                DrawPixelCircleV(currentPowerup->position, currentPowerup->radius * pulse, color);
                
                
                DrawPixelCircleV(currentPowerup->position, currentPowerup->radius * 0.7f * pulse, BLACK);
                
                
                if (currentPowerup->type == POWERUP_DAMAGE) {
                    
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
                    
                    DrawPixelCircleV(
                        currentPowerup->position, 
                        currentPowerup->radius * 0.3f * pulse, 
                        color
                    );
                } else {
                    
                    float shieldRadius = currentPowerup->radius * 0.3f * pulse;
                    DrawPixelCircleV(currentPowerup->position, shieldRadius, color);
                    
                    
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
                
                
                DrawPixelCircleV(
                    currentPowerup->position, 
                    currentPowerup->radius * 1.2f * pulse, 
                    Fade(color, 0.3f)
                );
            }
            currentPowerup = currentPowerup->next;
        }
    }

    
    
    
    Color topColor = Fade(BLACK, 0.85f);
    Color bottomColor = Fade(BLACK, 0.5f);
    
    for (int y = 0; y < 60; y++) {
        float alpha = Lerp(topColor.a, bottomColor.a, y/60.0f);
        DrawPixelRect(0, y, GetScreenWidth(), 1, Fade(BLACK, alpha));
    }
    
    
    float centerX = GetScreenWidth() / 2;
    for (int i = 10; i < 50; i += 3) {
        DrawPixelCircle(centerX, 30, 1, Fade(WHITE, 0.1f + (i % 2) * 0.1f));
    }
    
    
    
    
    static float heartPulse = 0.0f;
    heartPulse += GetFrameTime() * 1.2f;
    
    
    DrawPixelText("LIVES", 20, 24, 30, Fade(WHITE, 0.9f));
    
    
    int livesTextWidth = MeasureText("LIVES", 30);
    
    
    DrawPixelRect(30 + livesTextWidth, 20, player->lives * 42 + 10, 28, Fade(WHITE, 0.15f));
    
    
    for (int i = 0; i < player->lives; i++) {
        float pulse = 0.8f + sinf(heartPulse + i * 0.8f) * 0.2f;
        Color lifeColor = RED; 
        
        
        if (i == player->lives - 1 && player->isInvincible) {
            float blinkRate = sinf(GetTime() * 8.0f);
            lifeColor = Fade(RED, (blinkRate > 0) ? 0.7f : 0.3f); 
        }
        
        
        float heartX = 50 + livesTextWidth + i * 40; 
        float heartY = 34; 
        float heartSize = 9.0f * pulse; 
        
        
        DrawPixelCircle(heartX - heartSize/2, heartY, heartSize, lifeColor);
        DrawPixelCircle(heartX + heartSize/2, heartY, heartSize, lifeColor);
        
        
        DrawPixelLine(heartX - heartSize, heartY, heartX, heartY + heartSize*1.5, lifeColor);
        DrawPixelLine(heartX + heartSize, heartY, heartX, heartY + heartSize*1.5, lifeColor);
        
        
        DrawPixelCircle(heartX, heartY + heartSize*1.5, 2, Fade(WHITE, 0.7f * pulse));
    }
    
    
    
    
    static long lastScore = 0;
    static float scoreFlash = 0.0f;
    
    if (score > lastScore) {
        scoreFlash = 1.0f;
        lastScore = score;
    }
    
    scoreFlash = scoreFlash > 0.0f ? scoreFlash - GetFrameTime() * 2.0f : 0.0f;
    
    
    const char* scoreText = TextFormat("%ld", score);
    Color scoreColor = WHITE;
    
    if (scoreFlash > 0.0f) {
        scoreColor = Fade(WHITE, 0.7f + scoreFlash * 0.3f); 
    }
    
    DrawPixelText(scoreText, centerX - MeasureText(scoreText, 38)/2, 15, 38, scoreColor);
    
    
    DrawPixelText("SCORE", centerX - MeasureText("SCORE", 16)/2, 45, 16, Fade(LIGHTGRAY, 0.6f));
    
    
    if (enemies) {
        
        const char* enemiesText = TextFormat("%d", enemies->count);
        int textWidth = MeasureText(enemiesText, 28);
        int rightAlign = GetScreenWidth() - 20 - textWidth;
        
        
        DrawPixelText(enemiesText, rightAlign, 20, 28, Fade(WHITE, 0.9f));
        
        
        DrawPixelCircle(rightAlign - 20, 30, 8, Fade(RED, 0.7f)); 
        
        
        DrawPixelText("ENEMIES", GetScreenWidth() - MeasureText("ENEMIES", 16) - 20, 45, 16, Fade(LIGHTGRAY, 0.6f));
    }
    
    
    for (int x = 0; x < GetScreenWidth(); x += 2) {
        float brightness = 0.4f + sinf(x * 0.01f) * 0.1f;
        DrawPixelRect(x, 59, 2, 2, Fade(WHITE, brightness * 0.3f));
    }

    
    EndScissorMode();
    
    // Desenhar barra de dash na parte inferior da tela
    float dashCooldownRatio = player->dashCooldown / DASH_COOLDOWN;
    int barWidth = 200;
    int barHeight = 10;
    int barX = GetScreenWidth() / 2 - barWidth / 2;
    int barY = GetScreenHeight() - 30;
    
    Color dashBarColor = RED;
    float alpha = 0.7f + sinf(GetTime() * 4.0f) * 0.3f;
    
    DrawPixelText("DASH", barX, barY - 15, 16, Fade(WHITE, 0.6f));
    DrawPixelRect(barX - 2, barY - 2, barWidth + 4, barHeight + 4, Fade(WHITE, 0.3f));
    DrawPixelRect(barX, barY, barWidth, barHeight, Fade(BLACK, 0.6f));
    
    if (!player->isDashing) {
        int fillWidth = barWidth * (1.0f - dashCooldownRatio);
        DrawPixelRect(barX, barY, fillWidth, barHeight, Fade(dashBarColor, alpha));
        
        if (fillWidth > 0 && fillWidth < barWidth) {
            DrawPixelRect(barX + fillWidth - 3, barY, 6, barHeight, Fade(WHITE, alpha * 0.8f));
        }
    } else {
        DrawPixelRect(barX, barY, barWidth, barHeight, Fade(RED, alpha * 1.5f));
    }
    
    if (dashCooldownRatio <= 0.0f && !player->isDashing) {
        DrawPixelText("READY", barX + barWidth - MeasureText("READY", 14), barY - 15, 14, 
                     Fade(RED, 0.8f + sinf(GetTime() * 5.0f) * 0.2f));
    }
    
    DrawMinimalistCursor();
}


void DrawMainMenu(void) {
    ClearBackground(BLACK);
    
    
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    
    
    for (int i = 0; i < 50; i++) {
        float x = sinf(animTime * 0.5f + i * 0.3f) * GetScreenWidth() * 0.5f + GetScreenWidth() * 0.5f;
        float y = cosf(animTime * 0.3f + i * 0.2f) * GetScreenHeight() * 0.5f + GetScreenHeight() * 0.5f;
        float size = 2.0f + sinf(animTime + i) * 1.5f;
        
        
        Color particleColor = (i % 3 == 0) ? 
                             Fade(RED, 0.2f + sinf(animTime + i) * 0.1f) : 
                             Fade(WHITE, 0.1f + sinf(animTime + i * 0.7f) * 0.05f);
                             
        DrawPixelCircle(x, y, size, particleColor);
    }
    
    
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
    
    
    const char *title = "M.A.G.";
    int titleWidth = MeasureText(title, 120);
    
    
    float titlePulse = 1.0f + sinf(animTime * 3.0f) * 0.1f;
    for (int i = 10; i > 0; i -= 2) {
        DrawText(title, 
                GetScreenWidth()/2 - titleWidth/2 - i, 
                GetScreenHeight()/3 - i, 
                120, 
                Fade(RED, 0.05f * i * titlePulse));
    }
    
    
    DrawText(title, 
            GetScreenWidth()/2 - titleWidth/2, 
            GetScreenHeight()/3, 
            120, 
            Fade(WHITE, 0.8f + sinf(animTime * 4.0f) * 0.2f));
    
    
    const char *subtitle = "O inimigo agora é outro";
    int subtitleWidth = MeasureText(subtitle, 30);
    DrawText(subtitle, 
            GetScreenWidth()/2 - subtitleWidth/2, 
            GetScreenHeight()/3 + 100, 
            30, 
            Fade(WHITE, 0.5f + sinf(animTime * 2.0f) * 0.2f));
    
    
    const char *startOption = "PRESS ANY KEY TO START";  
    int startWidth = MeasureText(startOption, 30);
    
    
    float startPulse = 0.6f + sinf(animTime * 5.0f) * 0.4f;
    DrawText(startOption, 
            GetScreenWidth()/2 - startWidth/2, 
            GetScreenHeight()/2 + 200, 
            30, 
            Fade(RED, startPulse));
    
    
    DrawMinimalistCursor();
}



void DrawMinimalistCursor(void) {
    HideCursor();
    Vector2 mousePos = GetMousePosition();
    Color cursorColor = WHITE;
    
    
    static float animTime = 0.0f;
    animTime += GetFrameTime() * 2.0f;
    
    
    float baseSize = 18.0f; 
    float pulseEffect = sinf(animTime) * 1.5f;
    float size = baseSize + pulseEffect;
    
    
    float cornerSize = size / 3.0f; 
    
    
    DrawPixelLine(mousePos.x - size/2, mousePos.y - size/2, mousePos.x - size/2 + cornerSize, mousePos.y - size/2, cursorColor);
    DrawPixelLine(mousePos.x - size/2, mousePos.y - size/2, mousePos.x - size/2, mousePos.y - size/2 + cornerSize, cursorColor);
    
    
    DrawPixelLine(mousePos.x + size/2 - cornerSize, mousePos.y - size/2, mousePos.x + size/2, mousePos.y - size/2, cursorColor);
    DrawPixelLine(mousePos.x + size/2, mousePos.y - size/2, mousePos.x + size/2, mousePos.y - size/2 + cornerSize, cursorColor);
    
    
    DrawPixelLine(mousePos.x - size/2, mousePos.y + size/2 - cornerSize, mousePos.x - size/2, mousePos.y + size/2, cursorColor);
    DrawPixelLine(mousePos.x - size/2, mousePos.y + size/2, mousePos.x - size/2 + cornerSize, mousePos.y + size/2, cursorColor);
    
    
    DrawPixelLine(mousePos.x + size/2 - cornerSize, mousePos.y + size/2, mousePos.x + size/2, mousePos.y + size/2, cursorColor);
    DrawPixelLine(mousePos.x + size/2, mousePos.y + size/2 - cornerSize, mousePos.x + size/2, mousePos.y + size/2, cursorColor);
    
    
    float crossSize = size / 4.0f;
    float rotation = sinf(animTime * 0.5f) * 0.2f; 
    
    
    float cs = cosf(rotation);
    float sn = sinf(rotation);
    
    
    float x1 = -crossSize * cs - (-crossSize) * sn;
    float y1 = -crossSize * sn + (-crossSize) * cs;
    float x2 = crossSize * cs - crossSize * sn;
    float y2 = crossSize * sn + crossSize * cs;
    
    
    float x3 = crossSize * cs - (-crossSize) * sn;
    float y3 = crossSize * sn + (-crossSize) * cs;
    float x4 = -crossSize * cs - crossSize * sn;
    float y4 = -crossSize * sn + crossSize * cs;
    
    
    DrawPixelLine(mousePos.x + x1, mousePos.y + y1, mousePos.x + x2, mousePos.y + y2, cursorColor);
    DrawPixelLine(mousePos.x + x3, mousePos.y + y3, mousePos.x + x4, mousePos.y + y4, cursorColor);
}



void DrawPixelCircle(float centerX, float centerY, float radius, Color color) {
    
    const float pixelSize = 3.0f;
    
    
    int minX = (int)((centerX - radius) / pixelSize) * pixelSize;
    int minY = (int)((centerY - radius) / pixelSize) * pixelSize;
    int maxX = (int)((centerX + radius) / pixelSize) * pixelSize + pixelSize;
    int maxY = (int)((centerY + radius) / pixelSize) * pixelSize + pixelSize;
    
    
    for (float x = minX; x <= maxX; x += pixelSize) {
        for (float y = minY; y <= maxY; y += pixelSize) {
            
            float dx = (x + pixelSize/2) - centerX;
            float dy = (y + pixelSize/2) - centerY;
            if (dx*dx + dy*dy <= radius*radius) {
                DrawRectangle(x, y, pixelSize, pixelSize, color);
            }
        }
    }
}


void DrawPixelCircleV(Vector2 center, float radius, Color color) {
    DrawPixelCircle(center.x, center.y, radius, color);
}


void DrawPixelText(const char *text, int posX, int posY, int fontSize, Color color) {
    
    const float scaleFactor = 0.8f;
    int scaledFontSize = (int)(fontSize * scaleFactor);
    
    
    const float pixelGrid = 2.0f;
    int snapX = (int)(posX / pixelGrid) * pixelGrid;
    int snapY = (int)(posY / pixelGrid) * pixelGrid;
    
    DrawText(text, snapX, snapY, scaledFontSize, color);
}


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




void DrawEnemies(const EnemyList *enemies) {
    if (enemies) {
        const Enemy *currentEnemy = enemies->head;
        while (currentEnemy != NULL) {
            if (currentEnemy->active) {
                
                float radius = currentEnemy->radius;
                Vector2 pos = currentEnemy->position;
                
                
                static float pulseTime = 0.0f;
                pulseTime += GetFrameTime() * 2.0f;
                float pulseFactor = 1.0f + sinf(pulseTime + currentEnemy->position.x * 0.01f) * 0.1f;
                
                
                switch (currentEnemy->type) {
                    case ENEMY_TYPE_NORMAL:
                        
                        {
                            
                            float squareSize = radius * 1.8f * pulseFactor;
                            
                            
                            DrawPixelRect(pos.x - squareSize/2, pos.y - squareSize/2, squareSize, squareSize, WHITE);
                            DrawPixelRect(pos.x - squareSize*0.7f/2, pos.y - squareSize*0.7f/2, squareSize*0.7f, squareSize*0.7f, BLACK);
                            DrawPixelRect(pos.x - squareSize*0.4f/2, pos.y - squareSize*0.4f/2, squareSize*0.4f, squareSize*0.4f, WHITE);
                            
                            
                            DrawPixelCircleV(pos, radius * 0.15f, WHITE);
                        }
                        break;
                        
                    case ENEMY_TYPE_SPEEDER:
                        
                        {
                            
                            Vector2 dir = Vector2Normalize(currentEnemy->velocity);
                            if (Vector2Length(currentEnemy->velocity) < 0.1f) {
                                dir = (Vector2){0, -1}; 
                            }
                            
                            
                            DrawPixelCircleV(pos, radius * pulseFactor, SKYBLUE);
                            DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, BLACK);
                            
                            
                            Vector2 tipPos = Vector2Add(pos, Vector2Scale(dir, radius * 0.9f));
                            DrawPixelCircleV(tipPos, radius * 0.4f, SKYBLUE);
                            
                            
                            Vector2 perpendicular = (Vector2){-dir.y, dir.x};
                            
                            
                            Vector2 leftWing = Vector2Add(pos, Vector2Scale(perpendicular, radius * 0.8f));
                            DrawPixelCircleV(leftWing, radius * 0.3f, SKYBLUE);
                            
                            
                            Vector2 rightWing = Vector2Subtract(pos, Vector2Scale(perpendicular, radius * 0.8f));
                            DrawPixelCircleV(rightWing, radius * 0.3f, SKYBLUE);
                            
                            
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
                        
                        {
                            
                            DrawPixelCircleV(pos, radius * 1.3f * pulseFactor, DARKGRAY);
                            DrawPixelCircleV(pos, radius * 0.9f * pulseFactor, BLACK);
                            DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, GRAY);
                            
                            
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
                            
                            
                            Vector2 dir = Vector2Normalize(currentEnemy->velocity);
                            if (Vector2Length(currentEnemy->velocity) < 0.1f) {
                                dir = (Vector2){1, 0};
                            }
                            
                            
                            for (float t = 0; t <= 1.0f; t += 0.2f) {
                                Vector2 pointOnCannon = {
                                    pos.x + dir.x * radius * 1.5f * t,
                                    pos.y + dir.y * radius * 1.5f * t
                                };
                                float pointSize = radius * 0.25f * (1.0f - t * 0.3f);
                                DrawPixelCircleV(pointOnCannon, pointSize, DARKGRAY);
                            }
                            
                            
                            Vector2 cannonTip = Vector2Add(pos, Vector2Scale(dir, radius * 1.5f));
                            DrawPixelCircleV(cannonTip, radius * 0.15f, LIGHTGRAY);
                            
                            
                            Vector2 perp = (Vector2){-dir.y, dir.x};
                            
                            
                            for (float t = -0.5f; t <= 0.5f; t += 0.2f) {
                                Vector2 trackPoint = {
                                    pos.x + dir.x * radius * t - perp.x * radius * 0.8f,
                                    pos.y + dir.y * radius * t - perp.y * radius * 0.8f
                                };
                                DrawPixelCircleV(trackPoint, radius * 0.12f, DARKGRAY);
                            }
                            
                            
                            for (float t = -0.5f; t <= 0.5f; t += 0.2f) {
                                Vector2 trackPoint = {
                                    pos.x + dir.x * radius * t + perp.x * radius * 0.8f,
                                    pos.y + dir.y * radius * t + perp.y * radius * 0.8f
                                };
                                DrawPixelCircleV(trackPoint, radius * 0.12f, DARKGRAY);
                            }
                            
                            
                            DrawPixelCircleV(pos, radius * 0.2f, LIGHTGRAY);
                        }
                        break;
                        
                    case ENEMY_TYPE_EXPLODER:
                        
                        {
                            
                            DrawPixelCircleV(pos, radius * 0.7f * pulseFactor, RED);
                            DrawPixelCircleV(pos, radius * 0.5f * pulseFactor, BLACK);
                            
                            
                            int spikes = 8;
                            float spikeAngle = 2.0f * PI / spikes;
                            float rotation = GetTime() * 3.0f;
                            
                            for (int i = 0; i < spikes; i++) {
                                float angle = rotation + i * spikeAngle;
                                float spikePulse = 0.8f + sinf(GetTime() * 5.0f + i) * 0.2f;
                                
                                
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
                            
                            
                            float corePulse = 0.7f + sinf(GetTime() * 8.0f) * 0.3f;
                            DrawPixelCircleV(pos, radius * 0.3f * corePulse, RED);
                            
                            
                            if (corePulse > 0.9f) {
                                DrawPixelCircleV(pos, radius * pulseFactor * 1.5f, Fade(RED, corePulse * 0.2f));
                            }
                        }
                        break;
                        
                    case ENEMY_TYPE_SHOOTER:
                        
                        {
                            
                            DrawPixelCircleV(pos, radius * 0.8f * pulseFactor, YELLOW);
                            DrawPixelCircleV(pos, radius * 0.65f * pulseFactor, BLACK);
                            DrawPixelCircleV(pos, radius * 0.5f * pulseFactor, YELLOW);
                            
                            
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
                                
                                
                                DrawPixelLine(cannonBase.x, cannonBase.y, cannonTip.x, cannonTip.y, YELLOW);
                                
                                
                                if (currentEnemy->shootTimer > 0.5f) {
                                    float chargeRatio = (currentEnemy->shootTimer - 0.5f) / 0.5f;
                                    DrawPixelCircleV(cannonTip, radius * 0.18f * chargeRatio, 
                                                 Fade(WHITE, 0.4f + chargeRatio * 0.6f));
                                }
                            }
                            
                            
                            float corePulse = 0.7f;
                            if (currentEnemy->shootTimer > 0.3f) {
                                corePulse = 0.7f + sinf(GetTime() * 8.0f) * 0.3f * 
                                          ((currentEnemy->shootTimer - 0.3f) / 0.7f);
                            }
                            
                            DrawPixelCircleV(pos, radius * 0.35f * corePulse, Fade(YELLOW, 0.7f + corePulse * 0.3f));
                            
                            
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
                DrawEnemyDeathAnimation(currentEnemy);  
            }
            
            currentEnemy = currentEnemy->next;
        }
    }
}


void DrawEnemyDeathAnimation(const Enemy *enemy) {  
    float completionRatio = enemy->deathTimer / DEATH_ANIMATION_DURATION;
    Vector2 pos = enemy->position;
    float radius = enemy->radius;
    
    switch (enemy->type) {
        case ENEMY_TYPE_NORMAL:
            
            for (int i = 0; i < 3; i++) {
                float expandRatio = completionRatio * (1.0f + i * 0.3f);
                float alpha = (1.0f - completionRatio) * (1.0f - (float)i * 0.3f);
                float size = radius * expandRatio * 2.0f;
                DrawPixelRect(pos.x - size/2, pos.y - size/2, size, size, Fade(WHITE, alpha));
            }
            break;
            
        case ENEMY_TYPE_SPEEDER:
            
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
            
            for (int i = 0; i < 3; i++) {
                float ringRatio = 1.0f - completionRatio * 0.7f;
                float ringRadius = radius * ringRatio * (1.0f + i * 0.3f);
                float alpha = (1.0f - completionRatio) * (1.0f - (float)i * 0.2f);
                
                DrawPixelCircleV(pos, ringRadius, Fade(DARKGRAY, alpha));
            }
            
            
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
            
            for (int i = 0; i < 3; i++) {
                float waveTime = completionRatio - (float)i * 0.15f;
                
                if (waveTime > 0 && waveTime < 1.0f) {
                    float waveRadius = radius * waveTime * 3.0f;
                    float alpha = (1.0f - waveTime) * 0.8f;
                    
                    DrawPixelCircleV(pos, waveRadius, Fade(RED, alpha));
                    DrawPixelCircleV(pos, waveRadius * 0.8f, Fade(BLACK, alpha));
                }
            }
            
            
            for (int i = 0; i < 15; i++) {
                float angle = i * 24.0f * DEG2RAD; 
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
            
            for (int i = 0; i < 4; i++) {
                float ringRatio = 0.3f + (float)i * 0.2f;
                float ringRadius = radius * (ringRatio + completionRatio * 1.8f);
                float alpha = (1.0f - completionRatio) * (1.0f - (float)i * 0.2f);
                
                DrawPixelCircleV(pos, ringRadius, Fade(YELLOW, alpha));
                DrawPixelCircleV(pos, ringRadius * 0.8f, Fade(BLACK, alpha));
            }
            
            
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



void DrawPlayAreaBorder(void) {
    static float borderAnimTime = 0.0f;
    borderAnimTime += GetFrameTime() * 15.0f; 
    
    float pointSpacing = 1.0f; // Reduzido para criar mais pontos (borda mais densa)
    extern float currentPlayAreaRadius;  
    
    // Definir a área de HUD (evitar desenhar aqui)
    float hudHeight = 60.0f;  // Altura da área de score/HUD
    
    // Desenhar borda principal mais grossa
    for (float angle = 0; angle < 360.0f; angle += pointSpacing) {
        float rad = angle * DEG2RAD;
        
        float waveEffect = sinf((angle + borderAnimTime) * DEG2RAD * 3) * 3.0f;
        float radiusWithEffect = currentPlayAreaRadius + waveEffect;
        
        Vector2 pointOnCircle = {
            PLAY_AREA_CENTER_X + cosf(rad) * radiusWithEffect,
            PLAY_AREA_CENTER_Y + sinf(rad) * radiusWithEffect
        };
        
        // Verificar se o ponto está na área de HUD
        if (pointOnCircle.y < hudHeight) {
            continue;  // Pular este ponto (não desenhar)
        }
        
        // Cores mais vívidas e alternância mais definida
        Color pointColor;
        float segment = fmodf(angle + borderAnimTime, 30.0f); 
        
        if (segment < 15.0f) {
            pointColor = RED;
        } else {
            pointColor = WHITE;
        }
        
        // Desenhar círculos pequenos em vez de pixels para uma borda mais grossa
        DrawCircleV(pointOnCircle, 2.5f, pointColor);
    }
    
    // Adicionar brilho externo para aumentar visibilidade
    for (float angle = 0; angle < 360.0f; angle += pointSpacing * 3) {
        float rad = angle * DEG2RAD;
        float radiusWithEffect = currentPlayAreaRadius + 2.0f;
        
        Vector2 glowPoint = {
            PLAY_AREA_CENTER_X + cosf(rad) * radiusWithEffect,
            PLAY_AREA_CENTER_Y + sinf(rad) * radiusWithEffect
        };
        
        if (glowPoint.y < hudHeight) continue;
        
        Color glowColor = (angle + borderAnimTime) / 30.0f < 0.5f ? 
                         Fade(RED, 0.4f) : Fade(WHITE, 0.4f);
        DrawCircleV(glowPoint, 4.0f, glowColor);
    }
}


void DrawTutorialScreen(void) {
    ClearBackground(BLACK);
    
    
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    float pulse = 0.7f + sinf(animTime * 1.5f) * 0.2f;
    
    
    const char *title = "TUTORIAL";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, 
             GetScreenWidth()/2 - titleWidth/2, 
             60, 
             60, 
             Fade(WHITE, pulse));
    
    
    DrawPlayAreaBorder();
    
    
    int startY = 150;
    int lineHeight = 40;
    int sectionSpacing = 30;
    int currentY = startY;
    Color titleColor = RED;
    Color textColor = WHITE; 
    
    
    DrawText("CONTROLES", GetScreenWidth()/2 - MeasureText("CONTROLES", 30)/2, currentY, 30, titleColor);
    currentY += lineHeight;
    
    DrawText("Mover: WASD ou Setas", GetScreenWidth()/2 - MeasureText("Mover: WASD ou Setas", 20)/2, currentY, 20, textColor);
    currentY += lineHeight;
    
    DrawText("Atirar: Clique Esquerdo do Mouse", GetScreenWidth()/2 - MeasureText("Atirar: Clique Esquerdo do Mouse", 20)/2, currentY, 20, textColor);
    currentY += lineHeight;
    
    DrawText("Dash: Barra de Espaço", GetScreenWidth()/2 - MeasureText("Dash: Barra de Espaço", 20)/2, currentY, 20, textColor);
    currentY += lineHeight + sectionSpacing;
    
    
    DrawText("POWER-UPS", GetScreenWidth()/2 - MeasureText("POWER-UPS", 30)/2, currentY, 30, titleColor);
    currentY += lineHeight;
    
    
    DrawPixelCircle(GetScreenWidth()/2 - 300, currentY + 10, 15 * pulse, RED);
    DrawText("Dano Aumentado: Projéteis maiores e mais fortes (custa 1 vida)", 
             GetScreenWidth()/2 - 270, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight;
    
    
    DrawPixelCircle(GetScreenWidth()/2 - 300, currentY + 10, 15 * pulse, GREEN);
    DrawText("Cura: Recupera todas as vidas", 
             GetScreenWidth()/2 - 270, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight;
    
    
    DrawPixelCircle(GetScreenWidth()/2 - 300, currentY + 10, 15 * pulse, BLUE);
    DrawText("Escudo: repele um projétil", 
             GetScreenWidth()/2 - 270, 
             currentY, 
             20, 
             textColor);
    currentY += lineHeight + sectionSpacing;
    
    
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
    
    
    const char *startText = "PRESS SPACE TO START GAME";
    DrawText(startText, 
             GetScreenWidth()/2 - MeasureText(startText, 30)/2, 
             currentY, 
             30, 
             Fade(RED, 0.5f + sinf(animTime * 5.0f) * 0.5f));
    
    
    DrawMinimalistCursor();
}


void DrawHUD(long score, int enemyCount, int playerLives) {
    // Definir a área do HUD
    int hudHeight = 60;
    DrawRectangle(0, 0, GetScreenWidth(), hudHeight, Fade(BLACK, 0.8f));
    
    // ===== PONTUAÇÃO =====
    float centerX = GetScreenWidth() / 2;
    
    static long lastScore = 0;
    static float scoreFlash = 0.0f;
    
    if (score > lastScore) {
        scoreFlash = 1.0f;
        lastScore = score;
    }
    
    scoreFlash = scoreFlash > 0.0f ? scoreFlash - GetFrameTime() * 2.0f : 0.0f;
    
    const char* scoreText = TextFormat("%ld", score);
    Color scoreColor = WHITE;
    
    if (scoreFlash > 0.0f) {
        scoreColor = Fade(WHITE, 0.7f + scoreFlash * 0.3f); 
    }
    
    DrawPixelText(scoreText, centerX - MeasureText(scoreText, 38)/2, 15, 38, scoreColor);
    DrawPixelText("SCORE", centerX - MeasureText("SCORE", 16)/2, 45, 16, Fade(LIGHTGRAY, 0.6f));
    
    // ===== VIDAS (CORAÇÕES) =====
    DrawPixelText("LIVES", 20, 24, 30, Fade(WHITE, 0.9f));
    
    int livesTextWidth = MeasureText("LIVES", 30);
    DrawPixelRect(30 + livesTextWidth, 20, 3 * 42 + 10, 28, Fade(WHITE, 0.15f));
    
    static float heartPulse = 0.0f;
    heartPulse += GetFrameTime() * 1.2f;
    
    for (int i = 0; i < 3; i++) {
        float pulse = 0.8f + sinf(heartPulse + i * 0.8f) * 0.2f;
        
        // MUDANÇA AQUI: Verifica se este coração deve ser preenchido ou vazio
        Color lifeColor = i < playerLives ? RED : Fade(RED, 0.3f);
        
        float heartX = 50 + livesTextWidth + i * 40; 
        float heartY = 34; 
        float heartSize = 9.0f * pulse; 
        
        
        DrawPixelCircle(heartX - heartSize/2, heartY, heartSize, lifeColor);
        DrawPixelCircle(heartX + heartSize/2, heartY, heartSize, lifeColor);
        
        
        DrawPixelLine(heartX - heartSize, heartY, heartX, heartY + heartSize*1.5, lifeColor);
        DrawPixelLine(heartX + heartSize, heartY, heartX, heartY + heartSize*1.5, lifeColor);
        
        
        // Brilho central - mais fraco nos corações vazios
        float glowAlpha = i < playerLives ? 0.7f : 0.3f;
        DrawPixelCircle(heartX, heartY + heartSize*1.5, 2, Fade(WHITE, glowAlpha * pulse));
    }
    
    // ===== CONTAGEM DE INIMIGOS =====
    const char* enemiesText = TextFormat("%d", enemyCount);
    int textWidth = MeasureText(enemiesText, 28);
    int rightAlign = GetScreenWidth() - 20 - textWidth;
    
    DrawPixelText(enemiesText, rightAlign, 20, 28, Fade(WHITE, 0.9f));
    DrawPixelCircle(rightAlign - 20, 30, 8, Fade(RED, 0.7f)); 
    DrawPixelText("ENEMIES", GetScreenWidth() - MeasureText("ENEMIES", 16) - 20, 45, 16, Fade(LIGHTGRAY, 0.6f));
    
    // Linha separadora na parte inferior
    for (int x = 0; x < GetScreenWidth(); x += 2) {
        float brightness = 0.4f + sinf(x * 0.01f) * 0.1f;
        DrawPixelRect(x, 59, 2, 2, Fade(WHITE, brightness * 0.3f));
    }
}
void DrawGameOverScreen(long finalScore) {
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.9f));
    
    
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    
    
    const char *title = "GAME OVER";
    int titleWidth = MeasureText(title, 60);
    float titlePulse = 1.0f + sinf(animTime * 3.0f) * 0.1f;
    DrawText(title, 
             GetScreenWidth()/2 - titleWidth/2, 
             GetScreenHeight()/2 - 100, 
             60, 
             Fade(RED, titlePulse));
    
    
    char scoreText[100];
    sprintf(scoreText, "Pontuação final: %ld", finalScore);
    DrawText(scoreText, GetScreenWidth()/2 - MeasureText(scoreText, 30)/2, 
             GetScreenHeight()/2 - 30, 30, WHITE);
    
    
    const char *restartText = "Pressione R para reiniciar";
    const char *menuText = "Pressione M para voltar ao menu";
    
    
    for (int i = 0; i < 2; i++) {
        const char *text = (i == 0) ? restartText : menuText;
        float textAnim = animTime + i * 0.5f; 
        int textAlpha = (int)(sinf(textAnim * 6.0f) * 127.0f + 128.0f);
        Color textColor = (i == 0) ? GREEN : WHITE;
        
        
        DrawText(text, 
                 GetScreenWidth()/2 - MeasureText(text, 20)/2, 
                 GetScreenHeight()/2 + 40 + i * 30, 
                 20, 
                 Fade(textColor, textAlpha / 255.0f));
    }
    
    
    for (int i = 0; i < 20; i++) {
        float x = GetRandomValue(0, GetScreenWidth());
        float y = GetRandomValue(0, GetScreenHeight());
        float size = GetRandomValue(2, 4);
        Color particleColor = Fade(RED, GetRandomValue(50, 100) / 255.0f);
        
        DrawPixelCircle(x, y, size, particleColor);
    }
    
    
    DrawMinimalistCursor();
}


void DrawPauseMenu(void) {
    // Desenhar retângulo semi-transparente
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    
    // Animação de pulsação
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    float pulse = 0.7f + sinf(animTime * 3.0f) * 0.3f;
    
    // Título
    const char *title = "PAUSA";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, 
             GetScreenWidth()/2 - titleWidth/2, 
             GetScreenHeight()/4, 
             60, 
             Fade(WHITE, pulse));
    
    
    int startY = GetScreenHeight()/2;
    int lineHeight = 40;
    
    
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
    
    
    DrawPlayAreaBorder();
    
    
    DrawMinimalistCursor();
}


void DrawGameSummary(long score, int kills, float gameTime) {
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    
    
    DrawPlayAreaBorder();

    
    const char *title = "RESUMO DA PARTIDA";
    DrawText(title, 
             GetScreenWidth()/2 - MeasureText(title, 40)/2, 
             GetScreenHeight()/5, 
             40, 
             WHITE);
    
    
    int centerX = GetScreenWidth()/2;
    int startY = GetScreenHeight()/3;
    int lineHeight = 40;
    
    
    char scoreText[64];
    sprintf(scoreText, "PONTUAÇÃO: %ld", score);
    DrawText(scoreText, 
             centerX - MeasureText(scoreText, 30)/2, 
             startY, 
             30, 
             RED);
    
    
    char killsText[64];
    sprintf(killsText, "INIMIGOS ELIMINADOS: %d", kills);
    DrawText(killsText, 
             centerX - MeasureText(killsText, 30)/2, 
             startY + lineHeight, 
             30, 
             WHITE);
    
    
    char timeText[64];
    sprintf(timeText, "TEMPO DE JOGO: %s", FormatTime(gameTime));
    DrawText(timeText, 
             centerX - MeasureText(timeText, 30)/2, 
             startY + 2 * lineHeight, 
             30, 
             WHITE);
    
    
    DrawText("Pressione R para jogar novamente", 
             centerX - MeasureText("Pressione R para jogar novamente", 20)/2, 
             startY + 4 * lineHeight, 
             20, 
             GRAY);
    
    DrawText("Pressione S para salvar no ranking", 
             centerX - MeasureText("Pressione S para salvar no ranking", 20)/2, 
             startY + 5 * lineHeight, 
             20, 
             GRAY);
    
    DrawText("Pressione M para voltar ao menu", 
             centerX - MeasureText("Pressione M para voltar ao menu", 20)/2, 
             startY + 6 * lineHeight, 
             20, 
             GRAY);
    
    
    DrawMinimalistCursor();
}

void DrawNameEntryScreen(Game *game) {
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    
    
    DrawPlayAreaBorder();

    
    static float animTime = 0.0f;
    animTime += GetFrameTime();
    float pulse = 0.8f + sinf(animTime * 2.5f) * 0.2f;

    const char *title = "NOVO RECORDE!";
    int titleFontSize = 50;
    int titleY = GetScreenHeight() * 0.1f;
    DrawText(title, 
             GetScreenWidth()/2 - MeasureText(title, titleFontSize)/2, 
             titleY, 
             titleFontSize, 
             Fade(RED, pulse));

    
    const char *subtitle = "Digite seu nome para o ranking";
    int subtitleFontSize = 25;
    int subtitleY = titleY + titleFontSize + 15;
    DrawText(subtitle,
             GetScreenWidth()/2 - MeasureText(subtitle, subtitleFontSize)/2,
             subtitleY,
             subtitleFontSize,
             WHITE);

    
    int boxWidth = 500;
    int boxHeight = 60;
    int boxX = GetScreenWidth()/2 - boxWidth/2;
    int boxY = subtitleY + 60;

    
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(DARKGRAY, 0.7f));
    DrawRectangleLinesEx((Rectangle){boxX, boxY, boxWidth, boxHeight}, 
                         3, Fade(RED, pulse));

    
    int nameFontSize = 35;
    DrawText(game->playerName, 
             boxX + 20, 
             boxY + (boxHeight - nameFontSize)/2, 
             nameFontSize, 
             WHITE);

    
    int nameWidth = MeasureText(game->playerName, nameFontSize);
    if ((int)(GetTime() * 2) % 2 == 0) {
        DrawText("|", 
                 boxX + 20 + nameWidth, 
                 boxY + (boxHeight - nameFontSize)/2, 
                 nameFontSize, 
                 WHITE);
    }

    
    char scoreText[64];
    sprintf(scoreText, "PONTUAÇÃO: %ld", game->score);
    int scoreFontSize = 30;
    int scoreY = boxY + boxHeight + 40;
    DrawText(scoreText, 
             GetScreenWidth()/2 - MeasureText(scoreText, scoreFontSize)/2, 
             scoreY, 
             scoreFontSize, 
             RED);
    
    
    char statsText[100];
    sprintf(statsText, "Inimigos eliminados: %d | Tempo de jogo: %s", 
            game->enemiesKilled, FormatTime(game->gameTime));
    int statsY = scoreY + scoreFontSize + 20;
    DrawText(statsText, 
             GetScreenWidth()/2 - MeasureText(statsText, 20)/2, 
             statsY, 
             20, 
             GRAY);

    
    const char *instruction = "Pressione ENTER para confirmar";
    DrawText(instruction, 
             GetScreenWidth()/2 - MeasureText(instruction, 25)/2,
             GetScreenHeight() - 80, 
             25, 
             Fade(RED, 0.5f + sinf(animTime * 4) * 0.5f));

    
    DrawMinimalistCursor();
}

void RenderScoreboardScreen(void) {
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    
    
    DrawPlayAreaBorder();
    
    
    const char *title = "MELHORES PONTUAÇÕES";
    DrawText(title, 
             GetScreenWidth()/2 - MeasureText(title, 40)/2, 
             50, 
             40, 
             WHITE);
    
    
    int columnHeaderY = 120;
    DrawText("POSIÇÃO", GetScreenWidth()/2 - 250, columnHeaderY, 20, GRAY);
    DrawText("NOME", GetScreenWidth()/2 - 100, columnHeaderY, 20, GRAY);
    DrawText("PONTUAÇÃO", GetScreenWidth()/2 + 100, columnHeaderY, 20, GRAY);
    
    
    DrawLine(GetScreenWidth()/2 - 300, columnHeaderY + 30, 
             GetScreenWidth()/2 + 300, columnHeaderY + 30, 
             Fade(GRAY, 0.5));
    
    
    int startY = columnHeaderY + 50;
    int entryHeight = 40;
    
    int scoreCount = GetScoreCount();
    for (int i = 0; i < scoreCount && i < MAX_SCORES; i++) {
        ScoreEntry score = GetScoreAt(i);
        
        
        char posText[10];
        sprintf(posText, "%dº", i + 1);
        DrawText(posText, 
                 GetScreenWidth()/2 - 250, 
                 startY + i * entryHeight, 
                 25, 
                 WHITE);
        
        
        DrawText(score.name, 
                 GetScreenWidth()/2 - 100, 
                 startY + i * entryHeight, 
                 25, 
                 WHITE);
        
        
        char scoreText[20];
        sprintf(scoreText, "%ld", score.score);
        DrawText(scoreText, 
                 GetScreenWidth()/2 + 100, 
                 startY + i * entryHeight, 
                 25, 
                 WHITE);
    }
    
    
    static float time = 0;
    time += GetFrameTime();
    const char *instruction = "Pressione M para voltar ao menu";
    DrawText(instruction, 
             GetScreenWidth()/2 - MeasureText(instruction, 20)/2, 
             GetScreenHeight() - 50, 
             20, 
             Fade(RED, 0.5f + sinf(time * 3.0f) * 0.5f));
    
    
    DrawMinimalistCursor();
}
