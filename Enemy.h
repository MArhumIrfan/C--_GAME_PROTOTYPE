#pragma once

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
