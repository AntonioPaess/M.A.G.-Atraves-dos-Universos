#include "player.h"
#include "raylib.h" // For Raylib functions like IsKeyDown, Vector2, etc.
#include <math.h> // For fabsf if normalizing direction

void InitPlayer(Player *player, int windowWidth, int windowHeight) {
    player->size = (Vector2){ PLAYER_WIDTH, PLAYER_HEIGHT };
    player->position = (Vector2){
        (float)(windowWidth - player->size.x) / 2.0f,
        (float)(windowHeight - player->size.y - 10.0f) // Initial position at the bottom center
    };
    player->lives = PLAYER_LIVES;
    player->last_direction = (Vector2){ 0.0f, -1.0f }; // Initially looking up
}

void UpdatePlayer(Player *player, float deltaTime, int windowWidth, int windowHeight) {
    Vector2 movement_input = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement_input.y = -1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement_input.y = 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement_input.x = -1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement_input.x = 1.0f;

    // Normalize diagonal movement if necessary, or simply scale.
    // For simple scaling as in original:
    player->position.x += movement_input.x * PLAYER_SPEED * deltaTime;
    player->position.y += movement_input.y * PLAYER_SPEED * deltaTime;

    // Update last_direction if there was movement
    if (movement_input.x != 0.0f || movement_input.y != 0.0f) {
        // Normalize the last_direction vector
        float length = sqrtf(movement_input.x * movement_input.x + movement_input.y * movement_input.y);
        if (length > 0) { // Avoid division by zero
            player->last_direction.x = movement_input.x / length;
            player->last_direction.y = movement_input.y / length;
        } else {
            // If somehow length is 0 but there was input, keep previous last_direction or set a default
            // This case should ideally not happen if movement_input.x or .y is non-zero.
        }
    } // If no movement, last_direction remains as it was.

    // Keep player within window bounds
    if (player->position.x < 0) player->position.x = 0;
    if (player->position.y < 0) player->position.y = 0;
    if (player->position.x + player->size.x > windowWidth) player->position.x = windowWidth - player->size.x;
    if (player->position.y + player->size.y > windowHeight) player->position.y = windowHeight - player->size.y;
}

// The drawPlayer function has been removed as per the new modular structure.
// Rendering will be handled by the render module (render.c).

