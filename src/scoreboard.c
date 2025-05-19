#include "scoreboard.h"
#include <stdio.h>
#include <string.h>

// Função interna para encontrar posição de inserção
int GetScorePosition(long score);

// Array de pontuações
static ScoreEntry scores[MAX_SCORES];
static int scoreCount = 0;

// Inicializa o scoreboard
void InitScoreboard(void) {
    LoadScoreboard();
}

// Adiciona uma nova pontuação ao scoreboard
void AddScore(const char *name, long score, int kills, float gameTime) {
    if (scoreCount >= MAX_SCORES && score <= scores[scoreCount-1].score) {
        // Se o placar está cheio e a nova pontuação é menor que a menor pontuação atual
        return;
    }
    
    // Encontrar a posição para inserir
    int position = GetScorePosition(score);
    
    // Se o placar está cheio, removemos a última entrada
    if (scoreCount >= MAX_SCORES) {
        scoreCount--;
    }
    
    // Deslocar os registros para abrir espaço para a nova pontuação
    for (int i = scoreCount; i > position; i--) {
        scores[i] = scores[i-1];
    }
    
    // Inserir a nova pontuação
    strncpy(scores[position].name, name, MAX_NAME_LENGTH-1);
    scores[position].name[MAX_NAME_LENGTH-1] = '\0'; // Garantir que a string termine
    scores[position].score = score;
    scores[position].kills = kills;         // Novo campo
    scores[position].gameTime = gameTime;   // Novo campo
    scores[position].isNew = true;
    
    // Incrementar o contador se ainda não atingimos o máximo
    if (scoreCount < MAX_SCORES) {
        scoreCount++;
    }
    
    // Ordenar e salvar
    SortScoreboard(SORT_BY_SCORE);  // Ordenar por pontuação por padrão
    SaveScoreboard();
}

// Função de partição para o Quicksort
int Partition(ScoreEntry arr[], int low, int high) {
    long pivot = arr[high].score;
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        // Se o elemento atual é maior que o pivô (ordenamos em ordem decrescente)
        if (arr[j].score > pivot) {
            i++;
            // Trocar arr[i] e arr[j]
            ScoreEntry temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    // Trocar arr[i+1] e arr[high] (o pivô)
    ScoreEntry temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return (i + 1);
}

// Função Quicksort recursiva
void QuickSort(ScoreEntry arr[], int low, int high) {
    if (low < high) {
        // Encontrar o índice de partição
        int pi = Partition(arr, low, high);
        
        // Ordenar elementos antes e depois da partição
        QuickSort(arr, low, pi - 1);
        QuickSort(arr, pi + 1, high);
    }
}

// Ordenar o scoreboard usando Quicksort
void SortScoreboard(SortType sortType) {
    if (scoreCount <= 1) {
        return;  // Nada para ordenar
    }
    
    // Ordenação baseada no tipo especificado
    for (int i = 0; i < scoreCount - 1; i++) {
        for (int j = 0; j < scoreCount - i - 1; j++) {
            bool shouldSwap = false;
            
            switch (sortType) {
                case SORT_BY_SCORE:
                    shouldSwap = scores[j].score < scores[j + 1].score;
                    break;
                case SORT_BY_KILLS:
                    shouldSwap = scores[j].kills < scores[j + 1].kills;
                    break;
                case SORT_BY_TIME:
                    // Para tempo, menor é melhor (ordem crescente)
                    shouldSwap = scores[j].gameTime > scores[j + 1].gameTime;
                    break;
            }
            
            if (shouldSwap) {
                ScoreEntry temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}

// Salvar o scoreboard em um arquivo
void SaveScoreboard(void) {
    FILE *file = fopen("scores.dat", "wb");
    if (file == NULL) {
        return; // Não foi possível abrir o arquivo
    }
    
    // Escrever o número de pontuações
    fwrite(&scoreCount, sizeof(int), 1, file);
    
    // Escrever as pontuações
    for (int i = 0; i < scoreCount; i++) {
        // Não salvamos o campo isNew
        fwrite(scores[i].name, sizeof(char), MAX_NAME_LENGTH, file);
        fwrite(&scores[i].score, sizeof(long), 1, file);
        fwrite(&scores[i].kills, sizeof(int), 1, file);
        fwrite(&scores[i].gameTime, sizeof(float), 1, file);
    }
    
    fclose(file);
}

// Carregar o scoreboard de um arquivo
void LoadScoreboard(void) {
    FILE *file = fopen("scores.dat", "rb");
    if (file == NULL) {
        scoreCount = 0;
        return; // Arquivo não existe ou não pode ser aberto
    }
    
    // Ler o número de pontuações
    fread(&scoreCount, sizeof(int), 1, file);
    
    // Verificar se o número está dentro dos limites
    if (scoreCount > MAX_SCORES) {
        scoreCount = MAX_SCORES;
    }
    
    // Ler as pontuações
    for (int i = 0; i < scoreCount; i++) {
        fread(scores[i].name, sizeof(char), MAX_NAME_LENGTH, file);
        fread(&scores[i].score, sizeof(long), 1, file);
        fread(&scores[i].kills, sizeof(int), 1, file);
        fread(&scores[i].gameTime, sizeof(float), 1, file);
        scores[i].isNew = false; // Inicializar como não nova
    }
    
    fclose(file);
}

// Verifica se uma pontuação é suficiente para entrar no placar
bool IsHighScore(long score) {
    if (scoreCount < MAX_SCORES) {
        return true; // Ainda há espaço no placar
    }
    
    // Verificar se é maior que a menor pontuação no placar
    return score > scores[scoreCount-1].score;
}

// Encontra a posição onde uma nova pontuação deve ser inserida
int GetScorePosition(long score) {
    for (int i = 0; i < scoreCount; i++) {
        if (score > scores[i].score) {
            return i;
        }
    }
    return scoreCount;
}

// Desenha o placar de pontuações
void DrawScoreboard(SortType sortType) {
    // Ordenar o scoreboard pelo critério solicitado
    SortScoreboard(sortType);
    
    const int titleFontSize = 50;
    const int headerFontSize = 30;
    const int scoreFontSize = 25;
    const int lineHeight = scoreFontSize + 10;
    const int startY = 200;
    const Color textColor = WHITE;
    
    // Título
    const char *title = "PONTUAÇÕES MAIS ALTAS";
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, 100, titleFontSize, textColor);
    
    // Cabeçalhos
    DrawText("POS", GetScreenWidth()/2 - 250, startY, headerFontSize, textColor);
    DrawText("NOME", GetScreenWidth()/2 - 150, startY, headerFontSize, textColor);
    DrawText("PONTUAÇÃO", GetScreenWidth()/2 + 100, startY, headerFontSize, textColor);
    
    // Linhas dos placares
    for (int i = 0; i < scoreCount; i++) {
        int y = startY + (i+1) * lineHeight;
        
        // Posição
        char posText[10];
        sprintf(posText, "%d", i+1);
        DrawText(posText, GetScreenWidth()/2 - 250, y, scoreFontSize, textColor);
        
        // Nome do jogador (destacado se for novo)
        Color nameColor = scores[i].isNew ? GOLD : textColor;
        DrawText(scores[i].name, GetScreenWidth()/2 - 150, y, scoreFontSize, nameColor);
        
        // Pontuação
        char scoreText[20];
        sprintf(scoreText, "%ld", scores[i].score);
        DrawText(scoreText, GetScreenWidth()/2 + 100, y, scoreFontSize, textColor);
    }
    
    // Instrução para voltar
    const char *backText = "PRESSIONE ESPAÇO PARA VOLTAR";
    int backWidth = MeasureText(backText, 20);
    DrawText(backText, GetScreenWidth()/2 - backWidth/2, GetScreenHeight() - 100, 20, Fade(textColor, 0.7f));
}

// Reseta todas as flags isNew
void ResetNewFlags(void) {
    for (int i = 0; i < scoreCount; i++) {
        scores[i].isNew = false;
    }
}

// Retorna o número de pontuações no placar
int GetScoreCount(void) {
    return scoreCount;
}

// Retorna a entrada do placar no índice especificado
ScoreEntry GetScoreAt(int index) {
    if (index >= 0 && index < scoreCount) {
        return scores[index];
    }
    
    // Retorna uma entrada vazia se o índice estiver fora dos limites
    ScoreEntry empty = {"", 0, false};
    return empty;
}

// Função para formatar o tempo (segundos) em uma string legível "MM:SS"
const char* FormatTime(float seconds) {
    static char timeStr[16]; // Buffer estático para evitar problemas de memória
    
    int mins = (int)seconds / 60;
    int secs = (int)seconds % 60;
    
    // Formatar como "MM:SS"
    sprintf(timeStr, "%02d:%02d", mins, secs);
    
    return timeStr;
}