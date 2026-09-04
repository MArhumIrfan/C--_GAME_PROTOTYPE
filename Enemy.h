#pragma once
#include "GameData.h"
#include "Player.h"
#include <vector>

class Enemy {
public:
    float x = 12.5f, y = 12.5f;
    float speed = 1.8f;
    bool active = false;
    AIState state = AI_STATE_IDLE;
    bool isChasing = false; // For audio cue
    
    float animTimer = 0.0f, lungeTimer = 0.0f;
    int currentFrame = 0;
    
    std::vector<Point> currentPath;
    float pathRecalculateTimer = 0.0f;

    void update(float dt, const Player& player, const MapCell worldMap[27][27]);
    void reset(bool setActive, AIState startState);
    void investigate(float targetX, float targetY);

private:
    std::vector<Point> findPath(Point start, Point end, const MapCell worldMap[27][27]);
    void moveToward(float targetX, float targetY, float dtSec);
};