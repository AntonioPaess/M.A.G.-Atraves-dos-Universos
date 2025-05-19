#include "scoreboard.h"
#include <stdio.h>
#include <string.h>


int GetScorePosition(long score);


static ScoreEntry scores[MAX_SCORES];
static int scoreCount = 0;


void InitScoreboard(void) {
    LoadScoreboard();
}


void AddScore(const char *name, long score, int kills, float gameTime) {
    if (scoreCount >= MAX_SCORES && score <= scores[scoreCount-1].score) {
        
        return;
    }
    
    
    int position = GetScorePosition(score);
    
    
    if (scoreCount >= MAX_SCORES) {
        scoreCount--;
    }
    
    
    for (int i = scoreCount; i > position; i--) {
        scores[i] = scores[i-1];
    }
    
    
    strncpy(scores[position].name, name, MAX_NAME_LENGTH-1);
    scores[position].name[MAX_NAME_LENGTH-1] = '\0'; 
    scores[position].score = score;
    scores[position].kills = kills;         
    scores[position].gameTime = gameTime;   
    scores[position].isNew = true;
    
    
    if (scoreCount < MAX_SCORES) {
        scoreCount++;
    }
    
    
    SortScoreboard(SORT_BY_SCORE);  
    SaveScoreboard();
}


int Partition(ScoreEntry arr[], int low, int high) {
    long pivot = arr[high].score;
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        
        if (arr[j].score > pivot) {
            i++;
            
            ScoreEntry temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    
    ScoreEntry temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return (i + 1);
}


void QuickSort(ScoreEntry arr[], int low, int high) {
    if (low < high) {
        
        int pi = Partition(arr, low, high);
        
        
        QuickSort(arr, low, pi - 1);
        QuickSort(arr, pi + 1, high);
    }
}


void SortScoreboard(SortType sortType) {
    if (scoreCount <= 1) {
        return;  
    }
    
    
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


void SaveScoreboard(void) {
    FILE *file = fopen("scores.dat", "wb");
    if (file == NULL) {
        return; 
    }
    
    
    fwrite(&scoreCount, sizeof(int), 1, file);
    
    
    for (int i = 0; i < scoreCount; i++) {
        
        fwrite(scores[i].name, sizeof(char), MAX_NAME_LENGTH, file);
        fwrite(&scores[i].score, sizeof(long), 1, file);
        fwrite(&scores[i].kills, sizeof(int), 1, file);
        fwrite(&scores[i].gameTime, sizeof(float), 1, file);
    }
    
    fclose(file);
}


void LoadScoreboard(void) {
    FILE *file = fopen("scores.dat", "rb");
    if (file == NULL) {
        scoreCount = 0;
        return; 
    }
    
    
    fread(&scoreCount, sizeof(int), 1, file);
    
    
    if (scoreCount > MAX_SCORES) {
        scoreCount = MAX_SCORES;
    }
    
    
    for (int i = 0; i < scoreCount; i++) {
        fread(scores[i].name, sizeof(char), MAX_NAME_LENGTH, file);
        fread(&scores[i].score, sizeof(long), 1, file);
        fread(&scores[i].kills, sizeof(int), 1, file);
        fread(&scores[i].gameTime, sizeof(float), 1, file);
        scores[i].isNew = false; 
    }
    
    fclose(file);
}


bool IsHighScore(long score) {
    if (scoreCount < MAX_SCORES) {
        return true; 
    }
    
    
    return score > scores[scoreCount-1].score;
}


int GetScorePosition(long score) {
    for (int i = 0; i < scoreCount; i++) {
        if (score > scores[i].score) {
            return i;
        }
    }
    return scoreCount;
}


void DrawScoreboard(SortType sortType) {
    
    SortScoreboard(sortType);
    
    const int titleFontSize = 50;
    const int headerFontSize = 30;
    const int scoreFontSize = 25;
    const int lineHeight = scoreFontSize + 10;
    const int startY = 200;
    const Color textColor = WHITE;
    
    
    const char *title = "PONTUAÇÕES MAIS ALTAS";
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, 100, titleFontSize, textColor);
    
    
    DrawText("POS", GetScreenWidth()/2 - 250, startY, headerFontSize, textColor);
    DrawText("NOME", GetScreenWidth()/2 - 150, startY, headerFontSize, textColor);
    DrawText("PONTUAÇÃO", GetScreenWidth()/2 + 100, startY, headerFontSize, textColor);
    
    
    for (int i = 0; i < scoreCount; i++) {
        int y = startY + (i+1) * lineHeight;
        
        
        char posText[10];
        sprintf(posText, "%d", i+1);
        DrawText(posText, GetScreenWidth()/2 - 250, y, scoreFontSize, textColor);
        
        
        Color nameColor = scores[i].isNew ? GOLD : textColor;
        DrawText(scores[i].name, GetScreenWidth()/2 - 150, y, scoreFontSize, nameColor);
        
        
        char scoreText[20];
        sprintf(scoreText, "%ld", scores[i].score);
        DrawText(scoreText, GetScreenWidth()/2 + 100, y, scoreFontSize, textColor);
    }
    
    
    const char *backText = "PRESSIONE ESPAÇO PARA VOLTAR";
    int backWidth = MeasureText(backText, 20);
    DrawText(backText, GetScreenWidth()/2 - backWidth/2, GetScreenHeight() - 100, 20, Fade(textColor, 0.7f));
}


void ResetNewFlags(void) {
    for (int i = 0; i < scoreCount; i++) {
        scores[i].isNew = false;
    }
}


int GetScoreCount(void) {
    return scoreCount;
}


ScoreEntry GetScoreAt(int index) {
    if (index >= 0 && index < scoreCount) {
        return scores[index];
    }
    
    
    ScoreEntry empty = {"", 0, false};
    return empty;
}


const char* FormatTime(float seconds) {
    static char timeStr[16]; 
    
    int mins = (int)seconds / 60;
    int secs = (int)seconds % 60;
    
    
    sprintf(timeStr, "%02d:%02d", mins, secs);
    
    return timeStr;
}