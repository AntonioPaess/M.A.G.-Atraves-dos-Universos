#include "render.h"
#include "raylib.h"
#include "raymath.h"
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
    // Desenhar jogador como um peixe pixel art
    if (player) {
        if (player->visible) {
            // Configurar animação e timing
            static float fishTime = 0.0f;
            fishTime += GetFrameTime() * 4.0f; // Velocidade da animação
            
            // Calcular a direção do movimento para orientar o peixe
            static Vector2 lastPosition = {0, 0};
            static Vector2 direction = {1, 0}; // Começa olhando para a direita
            
            // Apenas atualiza a direção se o jogador se moveu significativamente
            if (Vector2Distance(lastPosition, player->position) > 0.5f) {
                direction = Vector2Normalize(Vector2Subtract(player->position, lastPosition));
                lastPosition = player->position;
            }
            
            // Ângulo de orientação do peixe (baseado na direção do movimento)
            float angle = atan2f(direction.y, direction.x);
            float facing = direction.x > 0 ? 1.0f : -1.0f; // Determina se peixe olha para direita ou esquerda
            
            // Centro do peixe
            Vector2 center = player->position;
            float radius = player->radius * 1.2f; 
            
            // Cores do peixe
            Color bodyColor = SKYBLUE;
            Color finColor = Fade(BLUE, 0.7f);
            Color eyeColor = WHITE;
            Color pupilColor = BLACK;
            
            // Calcular ondulação da cauda e barbatanas
            float tailWave = sinf(fishTime * 1.2f) * 0.6f;
            float finWave = cosf(fishTime * 0.8f) * 0.3f;
            
            // Desenhar corpo principal (oval)
            DrawPixelCircleV(center, radius, bodyColor);
            
            // Cauda (triangular com ondulação)
            Vector2 tailBase = {
                center.x - facing * radius * 0.7f,
                center.y
            };
            
            Vector2 tailTip = {
                center.x - facing * (radius * 1.8f),
                center.y + (tailWave * radius * 1.4f)
            };
            
            Vector2 tailTop = {
                center.x - facing * (radius * 1.2f),
                center.y - radius * 0.5f
            };
            
            Vector2 tailBottom = {
                center.x - facing * (radius * 1.2f),
                center.y + radius * 0.5f
            };
            
            // Desenhar triangulos da cauda
            DrawPixelLine(tailBase.x, tailBase.y, tailTip.x, tailTip.y, finColor);
            DrawPixelLine(tailTop.x, tailTop.y, tailTip.x, tailTip.y, finColor);
            DrawPixelLine(tailBottom.x, tailBottom.y, tailTip.x, tailTip.y, finColor);
            
            // Preencher cauda
            for (float y = tailTop.y; y <= tailBottom.y; y += 2.0f) {
                float t = (y - tailTop.y) / (tailBottom.y - tailTop.y);
                float x = Lerp(tailTop.x, tailBottom.x, t);
                DrawPixelLine(x, y, tailTip.x, tailTip.y, finColor);
            }
            
            // Barbatana superior
            Vector2 topFinBase = {
                center.x + facing * (radius * 0.1f),
                center.y - radius * 0.8f
            };
            
            Vector2 topFinTip = {
                center.x + facing * (radius * 0.5f + finWave * radius),
                center.y - radius * 1.4f
            };
            
            // Barbatana inferior
            Vector2 bottomFinBase = {
                center.x + facing * (radius * 0.1f),
                center.y + radius * 0.8f
            };
            
            Vector2 bottomFinTip = {
                center.x + facing * (radius * 0.4f + finWave * radius * 0.8f),
                center.y + radius * 1.3f
            };
            
            // Desenhar barbatanas
            DrawPixelLine(topFinBase.x, topFinBase.y, topFinTip.x, topFinTip.y, finColor);
            DrawPixelLine(bottomFinBase.x, bottomFinBase.y, bottomFinTip.x, bottomFinTip.y, finColor);
            
            // Barbatana lateral
            Vector2 sideFinBase = {
                center.x,
                center.y + radius * 0.2f
            };
            
            Vector2 sideFinTip = {
                center.x - facing * (radius * 0.8f),
                center.y + radius * 0.2f + finWave * radius
            };
            
            DrawPixelLine(sideFinBase.x, sideFinBase.y, sideFinTip.x, sideFinTip.y, finColor);
            
            // Olho
            Vector2 eyePos = {
                center.x + facing * radius * 0.5f,
                center.y - radius * 0.2f
            };
            
            DrawPixelCircle(eyePos.x, eyePos.y, radius * 0.25f, eyeColor);
            
            // Pupila (se move levemente com a animação)
            float pupilOffset = sinf(fishTime * 0.5f) * 0.1f;
            DrawPixelCircle(
                eyePos.x + facing * pupilOffset * radius,
                eyePos.y + pupilOffset * radius,
                radius * 0.12f,
                pupilColor
            );
            
            // Detalhes das brânquias (arcos na lateral da cabeça)
            Vector2 gillPos = {
                center.x + facing * radius * 0.3f,
                center.y
            };
            
            for (int i = 0; i < 2; i++) {
                float yOffset = (i - 0.5f) * radius * 0.4f;
                float gillAngle = PI * 0.5f;
                float startAngle = angle + (facing > 0 ? PI - gillAngle/2 : gillAngle/2);
                
                for (float a = 0; a < gillAngle; a += 0.2f) {
                    float px = gillPos.x + cosf(startAngle + (facing > 0 ? a : -a)) * radius * 0.4f;
                    float py = gillPos.y + yOffset + sinf(startAngle + (facing > 0 ? a : -a)) * radius * 0.4f;
                    
                    DrawPixelCircle(px, py, 1, Fade(DARKBLUE, 0.5f));
                }
            }
            
            // Efeito de borbulhas ocasionais
            static float bubbleTimer = 0.0f;
            static Vector2 bubblePos = {0, 0};
            static float bubbleSize = 0.0f;
            static bool activeBubble = false;
            
            bubbleTimer -= GetFrameTime();
            
            if (bubbleTimer <= 0.0f && !activeBubble) {
                // Criar nova bolha
                bubbleTimer = GetRandomValue(10, 30) / 10.0f; // 1-3 segundos
                bubblePos.x = center.x - facing * radius * 0.9f;
                bubblePos.y = center.y + GetRandomValue(-10, 10);
                bubbleSize = GetRandomValue(2, 5);
                activeBubble = true;
            }
            
            if (activeBubble) {
                // Animar bolha existente
                bubblePos.y -= GetFrameTime() * 30.0f; // Subir
                bubblePos.x -= facing * GetFrameTime() * 10.0f; // Seguir para trás do peixe
                
                // Desenhar bolha
                DrawPixelCircle(bubblePos.x, bubblePos.y, bubbleSize, Fade(LIGHTGRAY, 0.4f));
                
                // Verificar se bolha saiu da faixa visível
                if (bubblePos.y < center.y - radius * 3) {
                    activeBubble = false;
                }
            }
            
            // Efeito de "brilho" no corpo para sensação de escamas
            for (int i = 0; i < 5; i++) {
                float shimmerAngle = fishTime * 0.3f + i * 0.8f;
                float sx = center.x + cosf(shimmerAngle) * radius * 0.6f;
                float sy = center.y + sinf(shimmerAngle) * radius * 0.4f;
                
                DrawPixelCircle(sx, sy, 2, Fade(WHITE, 0.2f));
            }
        }
    }

    // Desenhar inimigos como gorilas e macacos
    if (enemies) {
        static float primateTimes[5] = {0}; 
        static float randomOffsets[10] = {0}; 
        static bool initialized = false;
        
        // Inicializar offsets aleatórios uma vez
        if (!initialized) {
            for (int i = 0; i < 10; i++) {
                randomOffsets[i] = GetRandomValue(0, 100) / 100.0f;
            }
            initialized = true;
        }
        
        // Atualizar timers de animação
        for (int i = 0; i < 5; i++) {
            primateTimes[i] += GetFrameTime() * (2.0f + i * 0.3f);
        }
        
        const Enemy *currentEnemy = enemies->head;
        while (currentEnemy != NULL) {
            if (currentEnemy->active) {
                // Pegar tipo e posição
                EnemyType type = currentEnemy->type;
                Vector2 position = currentEnemy->position;
                float radius = currentEnemy->radius;
                float animTime = primateTimes[type % 5];
                
                // Ajustar tamanho de acordo com o tipo
                float sizeMultiplier = 1.0f;
                switch (type) {
                    case ENEMY_TYPE_SPEEDER: sizeMultiplier = 1.8f; break;
                    case ENEMY_TYPE_SHOOTER: sizeMultiplier = 1.5f; break;
                    case ENEMY_TYPE_NORMAL: sizeMultiplier = 1.4f; break;
                    case ENEMY_TYPE_EXPLODER: sizeMultiplier = 1.3f; break;
                    case ENEMY_TYPE_TANK: sizeMultiplier = 1.2f; break;
                }
                radius *= sizeMultiplier;
                
                // Definir cores base
                Color bodyColor = WHITE;
                Color faceColor = LIGHTGRAY;
                Color detailColor = DARKGRAY;
                
                // Cores específicas por tipo
                switch (type) {
                    case ENEMY_TYPE_TANK: // Gorila prateado
                        bodyColor = (Color){120, 120, 120, 255};
                        faceColor = (Color){80, 80, 80, 255};
                        detailColor = (Color){50, 50, 50, 255};
                        break;
                        
                    case ENEMY_TYPE_EXPLODER: // Babuíno vermelho
                        bodyColor = (Color){180, 100, 100, 255};
                        faceColor = (Color){200, 120, 120, 255};
                        detailColor = (Color){140, 60, 60, 255};
                        break;
                        
                    case ENEMY_TYPE_SHOOTER: // Chimpanzé marrom
                        bodyColor = (Color){110, 90, 70, 255};
                        faceColor = (Color){150, 130, 110, 255};
                        detailColor = (Color){80, 60, 40, 255};
                        break;
                        
                    case ENEMY_TYPE_SPEEDER: // Sagui amarelado
                        bodyColor = (Color){180, 170, 130, 255};
                        faceColor = (Color){210, 200, 170, 255};
                        detailColor = (Color){150, 140, 100, 255};
                        break;
                        
                    case ENEMY_TYPE_NORMAL: // Macaco comum
                    default:
                        bodyColor = (Color){130, 110, 90, 255};
                        faceColor = (Color){160, 140, 120, 255};
                        detailColor = (Color){100, 80, 60, 255};
                        break;
                }
                
                // ----- ANIMAÇÕES PROCEDURAIS COMUNS A TODOS OS TIPOS -----
                
                // Modo de movimento - alterna entre andar/correr dependendo da velocidade
                bool isRunning = Vector2Length(currentEnemy->velocity) > 70.0f;
                
                // Calcular ciclo de respiração
                float breathingRate = isRunning ? 5.0f : 1.5f;
                float breathFactor = sinf(animTime * breathingRate) * 0.1f + 0.95f;
                
                // Calcular ciclo de caminhada/corrida
                float moveRate = isRunning ? 8.0f : 3.0f;
                float moveCycle = sinf(animTime * moveRate + randomOffsets[type]) * 0.5f;
                
                // Sistema de movimentação pendular para simular caminhada quadrúpede
                float bodyTilt = sinf(animTime * moveRate * 0.5f) * 0.15f; // Inclinação do corpo
                float limbSwing = sinf(animTime * moveRate) * 0.4f; // Movimento dos membros
                
                // ----- DESENHO DO CORPO BASE -----
                
                // Calcular pontos para o corpo em formato de gota/pêra
                float bodyWidth = radius * 0.9f * (1.0f + bodyTilt * 0.3f) * breathFactor;
                float bodyHeight = radius * 1.1f * (1.0f - bodyTilt * 0.1f);
                
                // Desenhar corpo principal como uma forma de "gota"
                DrawPixelCircle(position.x, position.y, bodyWidth, bodyColor);
                
                // Adicionar sombreamento ao corpo para dar profundidade
                for (int i = 0; i < 3; i++) {
                    float shadowAngle = PI * 0.25f + i * 0.2f;
                    float shadowX = position.x + cosf(shadowAngle) * bodyWidth * 0.5f;
                    float shadowY = position.y + sinf(shadowAngle) * bodyHeight * 0.5f;
                    DrawPixelCircle(shadowX, shadowY, bodyWidth * 0.25f, Fade(detailColor, 0.3f));
                }
                
                // ----- CABEÇA E CARACTERÍSTICAS FACIAIS -----
                
                // Posição da cabeça com leve oscilação baseada no movimento
                Vector2 headPos = {
                    position.x + sinf(animTime * moveRate * 0.5f) * radius * 0.1f,
                    position.y - bodyHeight * 0.55f - (isRunning ? sinf(animTime * moveRate) * radius * 0.1f : 0)
                };
                
                // Tamanho da cabeça varia por tipo
                float headSize = radius * (type == ENEMY_TYPE_TANK ? 0.65f : 0.7f);
                
                // Desenhar cabeça
                DrawPixelCircle(headPos.x, headPos.y, headSize, faceColor);
                
                // Expressão facial (olhos e boca) - muda com estado e tipo
                float emoteState = (sinf(animTime * 1.5f) > 0.7f) ? 1.0f : 0.0f; // Alternar expressões
                
                // Calcular base para olhos
                float eyeSpacing = headSize * (type == ENEMY_TYPE_TANK ? 0.4f : 0.5f);
                Vector2 leftEye = {headPos.x - eyeSpacing, headPos.y - headSize * 0.1f};
                Vector2 rightEye = {headPos.x + eyeSpacing, headPos.y - headSize * 0.1f};
                
                // Tamanho dos olhos varia por tipo
                float eyeSize = headSize * 0.2f;
                
                // Sistema de piscar - mais natural e aleatório
                float blinkRandom = sinf(animTime * 5.0f + randomOffsets[type] * 10.0f);
                float blinkThreshold = 0.95f; // Pisca 5% do tempo
                float blinkFactor = (blinkRandom > blinkThreshold) ? 
                                 (1.0f - (blinkRandom - blinkThreshold) * 20.0f) : 1.0f;
                blinkFactor = blinkFactor < 0.1f ? 0.1f : blinkFactor;
                
                // Sistema de olhar - olhos se movem naturalmente
                float eyeLook = sinf(animTime * 0.7f + randomOffsets[type]) * 0.3f;
                float eyeVertical = cosf(animTime * 0.5f) * 0.2f;
                
                // Desenhar brancos dos olhos
                DrawPixelCircle(leftEye.x, leftEye.y, eyeSize, WHITE);
                DrawPixelCircle(rightEye.x, rightEye.y, eyeSize, WHITE);
                
                // Desenhar pupilas com movimento e piscada
                DrawPixelCircle(
                    leftEye.x + eyeLook * eyeSize, 
                    leftEye.y + eyeVertical * eyeSize, 
                    eyeSize * 0.6f * blinkFactor, 
                    BLACK
                );
                DrawPixelCircle(
                    rightEye.x + eyeLook * eyeSize, 
                    rightEye.y + eyeVertical * eyeSize, 
                    eyeSize * 0.6f * blinkFactor, 
                    BLACK
                );
                
                // ----- CARACTERÍSTICAS ESPECÍFICAS POR TIPO DE PRIMATA -----
                
                if (type == ENEMY_TYPE_TANK) { // GORILA
                    // Postura característica de gorila - mais curvada e quadrúpede
                    // Ombros largos e proeminentes
                    float shoulderWidth = radius * 1.3f;
                    Vector2 leftShoulder = {
                        position.x - shoulderWidth * 0.5f,
                        position.y - radius * 0.3f + sinf(animTime * moveRate) * radius * 0.1f
                    };
                    Vector2 rightShoulder = {
                        position.x + shoulderWidth * 0.5f,
                        position.y - radius * 0.3f - sinf(animTime * moveRate) * radius * 0.1f
                    };
                    
                    // Braços grossos com articulações
                    float knuckleHeight = position.y + radius * 0.5f;
                    
                    // Braço esquerdo - movimento de caminhar quadrúpede
                    Vector2 leftElbow = {
                        leftShoulder.x - radius * 0.3f,
                        leftShoulder.y + radius * 0.5f + limbSwing * radius * 0.5f
                    };
                    Vector2 leftKnuckle = {
                        leftElbow.x - radius * 0.4f,
                        knuckleHeight - limbSwing * radius * 0.7f
                    };
                    
                    // Braço direito - movimento alternado
                    Vector2 rightElbow = {
                        rightShoulder.x + radius * 0.3f,
                        rightShoulder.y + radius * 0.5f - limbSwing * radius * 0.5f
                    };
                    Vector2 rightKnuckle = {
                        rightElbow.x + radius * 0.4f,
                        knuckleHeight + limbSwing * radius * 0.7f
                    };
                    
                    // Desenhar ombros volumosos
                    DrawPixelCircle(leftShoulder.x, leftShoulder.y, radius * 0.3f, bodyColor);
                    DrawPixelCircle(rightShoulder.x, rightShoulder.y, radius * 0.3f, bodyColor);
                    
                    // Desenhar braços com articulações
                    DrawPixelLine(leftShoulder.x, leftShoulder.y, leftElbow.x, leftElbow.y, bodyColor);
                    DrawPixelLine(leftElbow.x, leftElbow.y, leftKnuckle.x, leftKnuckle.y, bodyColor);
                    DrawPixelCircle(leftElbow.x, leftElbow.y, radius * 0.25f, bodyColor);
                    DrawPixelCircle(leftKnuckle.x, leftKnuckle.y, radius * 0.3f, bodyColor);
                    
                    DrawPixelLine(rightShoulder.x, rightShoulder.y, rightElbow.x, rightElbow.y, bodyColor);
                    DrawPixelLine(rightElbow.x, rightElbow.y, rightKnuckle.x, rightKnuckle.y, bodyColor);
                    DrawPixelCircle(rightElbow.x, rightElbow.y, radius * 0.25f, bodyColor);
                    DrawPixelCircle(rightKnuckle.x, rightKnuckle.y, radius * 0.3f, bodyColor);
                    
                    // Rosto característico de gorila
                    // Testa proeminente
                    DrawPixelCircle(
                        headPos.x, 
                        headPos.y - headSize * 0.3f, 
                        headSize * 0.7f, 
                        faceColor
                    );
                    
                    // Sobrancelhas grossas arqueadas
                    float browAngle = 0.2f + (emoteState * 0.2f); // Varia com expressão
                    float browWidth = headSize * 0.5f;
                    
                    // Sobrancelha esquerda
                    for (float t = 0.0f; t <= 1.0f; t += 0.2f) {
                        float x = leftEye.x - browWidth * 0.5f + t * browWidth;
                        float y = leftEye.y - eyeSize - 3 + sinf(t * PI) * browAngle * 10;
                        DrawPixelCircle(x, y, 2, detailColor);
                    }
                    
                    // Sobrancelha direita
                    for (float t = 0.0f; t <= 1.0f; t += 0.2f) {
                        float x = rightEye.x - browWidth * 0.5f + t * browWidth;
                        float y = rightEye.y - eyeSize - 3 + sinf(t * PI) * browAngle * 10;
                        DrawPixelCircle(x, y, 2, detailColor);
                    }
                    
                    // Focinho preto característico
                    Vector2 muzzlePos = {
                        headPos.x,
                        headPos.y + headSize * 0.3f
                    };
                    DrawPixelCircle(muzzlePos.x, muzzlePos.y, headSize * 0.6f, Fade(detailColor, 0.8f));
                    
                    // Narinas
                    DrawPixelCircle(muzzlePos.x - headSize * 0.2f, muzzlePos.y, headSize * 0.1f, BLACK);
                    DrawPixelCircle(muzzlePos.x + headSize * 0.2f, muzzlePos.y, headSize * 0.1f, BLACK);
                    
                    // Boca - expressão varia
                    float mouthOffset = emoteState > 0.5f ? -3 : 3; // Boca para cima ou para baixo
                    for (float t = 0.2f; t <= 0.8f; t += 0.2f) {
                        float mouthWidth = headSize * 0.6f;
                        float x = muzzlePos.x - mouthWidth * 0.5f + t * mouthWidth;
                        float y = muzzlePos.y + headSize * 0.15f + sinf(t * PI) * mouthOffset;
                        DrawPixelCircle(x, y, 1.5f, BLACK);
                    }
                    
                    // Animação de bater no peito (mais complexa e realista)
                    float chestCycle = fmodf(animTime, 6.0f); // Ciclo mais longo
                    if (chestCycle < 1.2f) {
                        // Fase de preparação - mãos se levantam
                        if (chestCycle < 0.3f) {
                            float t = chestCycle / 0.3f;
                            float chestX = position.x + sinf(t * PI) * radius * 0.2f;
                            float chestY = position.y - radius * 0.3f;
                            
                            // Reposicionar braços
                            Vector2 prepLeftHand = {
                                leftShoulder.x + (chestX - leftShoulder.x) * t,
                                leftShoulder.y + (chestY - leftShoulder.y) * t
                            };
                            Vector2 prepRightHand = {
                                rightShoulder.x + (chestX - rightShoulder.x) * t,
                                rightShoulder.y + (chestY - rightShoulder.y) * t
                            };
                            
                            DrawPixelLine(leftShoulder.x, leftShoulder.y, prepLeftHand.x, prepLeftHand.y, bodyColor);
                            DrawPixelLine(rightShoulder.x, rightShoulder.y, prepRightHand.x, prepRightHand.y, bodyColor);
                        }
                        // Fase de batidas no peito
                        else if (chestCycle < 1.0f) {
                            // Calcular posição das mãos batendo
                            float beatPhase = sinf((chestCycle - 0.3f) * 15.0f); // Batidas rápidas
                            float leftOffset = beatPhase > 0 ? radius * 0.3f : 0;
                            float rightOffset = beatPhase < 0 ? radius * 0.3f : 0;
                            
                            // Mão esquerda
                            Vector2 leftHand = {
                                position.x - leftOffset, 
                                position.y - radius * 0.3f
                            };
                            
                            // Mão direita
                            Vector2 rightHand = {
                                position.x + rightOffset, 
                                position.y - radius * 0.3f
                            };
                            
                            // Desenhar braços
                            DrawPixelLine(leftShoulder.x, leftShoulder.y, leftHand.x, leftHand.y, bodyColor);
                            DrawPixelLine(rightShoulder.x, rightShoulder.y, rightHand.x, rightHand.y, bodyColor);
                            
                            // Desenhar mãos
                            DrawPixelCircle(leftHand.x, leftHand.y, radius * 0.25f, bodyColor);
                            DrawPixelCircle(rightHand.x, rightHand.y, radius * 0.25f, bodyColor);
                            
                            // Efeito de impacto no peito
                            if (fabsf(beatPhase) > 0.8f) {
                                DrawPixelCircle(
                                    position.x + (beatPhase > 0 ? -radius * 0.2f : radius * 0.2f), 
                                    position.y - radius * 0.1f,
                                    radius * 0.2f * fabsf(beatPhase),
                                    Fade(WHITE, 0.4f * fabsf(beatPhase))
                                );
                            }
                            
                            // Expressão facial mais agressiva durante batidas
                            if (beatPhase > 0.5f) {
                                DrawPixelCircle(
                                    headPos.x, 
                                    headPos.y + headSize * 0.1f,
                                    headSize * 0.3f * beatPhase,
                                    Fade(RED, 0.2f * beatPhase)
                                );
                            }
                        }
                    }
                }
                else if (type == ENEMY_TYPE_EXPLODER) { // Babuíno vermelho
                    // Focinho proeminente
                    Vector2 snoutPos = {
                        headPos.x + headSize * 0.2f,
                        headPos.y + headSize * 0.2f
                    };
                    DrawPixelCircle(snoutPos.x, snoutPos.y, headSize * 0.4f, detailColor);
                    
                    // Juba/pelo volumoso
                    for (int i = 0; i < 6; i++) {
                        float angle = i * PI / 3.0f + animTime * 0.2f;
                        float dist = headSize * (1.1f + sinf(animTime * 2.0f + i) * 0.1f);
                        float x = headPos.x + cosf(angle) * dist;
                        float y = headPos.y + sinf(angle) * dist;
                        
                        DrawPixelCircle(x, y, headSize * 0.25f, bodyColor);
                    }
                    
                    // Efeito de "raiva" quando vai explodir
                    float angerFactor = sinf(animTime * 3.0f);
                    if (angerFactor > 0.7f) {
                        DrawPixelCircle(
                            headPos.x, headPos.y,
                            headSize * angerFactor * 0.5f,
                            Fade(RED, angerFactor * 0.3f)
                        );
                    }
                }
                else if (type == ENEMY_TYPE_SHOOTER) { // CHIMPANZÉ
                    // Braços mais compridos para arremessar
                    float armLength = radius * 1.1f;
                    float shoulderOffset = radius * 0.3f;
                    
                    // O braço que atira se estende mais
                    Vector2 shoulder = {position.x + shoulderOffset, position.y - radius * 0.1f};
                    Vector2 hand = {
                        shoulder.x + armLength * cosf(animTime * 2.0f) * 0.5f + armLength * 0.5f,
                        shoulder.y + armLength * sinf(animTime * 2.0f) * 0.5f
                    };
                    
                    DrawPixelLine(shoulder.x, shoulder.y, hand.x, hand.y, bodyColor);
                    DrawPixelCircle(hand.x, hand.y, radius * 0.2f, bodyColor);
                    
                    // Ocasionalmente mostra um "projétil" na mão
                    float shootPhase = fmodf(animTime, 3.0f);
                    if (shootPhase < 1.0f) {
                        DrawPixelCircle(
                            hand.x, hand.y,
                            radius * 0.15f * (1.0f - shootPhase),
                            YELLOW
                        );
                    }
                    
                    // Orelhas grandes
                    DrawPixelCircle(headPos.x - headSize * 0.7f, headPos.y - headSize * 0.3f, 
                                   headSize * 0.3f, Fade(faceColor, 0.7f));
                    DrawPixelCircle(headPos.x + headSize * 0.7f, headPos.y - headSize * 0.3f, 
                                   headSize * 0.3f, Fade(faceColor, 0.7f));
                }
                else if (type == ENEMY_TYPE_SPEEDER) { // SAGUI PEQUENO E ÁGIL
                    // Cauda longa e ondulante
                    float tailLength = radius * 2.0f;
                    Vector2 tailBase = {position.x, position.y + radius * 0.6f};
                    
                    for (int i = 0; i < 8; i++) {
                        float t = i / 8.0f;
                        float waveOffset = sinf(animTime * 3.0f + t * PI * 2) * radius * 0.5f;
                        
                        Vector2 tailPoint = {
                            tailBase.x + waveOffset,
                            tailBase.y + t * tailLength
                        };
                        
                        DrawPixelCircle(tailPoint.x, tailPoint.y, radius * 0.15f * (1.0f - t * 0.7f), bodyColor);
                    }
                    
                    // Orelhas pontudas
                    DrawPixelCircle(headPos.x - headSize * 0.5f, headPos.y - headSize * 0.6f, 
                                   headSize * 0.25f, faceColor);
                    DrawPixelCircle(headPos.x + headSize * 0.5f, headPos.y - headSize * 0.6f, 
                                   headSize * 0.25f, faceColor);
                    
                    // Efeitode velocidade/movimento
                    if (Vector2Length(currentEnemy->velocity) > 50.0f) {
                        Vector2 moveDir = Vector2Normalize(currentEnemy->velocity);
                        Vector2 trailPos = Vector2Subtract(position, Vector2Scale(moveDir, radius * 1.2f));
                        
                        DrawPixelCircle(trailPos.x, trailPos.y, radius * 0.5f, Fade(bodyColor, 0.3f));
                    }
                }
                else { // MACACO COMUM (NORMAL) - Animação mais fluida como no vídeo referência
                    // Sistema de tempo para diferentes partes da animação
                    float bounceTime = sinf(animTime * 2.5f) * 0.5f + 0.5f;
                    // float tailTime = cosf(animTime * 1.8f); // Variável não utilizada
                    float breathTime = sinf(animTime * 0.7f);
                    
                    // Parâmetros escaláveis
                    float limbLength = radius * 0.8f;
                    float shoulderOffset = radius * 0.4f;
                    float bounceHeight = radius * 0.15f * bounceTime;
                    
                    // Efeito de pulo/quique - corpo sobe e desce
                    Vector2 bouncedPosition = {
                        position.x,
                        position.y - bounceHeight
                    };
                    
                    // Corpo principal com efeito de respiração
                    float breathScale = 1.0f + breathTime * 0.1f;
                    DrawPixelCircle(bouncedPosition.x, bouncedPosition.y, bodyWidth * breathScale, bodyColor);
                    
                    // Sombra abaixo do corpo (para efeito de altura)
                    float shadowSize = bodyWidth * (0.8f - bounceHeight/radius * 0.3f);
                    DrawPixelCircle(position.x, position.y + radius * 0.1f, shadowSize, Fade(BLACK, 0.2f));
                    
                    // Braços com movimento de balanço mais natural
                    float armAngle1 = -PI/4 + sinf(animTime * 3.5f) * 0.4f;
                    float armAngle2 = -PI/4 - sinf(animTime * 3.5f) * 0.4f;
                    
                    // Braço esquerdo com juntas articuladas
                    Vector2 shoulder1 = {bouncedPosition.x - shoulderOffset, bouncedPosition.y - radius * 0.1f};
                    Vector2 elbow1 = {
                        shoulder1.x + cosf(armAngle1) * limbLength * 0.5f,
                        shoulder1.y + sinf(armAngle1) * limbLength * 0.5f
                    };
                    Vector2 hand1 = {
                        elbow1.x + cosf(armAngle1 + sinf(animTime * 2.8f) * 0.3f) * limbLength * 0.5f,
                        elbow1.y + sinf(armAngle1 + sinf(animTime * 2.8f) * 0.3f) * limbLength * 0.5f
                    };
                    
                    // Desenhar braço esquerdo
                    DrawPixelLine(shoulder1.x, shoulder1.y, elbow1.x, elbow1.y, bodyColor);
                    DrawPixelLine(elbow1.x, elbow1.y, hand1.x, hand1.y, bodyColor);
                    DrawPixelCircle(elbow1.x, elbow1.y, radius * 0.15f, bodyColor);
                    DrawPixelCircle(hand1.x, hand1.y, radius * 0.2f, bodyColor);
                    
                    // Braço direito com juntas articuladas
                    Vector2 shoulder2 = {bouncedPosition.x + shoulderOffset, bouncedPosition.y - radius * 0.1f};
                    Vector2 elbow2 = {
                        shoulder2.x + cosf(armAngle2) * limbLength * 0.5f,
                        shoulder2.y + sinf(armAngle2) * limbLength * 0.5f
                    };
                    Vector2 hand2 = {
                        elbow2.x + cosf(armAngle2 - sinf(animTime * 2.8f) * 0.3f) * limbLength * 0.5f,
                        elbow2.y + sinf(armAngle2 - sinf(animTime * 2.8f) * 0.3f) * limbLength * 0.5f
                    };
                    
                    // Desenhar braço direito
                    DrawPixelLine(shoulder2.x, shoulder2.y, elbow2.x, elbow2.y, bodyColor);
                    DrawPixelLine(elbow2.x, elbow2.y, hand2.x, hand2.y, bodyColor);
                    DrawPixelCircle(elbow2.x, elbow2.y, radius * 0.15f, bodyColor);
                    DrawPixelCircle(hand2.x, hand2.y, radius * 0.2f, bodyColor);
                    
                    // Pernas com movimento de oscilação oposto ao corpo
                    float legAngle1 = PI/4 - sinf(animTime * 3.5f) * 0.3f;
                    float legAngle2 = PI/4 + sinf(animTime * 3.5f) * 0.3f;
                    
                    // Perna esquerda
                    Vector2 hip1 = {bouncedPosition.x - shoulderOffset * 0.7f, bouncedPosition.y + radius * 0.3f};
                    Vector2 knee1 = {
                        hip1.x + cosf(legAngle1) * limbLength * 0.5f,
                        hip1.y + sinf(legAngle1) * limbLength * 0.5f
                    };
                    Vector2 foot1 = {
                        knee1.x + cosf(legAngle1 + sinf(animTime * 2.8f + PI) * 0.4f) * limbLength * 0.6f,
                        knee1.y + sinf(legAngle1 + sinf(animTime * 2.8f + PI) * 0.4f) * limbLength * 0.6f
                    };
                    
                    // Desenhar perna esquerda
                    DrawPixelLine(hip1.x, hip1.y, knee1.x, knee1.y, bodyColor);
                    DrawPixelLine(knee1.x, knee1.y, foot1.x, foot1.y, bodyColor);
                    DrawPixelCircle(knee1.x, knee1.y, radius * 0.15f, bodyColor);
                    DrawPixelCircle(foot1.x, foot1.y, radius * 0.22f, bodyColor);
                    
                    // Perna direita
                    Vector2 hip2 = {bouncedPosition.x + shoulderOffset * 0.7f, bouncedPosition.y + radius * 0.3f};
                    Vector2 knee2 = {
                        hip2.x + cosf(legAngle2) * limbLength * 0.5f,
                        hip2.y + sinf(legAngle2) * limbLength * 0.5f
                    };
                    Vector2 foot2 = {
                        knee2.x + cosf(legAngle2 - sinf(animTime * 2.8f + PI) * 0.4f) * limbLength * 0.6f,
                        knee2.y + sinf(legAngle2 - sinf(animTime * 2.8f + PI) * 0.4f) * limbLength * 0.6f
                    };
                    
                    // Desenhar perna direita
                    DrawPixelLine(hip2.x, hip2.y, knee2.x, knee2.y, bodyColor);
                    DrawPixelLine(knee2.x, knee2.y, foot2.x, foot2.y, bodyColor);
                    DrawPixelCircle(knee2.x, knee2.y, radius * 0.15f, bodyColor);
                    DrawPixelCircle(foot2.x, foot2.y, radius * 0.22f, bodyColor);
                    
                    // Cauda com movimento ondulante mais elaborado
                    float tailLength = radius * 2.0f;
                    Vector2 tailBase = {bouncedPosition.x, bouncedPosition.y + radius * 0.4f};
                    
                    int tailSegments = 6;
                    Vector2 prevPoint = tailBase;
                    
                    for (int i = 0; i < tailSegments; i++) {
                        float t = (float)i / (tailSegments - 1);
                        float waveOffset = sinf(animTime * 3.0f + t * PI * 2) * radius * (0.3f + t * 0.4f);
                        float segmentLength = tailLength * (1.0f - t) / tailSegments * 1.5f;
                        
                        Vector2 tailPoint = {
                            prevPoint.x + waveOffset,
                            prevPoint.y + segmentLength
                        };
                        
                        DrawPixelLine(prevPoint.x, prevPoint.y, tailPoint.x, tailPoint.y, bodyColor);
                        DrawPixelCircle(tailPoint.x, tailPoint.y, radius * 0.15f * (1.0f - t * 0.7f), bodyColor);
                        
                        prevPoint = tailPoint;
                    }
                    
                    // Cabeça na posição certa
                    Vector2 headPos = {
                        bouncedPosition.x + sinf(animTime * 2.0f) * radius * 0.08f,
                        bouncedPosition.y - bodyHeight * 0.55f + sinf(animTime * 3.5f) * radius * 0.08f
                    };
                    
                    // Desenhar cabeça
                    DrawPixelCircle(headPos.x, headPos.y, headSize, faceColor);
                    
                    // Expressão mais elaborada - sobrancelhas que se movem
                    float browRaise = sinf(animTime * 1.2f) * 0.5f + 0.5f;
                    
                    // Sobrancelhas
                    for (int i = -1; i <= 1; i += 2) {
                        float x = headPos.x + i * eyeSpacing;
                        float y = headPos.y - headSize * 0.2f - browRaise * headSize * 0.1f;
                        float width = headSize * 0.25f;
                        
                        for (float t = 0; t <= 1.0f; t += 0.2f) {
                            float px = x - width/2 + t * width;
                            float py = y + sinf(t * PI) * headSize * 0.05f;
                            DrawPixelCircle(px, py, 1.5f, detailColor);
                        }
                    }
                    
                    // Boca mais expressiva que varia conforme o tempo
                    float mouthOpen = (sinf(animTime * 1.5f) * 0.5f + 0.5f) * 0.7f;
                    if (mouthOpen > 0.3f) {
                        // Boca aberta quando o valor de mouthOpen é alto
                        DrawPixelCircle(headPos.x, headPos.y + headSize * 0.2f, headSize * 0.2f * mouthOpen, Fade(BLACK, 0.7f));
                    } else {
                        // Linha para boca fechada
                        for (float t = 0.3f; t <= 0.7f; t += 0.1f) {
                            float x = headPos.x - headSize * 0.3f + t * headSize * 0.6f;
                            float y = headPos.y + headSize * 0.2f + sinf(t * PI) * headSize * 0.05f;
                            DrawPixelCircle(x, y, 1.0f, BLACK);
                        }
                    }
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
    
    // Desenhar projéteis dos inimigos como bananas
    static float enemyBulletRotation = 0.0f;
    enemyBulletRotation += GetFrameTime() * 5.0f;
    
    if (enemyBullets) {
        const Bullet *currentBullet = enemyBullets;
        while (currentBullet != NULL) {
            if (currentBullet->active) {
                // Identificar origem do projétil
                bool isShooterBullet = false;
                if (currentBullet->radius >= BULLET_RADIUS * 1.2f) {
                    isShooterBullet = true;
                }
                
                Vector2 center = currentBullet->position;
                float angle = atan2f(currentBullet->velocity.y, currentBullet->velocity.x);
                
                // Tamanho da banana
                float bananaLength = currentBullet->radius * 3.5f;
                float bananaWidth = currentBullet->radius * 1.5f;
                
                // Cor base (amarelo para banana)
                Color bananaColor = isShooterBullet ? GOLD : YELLOW;
                Color peelColor = isShooterBullet ? ORANGE : (Color){255, 200, 0, 255};
                
                // Desenhar banana com curvatura
                float segments = 8.0f;
                Vector2 prev = {0};
                
                for (int i = 0; i <= segments; i++) {
                    float t = i/segments;
                    float bananaAngle = angle + PI * 0.3f * sinf(t * PI - PI/2);
                    float dist = bananaLength * t;
                    
                    Vector2 point = {
                        center.x + cosf(bananaAngle) * dist,
                        center.y + sinf(bananaAngle) * dist
                    };
                    
                    if (i > 0) {
                        DrawPixelLine(prev.x, prev.y, point.x, point.y, bananaColor);
                        
                        // Desenhar largura da banana (casca)
                        float perpAngle = bananaAngle + PI/2;
                        float width = bananaWidth * (0.9f - 0.8f * (t - 0.5f) * (t - 0.5f));
                        
                        Vector2 top = {
                            point.x + cosf(perpAngle) * width * 0.5f,
                            point.y + sinf(perpAngle) * width * 0.5f
                        };
                        
                        Vector2 bottom = {
                            point.x - cosf(perpAngle) * width * 0.5f,
                            point.y - sinf(perpAngle) * width * 0.5f
                        };
                        
                        DrawPixelLine(top.x, top.y, bottom.x, bottom.y, peelColor);
                    }
                    
                    // Detalhes das pontas da banana
                    if (i == 0 || i == segments) {
                        DrawPixelCircle(point.x, point.y, bananaWidth * 0.3f, peelColor);
                    }
                    
                    prev = point;
                }
                
                // Adicionar detalhes de manchas na banana
                for (int i = 0; i < 3; i++) {
                    float spotAngle = angle + enemyBulletRotation * 0.2f + i * PI * 0.5f;
                    float spotDist = bananaLength * 0.3f + i * bananaLength * 0.2f;
                    float spotSize = bananaWidth * 0.15f;
                    
                    Vector2 spotPos = {
                        center.x + cosf(spotAngle) * spotDist,
                        center.y + sinf(spotAngle) * spotDist
                    };
                    
                    DrawPixelCircle(spotPos.x, spotPos.y, spotSize, BROWN);
                }
                
                // Efeito de rotação da banana
                if (isShooterBullet) {
                    DrawPixelCircle(center.x, center.y, bananaWidth * 0.2f, Fade(WHITE, 0.7f));
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
    float baseSize = 18.0f; 
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