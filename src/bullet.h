#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"
#include "utils.h" // Para BULLET_RADIUS, BULLET_SPEED
#include <stdbool.h>

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    Color color;
    struct Bullet *next;
} Bullet;

void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction);
void UpdateBullets(Bullet **head, float deltaTime, int screenWidth, int screenHeight);
void DrawBullets(const Bullet *head); // Movido para render.c, mas declaração pode ficar aqui ou em render.h
void FreeBullets(Bullet **head);

#endif // BULLET_H