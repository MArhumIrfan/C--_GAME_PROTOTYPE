#pragma once
#include <cstdint>
#include <SDL2/SDL.h>
#include <string>      // <-- ADDED THIS
#include "GameData.h"  // <-- ADDED THIS

struct AudioSystem {
    float masterVolume = 0.5f;
    float ambientPhase = 0.0f;
    float heartbeatPhase = 0.0f;
    float monsterPhase = 0.0f;
    float screamPhase = 0.0f;
    float footstepPhase = 0.0f;
    float breathingPhase = 0.0f;
    float fakeFootstepPhase = 0.0f;
    float fakeFootstepTimer = 0.0f;

    int itemSoundType = 0;
    float itemSoundTimer = 0.0f;
    float itemSoundPhase = 0.0f;

    // Data from game
    float sanity = 100.0f;
    float closestEnemyDist = 99.0f;
    float corruption = 0.0f;
    bool isChasing = false;
    bool isJumpscare = false;
    bool inGame = false;
    bool isMoving = false;
    bool isSprinting = false;
    bool isHiding = false;

    uint32_t rngSeed = 1337;

    void update(float dt);
    void triggerItemSound(ItemType type);
    void triggerStalkerSound(const std::string& type);

    static void audioCallback(void* userdata, Uint8* stream, int len);
};