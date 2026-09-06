#pragma once
#include "GameData.h"

class Player {
public:
    float posX = 1.5f, posY = 1.5f, posZ = 0.0f, targetPosZ = 0.0f;
    float eyeHeight = 0.8f, targetEyeHeight = 0.8f, pitch = 0.0f;
    float dirX = 1.0f, dirY = 0.0f, planeX = 0.0f, planeY = 0.66f;
    
    float baseMoveSpeed = 3.2f;
    float moveSpeed = 3.2f;
    float mouseSensitivity = 0.5f;
    int forward = 0, strafe = 0;
    bool isSprinting = false, isCrouching = false;
    
    float stepAccumulator = 0.0f;
    float headbobTimer = 0.0f;

    float sanity = 100.0f, health = 100.0f, stamina = 100.0f;
    bool takingDamage = false;
    float damageShake = 0.0f;
    
    ItemType inventory[3] = { ITEM_NONE, ITEM_NONE, ITEM_NONE };
    float toxicTimer = 0.0f;

    bool lanternOn = false, lanternBroken = false;
    int reigniteClicks = 0;
    float lanternFuel = 100.0f;

    bool hasKey = false;
    float showLockedMessage = 0.0f;
    bool inLocker = false;
    float noiseLevel = 0.0f;

    void update(float dt, const MapCell worldMap[MAP_H][MAP_W]);
    void reset();
    void resetForNewLevel(float spawnX, float spawnY);
    bool isMoving() const { return forward != 0 || strafe != 0; }
};