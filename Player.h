#pragma once
<<<<<<< HEAD

#include "GameTypes.h"

struct Player {
    float posX = 1.5f;
    float posY = 1.5f;
    float posZ = 0.0f;
    float targetPosZ = 0.0f;
    float eyeHeight = 0.8f;
    float targetEyeHeight = 0.8f;
    float pitch = 0.0f;
    float dirX = 1.0f;
    float dirY = 0.0f;
    float planeX = 0.0f;
    float planeY = 0.66f;
    float baseMoveSpeed = 3.2f;
    float moveSpeed = 3.2f;
    float mouseSensitivity = 0.5f;
    int forward = 0;
    int strafe = 0;
    bool isSprinting = false;
    bool isCrouching = false;
    float stepAccumulator = 0.0f;
    float sanity = 100.0f;
    float health = 100.0f;
    float stamina = 100.0f;
    bool takingDamage = false;
    float damageShake = 0.0f;
    ItemType inventory[3] = {ITEM_NONE, ITEM_NONE, ITEM_NONE};
    float toxicTimer = 0.0f;
    bool lanternOn = false;
    bool lanternBroken = false;
    int reigniteClicks = 0;

    bool hasKey = false;
    float showLockedMessage = 0.0f;
    bool inLocker = false;

    // Resets everything that should be fresh at the start of a level
    // (position, orientation, movement state, lantern, locker) while
    // deliberately leaving sanity/health/stamina/inventory untouched -
    // those persist across levels (see nextLevel()/startNewGame() in the
    // engine, which manage them intentionally) and hasKey, which depends
    // on the engine's currentLevel and is set by the caller after this.
    void resetForNewLevel(float spawnX, float spawnY);
};
=======
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
>>>>>>> 3d16c596295cdde7651fdce0ae976730070fcab2
