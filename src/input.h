#ifndef INPUT_H
#define INPUT_H

#include "raylib.h"

// Estrutura para armazenar o estado do input, se necessário para desacoplar de Raylib.
// Por ora, usaremos diretamente as funções da Raylib.

// Função para processar inputs do jogador (movimento, tiro, etc.)
// Poderia retornar uma estrutura com ações do jogador ou modificar o estado do jogo diretamente.
void ProcessInput(void); // Exemplo, pode precisar de parâmetros dependendo da arquitetura

// Funções para verificar teclas específicas, se quisermos abstrair Raylib
// bool IsActionPressed(GameAction action); // Exemplo de abstração

#endif // INPUT_H
