#include "utils.h"

#include "raymath.h"  
#include <math.h> 


bool isFullscreenGlobal = false;


float PLAY_AREA_LEFT = PLAY_AREA_MARGIN;
float PLAY_AREA_TOP = PLAY_AREA_MARGIN;
float PLAY_AREA_RIGHT = SCREEN_WIDTH - PLAY_AREA_MARGIN;
float PLAY_AREA_BOTTOM = SCREEN_HEIGHT - PLAY_AREA_MARGIN;


float currentPlayAreaRadius;  
float targetPlayAreaRadius;   
float areaChangeTimer = 0.3f;                    
float areaTransitionSpeed = 1.0f;                
bool isAreaShrinking = false;                    

void UpdateScreenSizeVars(void) {
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();
    
    
    float hudHeight = 60.0f;
    float extraMargin = 10.0f; 
    
    
    PLAY_AREA_LEFT = (currentWidth / 2.0f) - currentPlayAreaRadius;
    PLAY_AREA_RIGHT = (currentWidth / 2.0f) + currentPlayAreaRadius;
    PLAY_AREA_TOP = MAX(hudHeight + extraMargin, (currentHeight / 2.0f) - currentPlayAreaRadius);
    PLAY_AREA_BOTTOM = (currentHeight / 2.0f) + currentPlayAreaRadius;
}

void AppToggleFullscreen(void) {
    
    ToggleFullscreen(); 

    
    isFullscreenGlobal = !isFullscreenGlobal;
}


float LerpUtil(float start, float end, float t) {
    return start + t * (end - start);
}

bool IsPointInPlayArea(Vector2 point) {
    float distanceToCenter = Vector2Distance(
        point,
        (Vector2){PLAY_AREA_CENTER_X, PLAY_AREA_CENTER_Y}
    );
    
    return distanceToCenter <= currentPlayAreaRadius;
}


void InitPlayArea(void) {
    
    float baseRadius = fminf(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f - PLAY_AREA_MARGIN;
    currentPlayAreaRadius = baseRadius;  
    targetPlayAreaRadius = baseRadius;   
    
    
    isAreaShrinking = false;
    areaChangeTimer = 0.0f;
    
    
    UpdateScreenSizeVars();
}


void UpdateDynamicPlayArea(float deltaTime, float gameScore) {
    
    if (gameScore >= 1000) {
        areaChangeTimer += deltaTime;
    }
    
    
    if (currentPlayAreaRadius != targetPlayAreaRadius) {
        float direction = (targetPlayAreaRadius > currentPlayAreaRadius) ? 1.0f : -1.0f;
        float step = deltaTime * areaTransitionSpeed * 120.0f;
        
        currentPlayAreaRadius += direction * step;
        
        
        if ((direction > 0 && currentPlayAreaRadius >= targetPlayAreaRadius) ||
            (direction < 0 && currentPlayAreaRadius <= targetPlayAreaRadius)) {
            currentPlayAreaRadius = targetPlayAreaRadius;
        }
    }
    
    
    if (gameScore >= 1000) {
        
        if (areaChangeTimer >= 10.0f) {
            areaChangeTimer = 0.0f;
            isAreaShrinking = !isAreaShrinking;  
            
            
            float baseRadius = fminf(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f - PLAY_AREA_MARGIN;
            
            
            float minRadius = baseRadius * 0.7f;  
            float maxRadius = baseRadius * 1.2f;  
            
            if (isAreaShrinking) {
                targetPlayAreaRadius = minRadius;
            } else {
                targetPlayAreaRadius = maxRadius;
            }
        }
    }
    
    
    UpdateScreenSizeVars();
}

