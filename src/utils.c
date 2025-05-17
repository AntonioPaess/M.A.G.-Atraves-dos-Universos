#include "utils.h"
// raylib.h é incluído através de utils.h
#include <math.h> // Para sqrtf se necessário em outras funções, mas não para AppToggleFullscreen

// Definição da variável global
bool isFullscreenGlobal = false;

void AppToggleFullscreen(void) {
    // Chama a função ToggleFullscreen() da biblioteca Raylib
    ToggleFullscreen(); // Esta é a chamada para a função da Raylib

    // Atualiza nosso rastreador de estado global
    isFullscreenGlobal = !isFullscreenGlobal;

    // Se saímos da tela cheia (ou seja, isFullscreenGlobal é agora false),
    // e a janela não está minimizada, restauramos o tamanho da janela.
    // Se estiver minimizada, a Raylib pode lidar com a restauração de forma diferente.
    if (!IsWindowFullscreen() && !IsWindowMinimized()) {
        // Apenas redefine se não estivermos mais em tela cheia
        // A Raylib pode já ter restaurado para o tamanho anterior ao sair do fullscreen.
        // Se quisermos forçar nosso tamanho definido:
        // SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    // A lógica de SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT) ao sair do fullscreen
    // pode ser desnecessária se a Raylib já restaurar para o tamanho original da janela.
    // Testar o comportamento é importante. Se a janela ficar pequena ao sair do fullscreen,
    // descomente e ajuste o SetWindowSize.
}

// Funções de vetores (Raylib já tem muitas, mas se precisar de alguma específica)
// float Vector2Distance(Vector2 v1, Vector2 v2) {
//     return Vector2Distance(v1, v2); // Usar da Raylib
// }

// Vector2 Vector2Normalize(Vector2 v) {
//     return Vector2Normalize(v); // Usar da Raylib
// }