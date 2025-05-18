#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"
#include "utils.h" // Adicione este include

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    int damage;
    bool canRicochet;    // Novo: indica se a bala pode ricocheter
    int ricochetsLeft;   // Novo: número de ricochetes restantes
    struct Bullet *next;
} Bullet;

void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction);
void AddBulletWithProps(Bullet **head, Vector2 startPosition, Vector2 direction, float radius, int damage);
void AddRicochetBullet(Bullet **head, Vector2 startPosition, Vector2 direction); // Nova função
void UpdateBullets(Bullet **head, float deltaTime, int screenWidth, int screenHeight);
void DestroyBullets(Bullet **head);

#endif // BULLET_H