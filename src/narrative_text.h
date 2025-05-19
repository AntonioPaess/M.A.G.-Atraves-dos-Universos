#ifndef NARRATIVE_TEXT_H
#define NARRATIVE_TEXT_H

#include "raylib.h"


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


void InitNarrativeText(void);


const char* GetIntroText(void);


const char* GetKillMilestoneText(void);


const char* GetDamageText(void);


const char* GetBossAppearText(void);
const char* GetBossPhaseText(void);
const char* GetBossDefeatText(void);


const char* GetRandomJokeText(void);


const char* GetCosmicWisdomText(void);


void ShowScreenText(const char* text, Vector2 position, float fontSize, Color color, float duration, bool fadeOut);


void UpdateScreenTexts(float deltaTime);


void DrawScreenTexts(void);

#endif 