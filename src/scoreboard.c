#include "scoreboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "raylib.h" // Removido para não depender de Raylib para logging aqui

// Helper function for Quicksort (partition)
static int Partition(ScoreEntry arr[], int low, int high) {
    ScoreEntry pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].score >= pivot.score) { // Sort descending by score
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

// Quicksort implementation
static void QuickSort(ScoreEntry arr[], int low, int high) {
    if (low < high) {
        int pi = Partition(arr, low, high);
        QuickSort(arr, low, pi - 1);
        QuickSort(arr, pi + 1, high);
    }
}

void SortScoreboard(ScoreEntry *entries, int count) {
    if (entries && count > 1) {
        QuickSort(entries, 0, count - 1);
    }
}

int LoadScoreboard(const char *filename, ScoreEntry **entries) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        *entries = NULL; 
        return 0;
    }

    *entries = NULL;
    int count = 0;
    ScoreEntry buffer;
    char line[256]; 

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%49[^,],%d,%d,%f,%d,%d",
                   buffer.name, &buffer.score, &buffer.lives,
                   &buffer.timeAlive, &buffer.enemiesKilled, &buffer.phasesWon) == 6) {
            
            ScoreEntry *temp_entries = realloc(*entries, sizeof(ScoreEntry) * (count + 1));
            if (temp_entries == NULL) {
                fprintf(stderr, "[SCOREBOARD ERROR] Failed to reallocate memory for scoreboard entries.\n");
                if (*entries) free(*entries); 
                *entries = NULL;
                fclose(file);
                return 0; 
            }
            *entries = temp_entries;
            (*entries)[count++] = buffer;
        } else {
            fprintf(stderr, "[SCOREBOARD WARNING] Skipping malformed line in scoreboard file: %s\n", line);
        }
    }

    fclose(file);
    return count;
}

void SaveScoreboard(const char *filename, const ScoreEntry *entries, int count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "[SCOREBOARD ERROR] Failed to open scoreboard file for writing: %s\n", filename);
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%d,%d,%.2f,%d,%d\n",
                entries[i].name,
                entries[i].score,
                entries[i].lives,
                entries[i].timeAlive,
                entries[i].enemiesKilled,
                entries[i].phasesWon);
    }
    fclose(file);
}

void AddScoreEntry(ScoreEntry **entries, int *count, ScoreEntry newEntry) {
    ScoreEntry *temp_entries = realloc(*entries, sizeof(ScoreEntry) * (*count + 1));
    if (temp_entries == NULL) {
        fprintf(stderr, "[SCOREBOARD ERROR] Failed to reallocate memory for adding score entry.\n");
        return;
    }
    *entries = temp_entries;
    (*entries)[*count] = newEntry;
    (*count)++;
}

