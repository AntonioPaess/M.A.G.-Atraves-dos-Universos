#include "bullet.h"
#include <stdlib.h> 
#include "raymath.h" 

void AddBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return; 

    newBullet->position = startPosition;
    
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED);
    } else {
        
        newBullet->velocity = (Vector2){0, -BULLET_SPEED}; 
    }
    newBullet->radius = BULLET_RADIUS;
    newBullet->active = true;
    newBullet->next = *head;
    newBullet->damage = 1;  
    *head = newBullet;
}

void UpdateBullets(Bullet **head, float deltaTime, int screenWidth, int screenHeight) {
    Bullet *current = *head;
    Bullet *prev = NULL;

    while (current != NULL) {
        if (current->active) {
            
            current->position.x += current->velocity.x * deltaTime;
            current->position.y += current->velocity.y * deltaTime;

            
            if (current->canRicochet && current->ricochetsLeft > 0) {
                
                extern float currentPlayAreaRadius;
                Vector2 center = {PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y};
                float distToCenter = Vector2Distance(current->position, center);
                
                if (distToCenter >= currentPlayAreaRadius - current->radius) {
                    
                    Vector2 normal = Vector2Normalize(Vector2Subtract(center, current->position));
                    
                    
                    float dotProduct = Vector2DotProduct(current->velocity, normal);
                    Vector2 reflection = Vector2Subtract(
                        current->velocity,
                        Vector2Scale(normal, 2 * dotProduct)
                    );
                    
                    
                    current->velocity = reflection;
                    current->ricochetsLeft--;
                    
                    
                    current->position = Vector2Add(
                        center,
                        Vector2Scale(
                            Vector2Normalize(Vector2Subtract(current->position, center)),
                            currentPlayAreaRadius - current->radius - 2.0f
                        )
                    );
                }
            }
            
            
            if (current->position.x + current->radius < 0 ||
                current->position.x - current->radius > screenWidth ||
                current->position.y + current->radius < 0 ||
                current->position.y - current->radius > screenHeight) {
                current->active = false;
            }
        }

        
        if (!current->active) {
            Bullet *toRemove = current;
            
            if (prev == NULL) { 
                *head = current->next;
                current = *head;
            } else {
                prev->next = current->next;
                current = current->next;
            }
            
            free(toRemove);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void FreeBullets(Bullet **head) {
    Bullet *current = *head;
    while (current != NULL) {
        Bullet *next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}


void AddBulletWithProps(Bullet **bulletsList, Vector2 position, Vector2 direction, float radius, int damage) {
    Bullet *bullet = (Bullet*)malloc(sizeof(Bullet));
    if (bullet) {
        bullet->position = position;
        bullet->velocity = Vector2Scale(direction, BULLET_SPEED);
        bullet->active = true;
        bullet->radius = radius;
        bullet->damage = damage;  
        bullet->next = *bulletsList;
        *bulletsList = bullet;
    }
}


void AddRicochetBullet(Bullet **head, Vector2 startPosition, Vector2 direction) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    if (!newBullet) return;

    newBullet->position = startPosition;
    
    if (Vector2LengthSqr(direction) > 0) {
        newBullet->velocity = Vector2Scale(Vector2Normalize(direction), BULLET_SPEED * 1.5f); 
    } else {
        newBullet->velocity = (Vector2){0, -BULLET_SPEED * 1.5f};
    }
    newBullet->radius = BULLET_RADIUS * 1.2f; 
    newBullet->active = true;
    newBullet->damage = 1;
    newBullet->canRicochet = true;  
    newBullet->ricochetsLeft = 1;   
    newBullet->next = *head;
    *head = newBullet;
}