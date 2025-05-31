#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"
#include "utils.h" 

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    int damage;
    bool canRicochet;    
    int ricochetsLeft;   
    struct Bullet *next;
} Bullet;

void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction, bool isPlayerBullet);
void AddBulletWithProps(Bullet **head, Vector2 startPosition, Vector2 direction, float radius, int damage);
void AddRicochetBullet(Bullet **head, Vector2 startPosition, Vector2 direction); 
void UpdateBullets(Bullet **head, float deltaTime, int screenWidth, int screenHeight);
void DestroyBullets(Bullet **head);

// Adicione estas declarações junto com as outras funções de balas
void AddPenetratingBullet(Bullet **head, Vector2 startPosition, Vector2 direction);
void AddHomingBullet(Bullet **head, Vector2 startPosition, Vector2 direction);

#endif