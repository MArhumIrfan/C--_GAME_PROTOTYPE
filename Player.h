#pragma once
#include "GameData.h"

class Player {
public:
    float posX = 1.5f, posY = 1.5f;
    float eyeHeight = 0.8f, targetEyeHeight = 0.8f, pitch = 0.0f;
    float dirX = 1.0f, dirY = 0.0f, planeX = 0.0f, planeY = 0.66f;
    float moveSpeed = 3.2f;
    float mouseSensitivity = 0.5f;
    int forward = 0, strafe = 0;
    bool isSprinting = false, isCrouching = false;
    float stepAccumulator = 0.0f;
    float headbobTimer = 0.0f;

    float sanity = 100.0f, health = 100.0f;
    bool takingDamage = false;
    ItemType inventory[3] = { ITEM_NONE, ITEM_NONE, ITEM_NONE }; // <-- CORRECTED THIS LINE
    float toxicTimer = 0.0f;

    bool lanternOn = false, lanternBroken = false;
    int reigniteClicks = 0;
    float lanternFuel = 100.0f;

    float noiseLevel = 0.0f;

    void update(float dt, const MapCell worldMap[27][27]); // <-- CORRECTED THIS LINE
    void reset();
    bool isMoving() const { return forward != 0 || strafe != 0; }
};