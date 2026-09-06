#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

#include "GameTypes.h"

constexpr int AUDIO_SAMPLE_RATE = 44100;
constexpr int AUDIO_BUFFER_SIZE = 1024;

// ==========================================
// AUDIO SYSTEM (GENERATIVE AMBIENCE)
// ==========================================
struct AudioState {
    int gameState = STATE_TITLE;
    int currentLevel = 1;
    float globalTime = 0.0f;

    float heartbeatPhase = 0.0f;
    float monsterPhase = 0.0f;
    float screamPhase = 0.0f;
    float footstepPhase = 0.0f;
    
    int itemSoundType = 0; 
    float itemSoundTimer = 0.0f;
    float itemSoundPhase = 0.0f;

    int monsterVocalType = 0; 
    float monsterVocalTimer = 0.0f;
    float monsterVocalPhase = 0.0f;
    
    float ambientEventCooldown = 5.0f;
    float ambientEventTimer = 0.0f;
    float ambientEventPhase = 0.0f;
    int ambientEventType = 0;

    int floorSurface = 0; 
    float sanity = 100.0f;
    float closestEnemyDist = 20.0f;
    float corruption = 0.0f;
    
    bool isChasing = false;
    bool isJumpscare = false;
    bool inGame = false;
    
    bool isMoving = false;
    bool isSprinting = false;
    bool isCrouching = false;
    
    uint32_t rngSeed = 1337;
    float masterVolume = 0.5f;
};

// SDL audio device callback. Synthesizes the main-menu theme, per-level
// background ambience/corruption arc, footsteps, heartbeat, monster
// breathing/vocalizations, and one-shot item/interaction sounds entirely
// procedurally from the fields in AudioState - no audio assets.
void audioCallback(void* userdata, Uint8* stream, int len);
