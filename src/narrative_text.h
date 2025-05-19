#ifndef NARRATIVE_TEXT_H
#define NARRATIVE_TEXT_H

#include "raylib.h"

// Estrutura para armazenar texto temporário na tela
typedef struct {
    char text[100];
    float timer;
    float duration;
    Vector2 position;
    float fontSize;
    Color color;
    bool active;
    bool fadeOut;
} ScreenText;

// Inicializar o sistema de texto narrativo
void InitNarrativeText(void);

// Obter texto de introdução do jogo
const char* GetIntroText(void);

// Obter texto de evento de kill milestone (a cada 10 kills)
const char* GetKillMilestoneText(void);

// Obter onomatopeia para dano
const char* GetDamageText(void);

// Obter textos relacionados ao boss
const char* GetBossAppearText(void);
const char* GetBossPhaseText(void);
const char* GetBossDefeatText(void);

// Obter texto de piada aleatória
const char* GetRandomJokeText(void);

// Obter texto de sabedoria cósmica
const char* GetCosmicWisdomText(void);

// Mostrar texto temporário na tela
void ShowScreenText(const char* text, Vector2 position, float fontSize, Color color, float duration, bool fadeOut);

// Atualizar textos na tela (para fade-out)
void UpdateScreenTexts(float deltaTime);

// Desenhar textos ativos na tela
void DrawScreenTexts(void);

#endif // NARRATIVE_TEXT_H