#include "Enemy.h"

void Enemy::resetForNewLevel(float spawnX, float spawnY) {
    x = spawnX;
    y = spawnY;
    awareness = 0.0f;
    active = false;
    mode = 0;
    isChasing = false;
    enragedTimer = 0.0f;
    investigateTimer = 0.0f;
    doorAttackTimer = 0.0f;
    currentPath.clear();
    pathRecalculateTimer = 0.0f;
    spottedPlayer = false;
    wasHunting = false;
}
