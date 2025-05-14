#include "utils.h"
// raylib.h é incluído através de utils.h, que deve vir primeiro se utils.h o inclui.
// Se utils.h não inclui raylib.h, então #include "raylib.h" deve vir aqui.
// Assumindo que utils.h já inclui raylib.h como mostrado na sugestão para utils.h.
#include <math.h> 

// Definição da variável global
bool isFullscreenGlobal = false;

// Esta é a função que o jogo chama (de main.c)
void AppToggleFullscreen(void) {
    // Chama a função ToggleFullscreen() da biblioteca Raylib
    ToggleFullscreen(); // Esta é a chamada para a função da Raylib

    // Atualiza nosso rastreador de estado global
    isFullscreenGlobal = !isFullscreenGlobal;

    // Se saímos da tela cheia (ou seja, isFullscreenGlobal é agora false),
    // restauramos o tamanho da janela para as dimensões definidas.
    if (!isFullscreenGlobal) {
        SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
}

// Exemplo de implementação (se fosse necessário, mas Raylib já tem):
/*
float Vector2Distance(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return sqrtf(dx*dx + dy*dy);
}

Vector2 Vector2Normalize(Vector2 v) {
    float length = sqrtf(v.x*v.x + v.y*v.y);
    if (length != 0) {
        return (Vector2){ v.x/length, v.y/length };
    }
    return (Vector2){0,0};
}
*/

// Funções para carregar assets (exemplo)
/*
Texture2D LoadSprite(const char *fileName) {
    // Idealmente, os assets estariam em uma pasta "assets/"
    // char path[256];
    // sprintf(path, "assets/sprites/%s", fileName);
    // return LoadTexture(path);
    return LoadTexture(fileName); // Simples por agora
}

Sound LoadSoundEffect(const char *fileName) {
    // char path[256];
    // sprintf(path, "assets/audio/%s", fileName);
    // return LoadSound(path);
    return LoadSound(fileName);
}

Music LoadGameMusic(const char *fileName) {
    // char path[256];
    // sprintf(path, "assets/audio/%s", fileName);
    // return LoadMusicStream(path);
    return LoadMusicStream(fileName);
}

void UnloadSprite(Texture2D texture) {
    UnloadTexture(texture);
}

void UnloadSoundEffect(Sound sound) {
    UnloadSound(sound);
}

void UnloadGameMusic(Music music) {
    UnloadMusicStream(music);
}
*/ // Adicionado para fechar o comentário

