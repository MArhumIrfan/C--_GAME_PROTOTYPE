#pragma once
#include "GameData.h"
#include <vector>

class Player; // Forward declaration

class Enemy {
public:
    float x = 12.5f, y = 12.5f;
    float speed = 1.8f;
    bool active = false;
    AIState state = AI_STATE_IDLE;
    bool isChasing = false;
    
    float animTimer = 0.0f, lungeTimer = 0.0f;
    int currentFrame = 0;
    
    Point investigateTarget;
    float investigateTimer = 0.0f;
    
    std::vector<Point> currentPath;
    float pathRecalculateTimer = 0.0f;

    void update(float dt, const Player& player, const MapCell worldMap[27][27]); // <-- CORRECTED
    void reset(bool setActive, AIState startState);
    void setInvestigateTarget(float targetX, float targetY, const MapCell worldMap[27][27]); // <-- CORRECTED

private:
    std::vector<Point> findPath(Point start, Point end, const MapCell worldMap[27][27]); // <-- CORRECTED
    void moveToward(float targetX, float targetY, float dtSec);
};