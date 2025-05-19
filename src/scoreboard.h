#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "raylib.h"

#define MAX_SCORES 10        
#define MAX_NAME_LENGTH 50   


typedef enum {
    SORT_BY_SCORE,   
    SORT_BY_KILLS,   
    SORT_BY_TIME     
} SortType;

typedef struct {
    char name[MAX_NAME_LENGTH];  
    long score;                  
    int kills;                   
    float gameTime;              
    bool isNew;                  
} ScoreEntry;


void InitScoreboard(void);
void AddScore(const char *name, long score, int kills, float gameTime);
void SaveScoreboard(void);
void LoadScoreboard(void);
void SortScoreboard(SortType sortType);
void DrawScoreboard(SortType sortType);
void DrawGameSummary(long score, int kills, float gameTime);
bool IsHighScore(long score);
bool IsHighKills(int kills);
bool IsFastTime(float gameTime);
void ResetNewFlags(void);


int GetScoreCount(void);
ScoreEntry GetScoreAt(int index);
const char* FormatTime(float seconds); 

#endif 