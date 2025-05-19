#include "narrative_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SCREEN_TEXTS 10
#define SCRIPT_PATH "./run_gemini.sh"
#define MAX_CACHED_PHRASES 5
#define MAX_PHRASE_LENGTH 100

// Buffer para armazenar respostas da API
static char responseBuffer[100];

// Array de textos na tela
static ScreenText screenTexts[MAX_SCREEN_TEXTS];

// Cache para cada tipo de texto
typedef struct {
    char phrases[MAX_CACHED_PHRASES][MAX_PHRASE_LENGTH];
    int count;
    int currentIndex;
} PhraseCache;

// Cache para cada tipo de texto narrativo
static PhraseCache caches[10] = {0}; // Agora são 10 tipos de frases

// Frases padrão para fallback imediato
static const char* defaultPhrases[] = {
    "HEXAKRON: Sua fim está próximo!", // boss
    "GRONKARR: Onde estou? Esta dimensão é estranha...", // intro
    "COSMOS: Uau! Mais 10 para a conta!", // kill_milestone
    "GRONKARR: ARGH!", // damage
    "NARRADOR: Um inimigo poderoso se aproxima!", // boss_appear
    "HEXAKRON: Este não é nem meu verdadeiro poder!", // boss_phase
    "HEXAKRON: Impossível... Como fui derrotado?!", // boss_defeat
    "COSMOS: Por que o círculo é o melhor DJ? Porque sabe fazer os melhores loops!", // random_joke
    "GRONKARR: Antes eu nadava livre... agora sou só um ponto no vazio.", // gronkarr_lament
    "UNIVERSO: Na matemática do caos, até o erro tem seu padrão." // cosmic_wisdom
};

// Protótipos de funções internas
static int getTypeIndex(const char* type);
static void loadCachedPhrases(void);
static const char* getPhrase(const char* tipo);
static void requestPhraseInBackground(const char* tipo);

// Inicializar o sistema de texto e pre-carregar algumas frases
void InitNarrativeText(void) {
    // Inicializar array de textos na tela
    for (int i = 0; i < MAX_SCREEN_TEXTS; i++) {
        screenTexts[i].active = false;
    }
    
    // Inicializar a semente para randomização
    srand(time(NULL));
    
    // Inicializar cada cache com pelo menos a frase padrão
    for (int i = 0; i < 10; i++) {
        caches[i].count = 1;
        caches[i].currentIndex = 0;
        strcpy(caches[i].phrases[0], defaultPhrases[i]);
    }
    
    // Carregar frases do cache durante a inicialização
    loadCachedPhrases();
    
    // Pré-carregar frases de forma não bloqueante
    FILE* loadingProcess = popen("./preload_phrases.sh &", "r");
    if (loadingProcess) {
        pclose(loadingProcess);
    }
}

// Mapeia o tipo para um índice de cache
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
    return 0; // Fallback para boss
}

// Tenta carregar frases do arquivo de cache
void loadCachedPhrases(void) {
    FILE* cacheFile = fopen("phrases_cache.txt", "r");
    if (!cacheFile) return;
    
    char line[150];
    char type[20];
    char phrase[MAX_PHRASE_LENGTH];
    
    while (fgets(line, sizeof(line), cacheFile)) {
        // Formato esperado: tipo:frase
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

// Chamada de API não bloqueante (em segundo plano)
void requestPhraseInBackground(const char* tipo) {
    // Verificar se já temos muitas frases (para limitar requisições desnecessárias)
    int idx = getTypeIndex(tipo);
    if (caches[idx].count >= 3) return; // Já temos frases suficientes deste tipo
    
    char command[300];
    // Use nohup para garantir que o processo continue mesmo que o terminal seja fechado
    snprintf(command, sizeof(command), 
             "nohup %s %s >> phrases_cache.txt 2>/dev/null &", 
             SCRIPT_PATH, tipo);
    
    system(command);
}

// Obter uma frase do cache e solicitar uma nova em background
const char* getPhrase(const char* tipo) {
    int idx = getTypeIndex(tipo);
    
    // Se temos frases no cache, use-as em sequência
    if (caches[idx].count > 0) {
        // Seleção circular no cache
        caches[idx].currentIndex = (caches[idx].currentIndex + 1) % caches[idx].count;
        
        // Solicitar uma nova frase em background para manter o cache atualizado
        requestPhraseInBackground(tipo);
        
        // Retornar uma frase já armazenada no cache
        return caches[idx].phrases[caches[idx].currentIndex];
    }
    
    // Fallback para a frase padrão
    return defaultPhrases[idx];
}

// Implementações das funções de obtenção de texto (agora não bloqueantes)
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

// Mostrar texto temporário na tela
void ShowScreenText(const char* text, Vector2 position, float fontSize, Color color, float duration, bool fadeOut) {
    // Encontrar um slot livre
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

// Atualizar textos na tela
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

// Desenhar textos ativos na tela
void DrawScreenTexts(void) {
    for (int i = 0; i < MAX_SCREEN_TEXTS; i++) {
        if (screenTexts[i].active) {
            Color textColor = screenTexts[i].color;
            
            // Aplicar fade out se necessário
            if (screenTexts[i].fadeOut) {
                float alpha = 1.0f - (screenTexts[i].timer / screenTexts[i].duration);
                textColor.a = (unsigned char)(255 * alpha);
            }
            
            // Centralizar texto horizontalmente
            float textWidth = MeasureText(screenTexts[i].text, screenTexts[i].fontSize);
            float textX = screenTexts[i].position.x - textWidth/2;
            
            // Desenhar texto com sombra para melhor visibilidade
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