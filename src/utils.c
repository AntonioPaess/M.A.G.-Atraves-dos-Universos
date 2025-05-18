#include "utils.h"
// raylib.h é incluído através de utils.h
#include "raymath.h"  // Adicione esta linha para ter acesso às funções Vector2Distance
#include <math.h> // Para sqrtf se necessário em outras funções, mas não para AppToggleFullscreen

// Definição da variável global
bool isFullscreenGlobal = false;

// Essas variáveis devem estar definidas apenas em utils.c
float PLAY_AREA_LEFT = PLAY_AREA_MARGIN;
float PLAY_AREA_TOP = PLAY_AREA_MARGIN;
float PLAY_AREA_RIGHT = SCREEN_WIDTH - PLAY_AREA_MARGIN;
float PLAY_AREA_BOTTOM = SCREEN_HEIGHT - PLAY_AREA_MARGIN;

// Adicione estas variáveis antes de UpdateScreenSizeVars
float currentPlayAreaRadius;  // Raio atual da área jogável
float targetPlayAreaRadius;   // Raio alvo para transição suave
float areaChangeTimer = 0.3f;                    // Controla o tempo entre mudanças
float areaTransitionSpeed = 1.0f;                // Velocidade da transição
bool isAreaShrinking = false;                    // Controla direção da mudança

void UpdateScreenSizeVars(void) {
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();
    
    // Garantir que PLAY_AREA_TOP comece após o HUD (que tem 60 pixels de altura)
    float hudHeight = 60.0f;
    float extraMargin = 10.0f; // Margem adicional para visual
    
    // Calcular áreas com base no raio atual
    PLAY_AREA_LEFT = (currentWidth / 2.0f) - currentPlayAreaRadius;
    PLAY_AREA_RIGHT = (currentWidth / 2.0f) + currentPlayAreaRadius;
    PLAY_AREA_TOP = MAX(hudHeight + extraMargin, (currentHeight / 2.0f) - currentPlayAreaRadius);
    PLAY_AREA_BOTTOM = (currentHeight / 2.0f) + currentPlayAreaRadius;
}

void AppToggleFullscreen(void) {
    // Chama a função ToggleFullscreen() da biblioteca Raylib
    ToggleFullscreen(); // Esta é a chamada para a função da Raylib

    // Atualiza nosso rastreador de estado global
    isFullscreenGlobal = !isFullscreenGlobal;
}

// Função para interpolação linear - ÚNICA implementação
float LerpUtil(float start, float end, float t) {
    return start + t * (end - start);
}

bool IsPointInPlayArea(Vector2 point) {
    float distanceToCenter = Vector2Distance(
        point,
        (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}
    );
    
    return distanceToCenter <= currentPlayAreaRadius;
}

// Inicializa as variáveis de área - modificado para começar com tamanho estável
void InitPlayArea(void) {
    // Calcula o raio inicial - começar com tamanho normal (não aumentado)
    float baseRadius = fminf(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f - PLAY_AREA_MARGIN;
    currentPlayAreaRadius = baseRadius;  // Começa com tamanho normal
    targetPlayAreaRadius = baseRadius;   // Sem alvo de mudança inicialmente
    
    // Não iniciar com a área diminuindo até atingir 1000 pontos
    isAreaShrinking = false;
    areaChangeTimer = 0.0f;
    
    // Atualizar as variáveis de área jogável após definir o raio inicial
    UpdateScreenSizeVars();
}

// Função de atualização da área dinâmica - modificada para verificar pontuação
void UpdateDynamicPlayArea(float deltaTime, float gameScore) {
    // Atualizar o timer apenas se já atingiu 1000 pontos
    if (gameScore >= 1000) {
        areaChangeTimer += deltaTime;
    }
    
    // Transição suave para o raio alvo (sempre ativa para garantir transições suaves)
    if (currentPlayAreaRadius != targetPlayAreaRadius) {
        float direction = (targetPlayAreaRadius > currentPlayAreaRadius) ? 1.0f : -1.0f;
        float step = deltaTime * areaTransitionSpeed * 120.0f;
        
        currentPlayAreaRadius += direction * step;
        
        // Verificar se chegamos ao alvo
        if ((direction > 0 && currentPlayAreaRadius >= targetPlayAreaRadius) ||
            (direction < 0 && currentPlayAreaRadius <= targetPlayAreaRadius)) {
            currentPlayAreaRadius = targetPlayAreaRadius;
        }
    }
    
    // Mudança de tamanho periódica APENAS se pontuação >= 1000
    if (gameScore >= 1000) {
        // A cada 10 segundos
        if (areaChangeTimer >= 10.0f) {
            areaChangeTimer = 0.0f;
            isAreaShrinking = !isAreaShrinking;  // Alternar entre expandir e contrair
            
            // Definir novo raio alvo
            float baseRadius = fminf(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f - PLAY_AREA_MARGIN;
            
            // Tamanhos mínimo e máximo
            float minRadius = baseRadius * 0.7f;  // 70% do raio original
            float maxRadius = baseRadius * 1.2f;  // 120% do raio original
            
            if (isAreaShrinking) {
                targetPlayAreaRadius = minRadius;
            } else {
                targetPlayAreaRadius = maxRadius;
            }
        }
    }
    
    // Atualizar as variáveis de área jogável
    UpdateScreenSizeVars();
}

