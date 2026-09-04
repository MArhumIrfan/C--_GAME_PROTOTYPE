#include "Player.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

void Player::update(float dt, const MapCell worldMap[MAP_H][MAP_H]) { // <-- CORRECTED
    if (lanternOn && lanternFuel > 0) {
        lanternFuel -= LANTERN_FUEL_CONSUMPTION_RATE * dt;
        if (lanternFuel <= 0) {
            lanternFuel = 0;
            lanternOn = false;
        }
    }

    eyeHeight += (targetEyeHeight - eyeHeight) * 0.2f;

    if (isMoving()) {
        headbobTimer += dt * (isSprinting ? 12.0f : 7.0f);
        targetEyeHeight += std::sin(headbobTimer) * 0.01f;
    } else {
        headbobTimer = 0;
    }

    if (forward != 0 || strafe != 0) {
        float forwardStep = forward * moveSpeed * dt;
        float strafeStep = strafe * (moveSpeed * 0.85f) * dt;
        float moveX = dirX * forwardStep - dirY * strafeStep;
        float moveY = dirY * forwardStep + dirX * strafeStep;
        float bufX = (moveX > 0) ? 0.32f : -0.32f;
        float bufY = (moveY > 0) ? 0.32f : -0.32f;
        float prevX = posX;
        float prevY = posY;

        int nextTileX_X = std::clamp(int(posX + moveX + bufX), 0, MAP_W - 1);
        int currentTileY = std::clamp(int(posY), 0, MAP_H - 1);
        if (worldMap[currentTileY][nextTileX_X].wallType != 1 && worldMap[currentTileY][nextTileX_X].wallType != 5) {
            posX += moveX;
        }

        int currentTileX = std::clamp(int(posX), 0, MAP_W - 1);
        int nextTileY_Y = std::clamp(int(posY + moveY + bufY), 0, MAP_H - 1);
        if (worldMap[nextTileY_Y][currentTileX].wallType != 1 && worldMap[nextTileY_Y][currentTileX].wallType != 5) {
            posY += moveY;
        }
        
        stepAccumulator += std::hypot(posX - prevX, posY - prevY);
        noiseLevel = isSprinting ? 1.0f : 0.5f;
    } else {
        noiseLevel = 0.0f;
    }

    if (worldMap[int(posY)][int(posX)].wallType == 3) {
        isCrouching = true;
        moveSpeed = PLAYER_BASE_SPEED * CROUCH_SPEED_MULTIPLIER;
        targetEyeHeight = 0.45f;
        health = std::min(100.0f, health + CRAWLSPACE_HEAL_RATE * dt);
    } else {
        if (!isCrouching) {
             moveSpeed = isSprinting ? PLAYER_BASE_SPEED * SPRINT_SPEED_MULTIPLIER : PLAYER_BASE_SPEED;
             targetEyeHeight = 0.8f;
        }
    }
}

void Player::reset() {
    posX = 1.5f; posY = 1.5f;
    eyeHeight = 0.8f; targetEyeHeight = 0.8f; pitch = 0.0f;
    dirX = 1.0f; dirY = 0.0f; planeX = 0.0f; planeY = 0.66f;
    moveSpeed = PLAYER_BASE_SPEED;
    forward = 0; strafe = 0;
    isSprinting = false; isCrouching = false;
    stepAccumulator = 0.0f;
    sanity = 100.0f; health = 100.0f;
    takingDamage = false;
    for (int i = 0; i < 3; ++i) inventory[i] = ITEM_NONE;
    toxicTimer = 0.0f;
    lanternOn = false; lanternBroken = false;
    reigniteClicks = 0; lanternFuel = LANTERN_FUEL_MAX;
}