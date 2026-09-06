#include "Player.h"

void Player::resetForNewLevel(float spawnX, float spawnY) {
    posX = spawnX;
    posY = spawnY;
    posZ = 0.0f;
    targetPosZ = 0.0f;
    pitch = 0.0f;
    dirX = 1.0f;
    dirY = 0.0f;
    planeX = 0.0f;
    planeY = 0.66f;
    stepAccumulator = 0.0f;
    toxicTimer = 0.0f;
    stamina = 100.0f;

    lanternOn = false;
    lanternBroken = false;
    reigniteClicks = 0;

    inLocker = false;
    showLockedMessage = 0.0f;
}
