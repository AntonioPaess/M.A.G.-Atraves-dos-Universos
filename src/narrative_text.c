#include "narrative_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SCREEN_TEXTS 10
#define SCRIPT_PATH "./run_gemini.sh"
#define MAX_CACHED_PHRASES 5
#define MAX_PHRASE_LENGTH 100


static char responseBuffer[100];


static ScreenText screenTexts[MAX_SCREEN_TEXTS];


typedef struct {
    char phrases[MAX_CACHED_PHRASES][MAX_PHRASE_LENGTH];
    int count;
    int currentIndex;
} PhraseCache;


static PhraseCache caches[10] = {0}; 


static const char* defaultPhrases[] = {
    "HEXAKRON: Sua imperfeição me ofende.", 
    "GRONKARR: Luz... geometria... meu poder... esvaído?", 
    "COSMOS: Uau! Matou 10 hein... quer um presente otaro?", 
    "GRONKARR: EITA LAPADA DO KRAI OBJETO GEOMETRICO!", 
    "NARRADOR: Um objeto geometrico poderoso se aproxima!", 
    "HEXAKRON: Este não é nem meu verdadeiro poder!", 
    "HEXAKRON: Impossível... Como fui derrotado?!", 
    "COSMOS: Por que o círculo é o melhor DJ? Porque sabe fazer os melhores loops!", 
    "GRONKARR: Antes eu nadava livre... agora sou só um ponto no vazio.", 
    "UNIVERSO: Na matemática do caos, até o erro tem seu padrão." 
};


static int getTypeIndex(const char* type);
static void loadCachedPhrases(void);
static const char* getPhrase(const char* tipo);
static void requestPhraseInBackground(const char* tipo);


void InitNarrativeText(void) {
    
    for (int i = 0; i < MAX_SCREEN_TEXTS; i++) {
        screenTexts[i].active = false;
    }
    
    
    srand(time(NULL));
    
    
    for (int i = 0; i < 10; i++) {
        caches[i].count = 1;
        caches[i].currentIndex = 0;
        strcpy(caches[i].phrases[0], defaultPhrases[i]);
    }
    
    
    loadCachedPhrases();
    
    
    FILE* loadingProcess = popen("./preload_phrases.sh &", "r");
    if (loadingProcess) {
        pclose(loadingProcess);
    }
}


int getTypeIndex(const char* type) {
    if (strcmp(type, "boss") == 0) return 0;
    if (strcmp(type, "intro") == 0) return 1;
    if (strcmp(type, "kill_milestone") == 0) return 2;
    if (strcmp(type, "damage") == 0) return 3;
    if (strcmp(type, "boss_appear") == 0) return 4;
    if (strcmp(type, "boss_phase") == 0) return 5;
    if (strcmp(type, "boss_defeat") == 0) return 6;
    if (strcmp(type, "random_joke") == 0) return 7;
    if (strcmp(type, "gronkarr_lament") == 0) return 8;
    if (strcmp(type, "cosmic_wisdom") == 0) return 9;
    return 0; 
}


void loadCachedPhrases(void) {
    FILE* cacheFile = fopen("phrases_cache.txt", "r");
    if (!cacheFile) return;
    
    char line[150];
    char type[20];
    char phrase[MAX_PHRASE_LENGTH];
    
    while (fgets(line, sizeof(line), cacheFile)) {
        
        if (sscanf(line, "%19[^:]:%99[^\n]", type, phrase) == 2) {
            int idx = getTypeIndex(type);
            if (caches[idx].count < MAX_CACHED_PHRASES) {
                strcpy(caches[idx].phrases[caches[idx].count], phrase);
                caches[idx].count++;
            }
        }
    }
    
    fclose(cacheFile);
}


void requestPhraseInBackground(const char* tipo) {
    
    int idx = getTypeIndex(tipo);
    if (caches[idx].count >= 3) return; 
    
    char command[300];
    
    snprintf(command, sizeof(command), 
             "nohup %s %s >> phrases_cache.txt 2>/dev/null &", 
             SCRIPT_PATH, tipo);
    
    system(command);
}


const char* getPhrase(const char* tipo) {
    int idx = getTypeIndex(tipo);
    
    
    if (caches[idx].count > 0) {
        
        caches[idx].currentIndex = (caches[idx].currentIndex + 1) % caches[idx].count;
        
        
        requestPhraseInBackground(tipo);
        
        
        return caches[idx].phrases[caches[idx].currentIndex];
    }
    
    
    return defaultPhrases[idx];
}


const char* GetIntroText(void) {
    return getPhrase("intro");
}

const char* GetKillMilestoneText(void) {
    return getPhrase("kill_milestone");
}

const char* GetDamageText(void) {
    return getPhrase("damage");
}

const char* GetBossAppearText(void) {
    return getPhrase("boss_appear");
}

const char* GetBossPhaseText(void) {
    return getPhrase("boss_phase");
}

const char* GetBossDefeatText(void) {
    return getPhrase("boss_defeat");
}

const char* GetRandomJokeText(void) {
    return getPhrase("random_joke");
}

const char* GetGronkarrLamentText(void) {
    return getPhrase("gronkarr_lament");
}

const char* GetCosmicWisdomText(void) {
    return getPhrase("cosmic_wisdom");
}


void ShowScreenText(const char* text, Vector2 position, float fontSize, Color color, float duration, bool fadeOut) {
    
    for (int i = 0; i < MAX_SCREEN_TEXTS; i++) {
        if (!screenTexts[i].active) {
            strncpy(screenTexts[i].text, text, sizeof(screenTexts[i].text) - 1);
            screenTexts[i].position = position;
            screenTexts[i].fontSize = fontSize;
            screenTexts[i].color = color;
            screenTexts[i].timer = 0.0f;
            screenTexts[i].duration = duration;
            screenTexts[i].active = true;
            screenTexts[i].fadeOut = fadeOut;
            return;
        }
    }
}


void UpdateScreenTexts(float deltaTime) {
    for (int i = 0; i < MAX_SCREEN_TEXTS; i++) {
        if (screenTexts[i].active) {
            screenTexts[i].timer += deltaTime;
            
            if (screenTexts[i].timer >= screenTexts[i].duration) {
                screenTexts[i].active = false;
            }
        }
    }
}


void DrawScreenTexts(void) {
    for (int i = 0; i < MAX_SCREEN_TEXTS; i++) {
        if (screenTexts[i].active) {
            Color textColor = screenTexts[i].color;
            
            
            if (screenTexts[i].fadeOut) {
                float alpha = 1.0f - (screenTexts[i].timer / screenTexts[i].duration);
                textColor.a = (unsigned char)(255 * alpha);
            }
            
            
            float textWidth = MeasureText(screenTexts[i].text, screenTexts[i].fontSize);
            float textX = screenTexts[i].position.x - textWidth/2;
            
            
            DrawText(screenTexts[i].text, 
                    textX + 2, 
                    screenTexts[i].position.y + 2, 
                    screenTexts[i].fontSize, 
                    Fade(BLACK, textColor.a / 255.0f));
                    
            DrawText(screenTexts[i].text, 
                    textX, 
                    screenTexts[i].position.y, 
                    screenTexts[i].fontSize, 
                    textColor);
        }
    }
}