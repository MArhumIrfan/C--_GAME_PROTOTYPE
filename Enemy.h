#pragma once
<<<<<<< HEAD

#include "GameTypes.h"
#include <vector>

struct Enemy {
    float x = 12.5f;
    float y = 12.5f;
    float speed = 1.8f;
    float awareness = 0.0f;
    bool active = false;
    int mode = 0;
    bool isChasing = false;
    float animTimer = 0.0f;
    int currentFrame = 0;
    float enragedTimer = 0.0f;
    float investigateX = 0.0f;
    float investigateY = 0.0f;
    float investigateTimer = 0.0f;
    std::vector<Point> currentPath;
    float pathRecalculateTimer = 0.0f;
    float doorAttackTimer = 0.0f;
    bool spottedPlayer = false;
    bool wasHunting = false;

    // Resets everything to a fresh per-level state at the given spawn
    // point. Used for both the stalker and the mist enemy.
    //
    // NOTE: the pre-refactor code only reset currentPath/pathRecalculate-
    // Timer/investigateTimer/enragedTimer/doorAttackTimer for the stalker -
    // the mist enemy's reset skipped them, even though the mist enemy does
    // use currentPath/pathRecalculateTimer for its own movement. That looks
    // like an accidental omission (stale pathfinding state could carry over
    // into a new level's maze) rather than an intentional difference, so
    // this unified reset clears all of it for both enemy types.
    void resetForNewLevel(float spawnX, float spawnY);
};
=======
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
>>>>>>> 3d16c596295cdde7651fdce0ae976730070fcab2
