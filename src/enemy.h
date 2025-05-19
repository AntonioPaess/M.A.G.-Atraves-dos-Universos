#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "utils.h" 
#include "bullet.h" 
#include <stdbool.h>

#define DEATH_ANIMATION_DURATION 0.8f 


typedef enum {
    ENEMY_TYPE_NORMAL,   
    ENEMY_TYPE_SPEEDER,  
    ENEMY_TYPE_TANK,     
    ENEMY_TYPE_EXPLODER, 
    ENEMY_TYPE_SHOOTER   
} EnemyType;

typedef struct Enemy {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    Color color;
    float speed;
    EnemyType type;      
    int health;          
    float shootTimer;    
    int dodgeCount;      
    struct Enemy *next;  
    struct Enemy *prev;  
    
    bool isDying;        
    float deathTimer;    
} Enemy;

typedef struct {
    Enemy *head;
    int count;
} EnemyList;

void InitEnemyList(EnemyList *list);
void AddEnemy(EnemyList *list, Vector2 position, float radius, float speed, Color color, EnemyType type);
void UpdateEnemies(EnemyList *list, Vector2 playerPosition, float deltaTime, int screenWidth, int screenHeight, Bullet **playerBullets, Bullet **enemyBullets);
void DrawEnemies(const EnemyList *list);
void RemoveEnemy(EnemyList *list, Enemy *toRemove);
void FreeEnemies(EnemyList *list);

#endif 