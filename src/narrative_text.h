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





const char* GetDamageText(void);









void ShowScreenText(const char* text, Vector2 position, float fontSize, Color color, float duration, bool fadeOut);


void UpdateScreenTexts(float deltaTime);


void DrawScreenTexts(void);

#endif 