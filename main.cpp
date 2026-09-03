#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stack>

constexpr int CHAR_W = 6;
constexpr int CHAR_H = 6;
constexpr int TOTAL_COLS = 133; 
constexpr int ROWS = 80;         
constexpr int NATIVE_WIDTH = TOTAL_COLS * CHAR_W;  
constexpr int NATIVE_HEIGHT = ROWS * CHAR_H;       

struct ResolutionPreset {
    int width;
    int height;
    std::string label;
};

const std::vector<ResolutionPreset> RESOLUTION_PRESETS = {
    { 800,  480, "800x480 (1X)" },
    { 1280, 720, "1280x720 (HD)" },
    { 1366, 768, "1366x768 (WXGA)" },
    { 1600, 960, "1600x960 (2X)" },
    { 1920, 1080, "1920x1080 (FHD)" }
};

constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;

constexpr int MAP_W = 27;
constexpr int MAP_H = 27;

// Realistic Color Palettes for Themes
constexpr uint32_t THEME0_BRIGHT = 0xFF64748B;
constexpr uint32_t THEME0_MID    = 0xFF475569;
constexpr uint32_t THEME0_DARK   = 0xFF1E293B;

constexpr uint32_t THEME1_BRIGHT = 0xFF854D0E;
constexpr uint32_t THEME1_MID    = 0xFF653B0B;
constexpr uint32_t THEME1_DARK   = 0xFF422506;

constexpr uint32_t THEME2_BRIGHT = 0xFF4D7C0F;
constexpr uint32_t THEME2_MID    = 0xFF3F6212;
constexpr uint32_t THEME2_DARK   = 0xFF1A2E05;

constexpr uint32_t THEME3_BRIGHT = 0xFF78350F;
constexpr uint32_t THEME3_MID    = 0xFF451A03;
constexpr uint32_t THEME3_DARK   = 0xFF1C1917;

// Restored UI / Status Colors
constexpr uint32_t TIER_HIGH_BRIGHT = 0xFF86EFAC;
constexpr uint32_t TIER_HIGH_DARK   = 0xFF22C55E;
constexpr uint32_t TIER_MID_BRIGHT  = 0xFF38BDF8;
constexpr uint32_t TIER_LOW_BRIGHT  = 0xFF16A34A;

// Corrupted Nightmare Palettes
constexpr uint32_t CORRUPT_BRIGHT   = 0xFFF43F5E;
constexpr uint32_t CORRUPT_MID      = 0xFFBE123C;
constexpr uint32_t CORRUPT_DARK     = 0xFF881337;

constexpr uint32_t RED_GOAL_BRIGHT  = 0xFFF43F5E;
constexpr uint32_t RED_GOAL_DARK    = 0xFFBE123C;

constexpr int AUDIO_SAMPLE_RATE = 44100;
constexpr int AUDIO_BUFFER_SIZE = 1024;

enum GameState {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_JUMPSCARE,
    STATE_SUCCESS,
    STATE_GAMEOVER
};

enum Difficulty {
    DIFF_NORMAL = 0,
    DIFF_EASY   = 1
};

enum ItemType {
    ITEM_NONE,
    ITEM_BREAD,
    ITEM_MEDS,
    ITEM_PEBBLE
};

struct ItemEntity {
    float x;
    float y;
    ItemType type;
};

struct Projectile {
    float x, y, z;
    float vx, vy, vz;
    bool active = false;
};

const uint8_t FONT_8X8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},
    {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00},
    {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, {0x00,0x66,0xAC,0xD8,0x36,0x6A,0x00,0x00},
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00},
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},
    {0x7C,0xC6,0xCE,0xD6,0xE6,0xC6,0x7C,0x00}, {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},
    {0x7C,0xC6,0x06,0x1C,0x30,0x66,0xFE,0x00}, {0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00},
    {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00}, {0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00},
    {0x7C,0xC6,0xC0,0xFC,0xC6,0xC6,0x7C,0x00}, {0xFE,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},
    {0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00}, {0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00},
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30},
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00},
    {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00}, {0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00},
    {0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00}, {0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00},
    {0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00}, {0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00},
    {0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00}, {0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00},
    {0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00}, {0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00},
    {0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},
    {0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00}, {0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00},
    {0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00}, {0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00},
    {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00}, {0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00},
    {0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00}, {0x7C,0xC6,0xC6,0xC6,0xC6,0xCE,0x7C,0x06},
    {0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE6,0x00}, {0x7C,0xC6,0x60,0x38,0x0C,0xC6,0x7C,0x00},
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, {0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00},
    {0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x00}, {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00},
    {0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00}, {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},
    {0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00}, {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},
    {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00},
    // NOTE: the table above only covered ASCII 32-93 (space through ']'),
    // leaving ^ _ ` a-z { | } ~ DEL zero-initialized (i.e. invisible).
    // That silently blanked out lowercase HUD text (e.g. "Sprint", "Crouch",
    // "v1.0") and several characters used directly by the ASCII-art sprites
    // (the stalker/bread/meds art uses '_', '~', '|'). Filled in below.
    {0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // ^ _
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x78,0x0C,0x7C,0xCC,0x76,0x00}, // ` a
    {0xE0,0x60,0x60,0x7C,0x66,0x66,0xDC,0x00}, {0x00,0x00,0x78,0xCC,0xC0,0xCC,0x78,0x00}, // b c
    {0x1C,0x0C,0x0C,0x7C,0xCC,0xCC,0x76,0x00}, {0x00,0x00,0x78,0xCC,0xFC,0xC0,0x78,0x00}, // d e
    {0x38,0x6C,0x60,0xF0,0x60,0x60,0xF0,0x00}, {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0xF8}, // f g
    {0xE0,0x60,0x6C,0x76,0x66,0x66,0xE6,0x00}, {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00}, // h i
    {0x06,0x00,0x06,0x06,0x06,0x66,0x66,0x3C}, {0xE0,0x60,0x66,0x6C,0x78,0x6C,0xE6,0x00}, // j k
    {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, {0x00,0x00,0xEC,0xFE,0xD6,0xD6,0xD6,0x00}, // l m
    {0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x00}, {0x00,0x00,0x78,0xCC,0xCC,0xCC,0x78,0x00}, // n o
    {0x00,0x00,0xDC,0x66,0x66,0x7C,0x60,0xF0}, {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0x1E}, // p q
    {0x00,0x00,0xDC,0x76,0x66,0x60,0xF0,0x00}, {0x00,0x00,0x7C,0xC0,0x78,0x0C,0xF8,0x00}, // r s
    {0x10,0x30,0x7C,0x30,0x30,0x34,0x18,0x00}, {0x00,0x00,0xCC,0xCC,0xCC,0xCC,0x76,0x00}, // t u
    {0x00,0x00,0xCC,0xCC,0xCC,0x78,0x30,0x00}, {0x00,0x00,0xC6,0xD6,0xD6,0xFE,0x6C,0x00}, // v w
    {0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00}, {0x00,0x00,0xC6,0xC6,0xC6,0x7E,0x06,0xFC}, // x y
    {0x00,0x00,0xFC,0x98,0x30,0x64,0xFC,0x00}, {0x1C,0x30,0x30,0xE0,0x30,0x30,0x1C,0x00}, // z {
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, {0xE0,0x30,0x30,0x1C,0x30,0x30,0xE0,0x00}, // | }
    {0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  // ~ DEL
};

struct Point { int x, y; };

struct MapCell {
    int wallType = 0; 
    float floorH = 0.0f;
    float ceilH = 2.0f;
    bool isStairs = false;
};

struct AudioState {
    float ambientPhase = 0.0f;
    float heartbeatPhase = 0.0f;
    float monsterPhase = 0.0f;
    float screamPhase = 0.0f;
    float footstepPhase = 0.0f;
    
    float sanity = 100.0f;
    float monsterDist = 20.0f;
    float corruption = 0.0f;
    
    bool isChasing = false;
    bool isJumpscare = false;
    bool inGame = false;
    
    bool isMoving = false;
    bool isSprinting = false;
    bool isCrouching = false;
    
    uint32_t rngSeed = 1337; // Dedicated, thread-safe seed for audio
};

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioState* audio = static_cast<AudioState*>(userdata);
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);

    // Private, thread-safe noise generator (Fast LCG algorithm)
    auto getAudioNoise = [](uint32_t& seed) -> float {
        seed = seed * 1664525 + 1013904223;
        return (static_cast<float>(seed >> 16) / 32768.0f) * 2.0f - 1.0f;
    };

    for (int i = 0; i < samples; ++i) {
        if (!audio->inGame && !audio->isJumpscare) {
            buffer[i] = 0;
            continue;
        }

        if (audio->isJumpscare) {
            audio->screamPhase += (350.0f * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
            if (audio->screamPhase > 2.0f * 3.14159265f) audio->screamPhase -= 2.0f * 3.14159265f;
            
            float screech = std::sin(audio->screamPhase) * 0.5f;
            float rawNoise = getAudioNoise(audio->rngSeed) * 0.7f;
            float demonicRumble = std::sin(audio->screamPhase * 0.1f) * 0.4f;

            buffer[i] = static_cast<int16_t>(std::clamp(screech + rawNoise + demonicRumble, -1.0f, 1.0f) * 32767.0f);
            continue;
        }

        float mixAmbient = std::clamp((audio->corruption - 0.2f) * 4.0f, 0.0f, 1.0f);
        audio->ambientPhase += (42.0f * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
        if (audio->ambientPhase > 2.0f * 3.14159265f) audio->ambientPhase -= 2.0f * 3.14159265f;
        float ambient = std::sin(audio->ambientPhase) * 0.0896f * mixAmbient; 

        // Smoothed Footstep Engine
        float footstep = 0.0f;
        if (audio->isMoving && !audio->isCrouching) {
            float stepFreq = audio->isSprinting ? 4.5f : 2.5f;
            audio->footstepPhase += (stepFreq * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
            if (audio->footstepPhase > 2.0f * 3.14159265f) audio->footstepPhase -= 2.0f * 3.14159265f;
            
            // Replaces the harsh ON/OFF click with a curved envelope to sound like crunching gravel
            float stepEnv = std::max(0.0f, std::sin(audio->footstepPhase));
            stepEnv = std::pow(stepEnv, 6.0f); 
            footstep = getAudioNoise(audio->rngSeed) * stepEnv * 0.15f; 
        } else {
            audio->footstepPhase = 0.0f; 
        }

        float heartBPM = 1.0f + (100.0f - audio->sanity) / 100.0f * 2.0f;
        audio->heartbeatPhase += (heartBPM * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
        if (audio->heartbeatPhase > 2.0f * 3.14159265f) audio->heartbeatPhase -= 2.0f * 3.14159265f;

        float mixHeart = std::clamp((audio->corruption - 0.5f) * 3.0f, 0.0f, 1.0f);
        float beatEnv = 0.0f;
        float cyclePos = audio->heartbeatPhase / (2.0f * 3.14159265f);
        if (cyclePos < 0.15f) beatEnv = std::sin(cyclePos / 0.15f * 3.14159265f);
        else if (cyclePos > 0.22f && cyclePos < 0.35f) beatEnv = std::sin((cyclePos - 0.22f) / 0.13f * 3.14159265f) * 0.7f;
        float heartbeat = std::sin(audio->heartbeatPhase * 40.0f) * beatEnv * (0.35f + (100.0f - audio->sanity) / 100.0f * 0.50f) * mixHeart;

        float monsterAudio = 0.0f;
        if (audio->monsterDist < 10.0f) {
            float mixMonster = std::clamp((audio->corruption - 0.6f) * 3.0f, 0.0f, 1.0f);
            float proxVol = 1.0f - (audio->monsterDist / 10.0f);
            float breathFreq = audio->isChasing ? 2.5f : 0.8f;
            audio->monsterPhase += (breathFreq * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
            if (audio->monsterPhase > 2.0f * 3.14159265f) audio->monsterPhase -= 2.0f * 3.14159265f;
            
            // Smoothly thread-safe monster breathing
            float breathEnv = std::sin(audio->monsterPhase) * 0.5f + 0.5f;
            monsterAudio = getAudioNoise(audio->rngSeed) * breathEnv * proxVol * 0.4f * mixMonster;
        }

        buffer[i] = static_cast<int16_t>(std::clamp(ambient + footstep + heartbeat + monsterAudio, -1.0f, 1.0f) * 32767.0f);
    }
}

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioState* audio = static_cast<AudioState*>(userdata);
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);

    for (int i = 0; i < samples; ++i) {
        if (!audio->inGame && !audio->isJumpscare) {
            buffer[i] = 0;
            continue;
        }

        if (audio->isJumpscare) {
            audio->screamPhase += (350.0f * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
            if (audio->screamPhase > 2.0f * 3.14159265f) audio->screamPhase -= 2.0f * 3.14159265f;
            float screech = std::sin(audio->screamPhase) * 0.5f;
            float rawNoise = ((rand() % 2000) / 1000.0f - 1.0f) * 0.7f;
            float demonicRumble = std::sin(audio->screamPhase * 0.1f) * 0.4f;
            buffer[i] = static_cast<int16_t>(std::clamp(screech + rawNoise + demonicRumble, -1.0f, 1.0f) * 32767.0f);
            continue;
        }

        float mixAmbient = std::clamp((audio->corruption - 0.2f) * 4.0f, 0.0f, 1.0f);
        audio->ambientPhase += (42.0f * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
        if (audio->ambientPhase > 2.0f * 3.14159265f) audio->ambientPhase -= 2.0f * 3.14159265f;
        float ambient = std::sin(audio->ambientPhase) * 0.0896f * mixAmbient; 

        float footstep = 0.0f;
        if (audio->isMoving && !audio->isCrouching) {
            float stepFreq = audio->isSprinting ? 4.5f : 2.5f;
            audio->footstepPhase += (stepFreq * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
            if (audio->footstepPhase > 2.0f * 3.14159265f) audio->footstepPhase -= 2.0f * 3.14159265f;
            
            float stepEnv = std::sin(audio->footstepPhase);
            if (stepEnv > 0.85f) { 
                float noise = ((rand() % 2000) / 1000.0f - 1.0f);
                footstep = noise * 0.15f; 
            }
        } else {
            audio->footstepPhase = 0.0f; 
        }

        float heartBPM = 1.0f + (100.0f - audio->sanity) / 100.0f * 2.0f;
        audio->heartbeatPhase += (heartBPM * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
        if (audio->heartbeatPhase > 2.0f * 3.14159265f) audio->heartbeatPhase -= 2.0f * 3.14159265f;

        float mixHeart = std::clamp((audio->corruption - 0.5f) * 3.0f, 0.0f, 1.0f);
        float beatEnv = 0.0f;
        float cyclePos = audio->heartbeatPhase / (2.0f * 3.14159265f);
        if (cyclePos < 0.15f) beatEnv = std::sin(cyclePos / 0.15f * 3.14159265f);
        else if (cyclePos > 0.22f && cyclePos < 0.35f) beatEnv = std::sin((cyclePos - 0.22f) / 0.13f * 3.14159265f) * 0.7f;
        float heartbeat = std::sin(audio->heartbeatPhase * 40.0f) * beatEnv * (0.35f + (100.0f - audio->sanity) / 100.0f * 0.50f) * mixHeart;

        float monsterAudio = 0.0f;
        if (audio->monsterDist < 10.0f) {
            float mixMonster = std::clamp((audio->corruption - 0.6f) * 3.0f, 0.0f, 1.0f);
            float proxVol = 1.0f - (audio->monsterDist / 10.0f);
            float breathFreq = audio->isChasing ? 2.5f : 0.8f;
            audio->monsterPhase += (breathFreq * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
            if (audio->monsterPhase > 2.0f * 3.14159265f) audio->monsterPhase -= 2.0f * 3.14159265f;
            float noise = ((rand() % 2000) / 1000.0f - 1.0f);
            monsterAudio = noise * (std::sin(audio->monsterPhase) * 0.5f + 0.5f) * proxVol * 0.4f * mixMonster;
        }

        buffer[i] = static_cast<int16_t>(std::clamp(ambient + footstep + heartbeat + monsterAudio, -1.0f, 1.0f) * 32767.0f);
    }
}

class WalkAsciiElevationEngine {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* screenTexture = nullptr;
    SDL_AudioDeviceID audioDevice = 0;
    std::vector<uint32_t> pixelBuffer;
    bool isRunning = false;

    GameState currentState = STATE_TITLE;
    Difficulty currentDifficulty = DIFF_NORMAL;
    int currentResIndex = 2;
    int menuCursor = 0;

    AudioState audioState;
    MapCell worldMap[MAP_H][MAP_W];
    Point startPos;
    Point endPos;

    std::vector<ItemEntity> itemsInWorld;
    Projectile activePebble; 

    int currentLevel = 1;
    int totalSteps = 0;
    float levelTime = 0.0f;
    float corruptionLevel = 0.0f; 
    std::string deathReason = "";
    float jumpscareTimer = 0.0f;

    struct Player {
        float posX = 1.5f;
        float posY = 1.5f;
        float posZ = 0.0f;
        float targetPosZ = 0.0f;
        float eyeHeight = 0.5f;
        float targetEyeHeight = 0.5f;
        float pitch = 0.0f;

        float dirX = 1.0f;
        float dirY = 0.0f;
        float planeX = 0.0f;
        float planeY = 0.66f;
        float baseMoveSpeed = 3.2f;
        float moveSpeed = 3.2f;
        float mouseSensitivity = 0.0022f;

        int forward = 0; 
        int strafe = 0;  
        bool isSprinting = false;
        bool isCrouching = false;
        float stepAccumulator = 0.0f;
        
        float sanity = 100.0f;
        float health = 100.0f;
        bool takingDamage = false;

        ItemType inventory[3] = {ITEM_NONE, ITEM_NONE, ITEM_NONE};
        float toxicTimer = 0.0f;
    } player;

    const std::vector<std::string> spriteBread = {
        "          .-\"\"\"\"\"\"\"\"\"\"\"\"-.          ",
        "        .-'                '-.        ",
        "      .'                      '.      ",
        "     /    .--.                 \\      ",
        "    ;    /    \\    .           ;      ",
        "    |   |  ()  |       .       |      ",
        "    |    \\____/   .           |       ",
        "    ;       .       .   *     ;       ",
        "     \\   .      *             /       ",
        "      '.                    .'        ",
        "        '-.______________.-'          ",
        "           .  *    .  *  .            "
    };

    const std::vector<std::string> spriteMeds = {
        "                         .-~~~~~-.                      ",
        "                       .'  .---.  '.                    ",
        "                      /   /     \\   \\                   ",
        "                     |   |  _ _  |   |                  ",
        "                     |   | |   | |   |                  ",
        "                     |   | |___| |   |                  ",
        "                     |    \\_____/    |                  ",
        "                     |      |||      |                  ",
        "                 ____|______|||______|____              ",
        "              .-'                         '-.           ",
        "            .'       .------------- .         '.        ",
        "           /        /               \\           \\       ",
        "          /        /                 \\           \\      ",
        "         ;        |                  |           ;      ",
        "         |        |                  |           |      ",
        "         |        |   .----------.   |           |      ",
        "         |        |   |          |   |           |      ",
        "         |        |   |  1897    |   |           |      ",
        "         |        |   '----------'   |           |      ",
        "         |        |                  |           |      ",
        "         |        |                  |           |      ",
        "         ;        |                  |           ;      ",
        "          \\       |   . . . . . .    |          /       ",
        "           \\      '-----._____.-----'         /         ",
        "            '.            .  .              .'          ",
        "              '-._      .      .        _.-'            ",
        "                  '----.__________.-----'               ",
        "                    _/    /  \\    \\_                    ",
        "                  _/_____/____\\_____\\_                  ",
        "                 /   .     ||     .   \\                 ",
        "                /  .    *  ||  .     . \\                ",
        "               |      .    ||    *      |               ",
        "               |  *        ||       .   |               ",
        "                \\__________||__________/                ",
        "                 \\         ||         /                 ",
        "                  '--------''--------'                  "
    };

    const std::vector<std::string> spritePebble = {
        "    _----------_,                 ",
        "    ,\"__         _-:,             ",
        "   /    \"\"--_--\"\"...:\\            ",
        "  /         |.........\\           ",
        " /          |..........\\          ",
        "/,         _'_........./:         ",
        "! -,    _-\"   \"-_... ,;;:         ",
        "\\   -_-\"         \"-_/;;;;         ",
        " \\   \\             /;;;;'         ",
        "  \\   \\           /;;;;           ",
        "   '.  \\         /;;;'            ",
        "     \"-_\\_______/;;'              "
    };

    struct Monster {
        float x = 12.5f;
        float y = 12.5f;
        float speed = 1.8f;
        bool isChasing = false;
        float animTimer = 0.0f;
        int currentFrame = 0;
        float enragedTimer = 0.0f; 
        
        float investigateX = 0.0f;
        float investigateY = 0.0f;
        float investigateTimer = 0.0f;

        const std::vector<std::string> frame0 = {
            "                                                                   ",
            "                                                         .:-::....         .::.                                          ",
            "                              ..::----===========+++====+++++++:                                         ",
            "                            :-==++*##***+====. :=+*##%%%%######*#*+                                      ",
            "                       .::-==+#@@@@@@@@@#+-     :%@@@@@@@@@@%%#####* .                                   ",
            "                       -==+*%@@@@@@@@@@@@%-      =@@@@@@@@@@@@@@@%%**=.                                   ",
            "                       -+*#@@@@@@@@@@@@@@#       -@@@@@@@@@@@@@@%#*++=:.                                  ",
            "                        :=#@@@@@@@@@@@@#.       :#@@@@@@@@@@@@%*+-:.::-.                                 ",
            "                           =#@@@@@@@@%+.        .+-:+%@@@@@*-.     ..:-                                  ",
            "                              :======:           ==.  .              ..:                                 ",
            "                                                 :+:                 .:-                                 ",
            "                                                 .:.               ..--* .                               ",
            "                                        ::.   :=*#+#+              .:-+*                                 ",
            "                                       :=*#=:--+%@@%+             .:-+**=                                ",
            "                                             :-#@%%#*=:           .:=+###                                ",
            "                                                 :+*+:      ......:--+*%%%-                               ",
            "                                        .-+-.   :**:   .:...::-::::=+#%%%#                               ",
            "                                      -+%%==*#%%%@@%#*=---::--=----=*#%%%@                               ",
            "                                   .=+@- :=+ :*:-###@@%***=:=======+*%@@%@                               ",
            "                                  -*#-          :-=%@@@###*-++++=++*#%@@%*                               ",
            "                         .       :%%  .:*@%%@%==%-=+ @@@*#%*++++++*#%@@@%                                ",
            "                        .        #@@@@@@@@@@@@@@@@@@@@@#=%%%+++++*%@@@@%.                                ",
            "                         ..     :@@@@@@@@@@@@@@@@@@@@@@%-#%%#+**#%@@@@@#                                 ",
            "                         ..     +@@@@@@@@@@@@@@@@@@@@@@@-#%%%#*#%@@@@@@                                  ",
            "                   .      .     *@@@@@@@@@@@@@@@@@@@@@@@:%@@%%#%%@@@@@                                   ",
            "                     -    .     +@@@@@@%@@@@@@@@@@@@@@@@:@%@%%%%@%@%#                                    ",
            "                          .     .@@@@%#@@@@@@@@@@@@@@@@+-@%%%%%@@%=                                      ",
            "                                 @@@@@@@@@@@@@@@@@@@@@@ +@%%#=#@@                                        ",
            "                                  @@@@@+*@@@@@@@@@@@@%: #%%%+=%@                                         ",
            "                                  . =+*@@@@@@@@@@@% =  .%%#%=+%                                          ",
            "                                         -*%*+.   +=*  *@%%%--                                           ",
            "                                      : -+==+++++*%   *@%%@*                                             ",
            "                                 .=:      .=*+-:    -%@%%                                                ",
            "                                   :**=:        -+%@@+                                                   ",
            "                                        .-+**##=                                                         "
        };

        const std::vector<std::string> frame1 = {
            "                                                                   ",
            "                                                         .::-::....         .::.                                          ",
            "                              ..::----===========+++====+++++++:                                         ",
            "                            :-==++*##***+====. :=+*##%%%%######*#*+                                      ",
            "                       .::-==+#@@@@@@@@@#+-     :%@@@@@@@@@@%%#####* .                                   ",
            "                       -==+*%@@@@@@@@@@@@%-      =@@@@@@@@@@@@@@@%%**=.                                   ",
            "                       -+*#@@@@@@@@@@@@@@#       -@@@@@@@@@@@@@@%#*++=:.                                  ",
            "                        :=#@@@@@@@@@@@@#.       :#@@@@@@@@@@@@%*+-:.::-.                                 ",
            "                           =#@@@@@@@@%+.        .+-:+%@@@@@*-.     ..:-                                  ",
            "                              :======:           ==.  .              ..:                                 ",
            "                                                 :+:                 .:-                                 ",
            "                                                 .:.               ..--* .                               ",
            "                                        ::.   :=*#+#+              .:-+*                                 ",
            "                                       :=*#=:--+%@@%+             .:-+**=                                ",
            "                                             :-#@%%#*=:           .:=+###                                ",
            "                                                 :+*+:      ......::-+*%%%-                               ",
            "                                        .-+-.   :**:   .:...::-::::=+#%%%#                               ",
            "                                      -+%%==*#%%%@@%#*=---::--=----=*#%%%@                               ",
            "                                   .=+@- :=+ :*:-###@@%***=:=======+*%@@%@                               ",
            "                                  -*#-          :-=%@@@###*-++++=++*#%@@%*                               ",
            "                         .       :%%  .:*@%%@%==%-=+ @@@*#%*++++++*#%@@@%                                ",
            "                        .        #@@@@@@@@@@@@@@@@@@@@@#=%%%+++++*%@@@@%.                                ",
            "                         ..     :@@@@@@@@@@@@@@@@@@@@@@%-#%%#+**#%@@@@@#                                 ",
            "                         ..     +@@@@@@@@@@@@@@@@@@@@@@@-#%%%#*#%@@@@@@                                  ",
            "                   .      .     *@@@@@@@@@@@@@@@@@@@@@@@:%@@%%#%%@@@@@                                   ",
            "                     -    .     +@@@@@@%@@@@@@@@@@@@@@@@:@%@%%%%@%@%#                                    ",
            "                          .     .@@@@%#@@@@@@@@@@@@@@@@+-@%%%%%@@%=                                      ",
            "                                 @@@@@@@@@@@@@@@@@@@@@@ +@%%#=#@@                                        ",
            "                                  @@@@@+*@@@@@@@@@@@@%: #%%%+=%@                                         ",
            "                                  . =+*@@@@@@@@@@@% =  .%%#%=+%                                          ",
            "                                         -*%*+.   +=*  *@%%%--                                           ",
            "                                      : -+==+++++*%   *@%%@*                                             ",
            "                                 .=:      .=*+-:    -%@%%                                                ",
            "                                   :**=:        -+%@@+                                                   ",
            "                                        .-+**##=                                                         "
        };
    } stalker;

    std::string getCurrentThemeName() {
        int theme = (currentLevel - 1) % 4;
        if (theme == 0) return "STANDARD MAZE";
        if (theme == 1) return "MANSION";
        if (theme == 2) return "HEDGE MAZE";
        return "COURTYARD";
    }

    void updateWindowScale() {
        if (window) {
            int targetW = RESOLUTION_PRESETS[currentResIndex].width;
            int targetH = RESOLUTION_PRESETS[currentResIndex].height;
            SDL_SetWindowSize(window, targetW, targetH);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
    }

    void setCaptureMouse(bool capture) {
        SDL_SetRelativeMouseMode(capture ? SDL_TRUE : SDL_FALSE);
    }

    // Moves the stalker toward (dx,dy)*speed, resolving X and Y separately
    // (like the player's own movement) so it can slide along a wall instead
    // of freezing whenever the single diagonal target cell happens to be
    // blocked. Indices are clamped so this is always a safe array access
    // regardless of how far stalker.x/y have drifted.
    void moveStalkerToward(float dx, float dy, float dtSec) {
        float moveX = dx * stalker.speed * dtSec;
        float moveY = dy * stalker.speed * dtSec;

        int curTileX = std::clamp(int(stalker.x), 0, MAP_W - 1);
        int curTileY = std::clamp(int(stalker.y), 0, MAP_H - 1);
        int nextTileX = std::clamp(int(stalker.x + moveX), 0, MAP_W - 1);
        int nextTileY = std::clamp(int(stalker.y + moveY), 0, MAP_H - 1);

        if (worldMap[curTileY][nextTileX].wallType == 0) stalker.x += moveX;
        if (worldMap[nextTileY][curTileX].wallType == 0) stalker.y += moveY;
    }

    uint32_t getElevationColor(float /*elevation*/, float dist, int side) {
        uint32_t cBright, cMid, cDark;
        float activeCorruption = (player.toxicTimer > 0.0f) ? 0.9f : corruptionLevel;

        if (activeCorruption >= 0.85f) {
            cBright = CORRUPT_BRIGHT; cMid = CORRUPT_MID; cDark = CORRUPT_DARK;
        } else {
            int theme = (currentLevel - 1) % 4;
            if (theme == 0)      { cBright = THEME0_BRIGHT; cMid = THEME0_MID; cDark = THEME0_DARK; }
            else if (theme == 1) { cBright = THEME1_BRIGHT; cMid = THEME1_MID; cDark = THEME1_DARK; }
            else if (theme == 2) { cBright = THEME2_BRIGHT; cMid = THEME2_MID; cDark = THEME2_DARK; }
            else                 { cBright = THEME3_BRIGHT; cMid = THEME3_MID; cDark = THEME3_DARK; }
        }

        if (activeCorruption >= 0.4f && (rand() % 100) < int(activeCorruption * 5)) return CORRUPT_BRIGHT;
        if (dist < 3.0f)       return (side == 0) ? cBright : cMid;
        else if (dist < 6.5f)  return (side == 0) ? cMid : cDark;
        else                   return cDark;
    }

    void generateProceduralMultiLevelMaze() {
        srand(static_cast<unsigned int>(time(nullptr)) + currentLevel * 1337);
        itemsInWorld.clear();
        activePebble.active = false;

        for (int r = 0; r < MAP_H; ++r) {
            for (int c = 0; c < MAP_W; ++c) {
                worldMap[r][c].wallType = 1;
                worldMap[r][c].floorH = 0.0f;
                worldMap[r][c].ceilH = 2.0f;
                worldMap[r][c].isStairs = false;
            }
        }

        std::stack<Point> stack;
        startPos = { 1, 1 };
        worldMap[startPos.y][startPos.x].wallType = 0;
        stack.push(startPos);

        const int dx[4] = { 0, 0, 2, -2 };
        const int dy[4] = { -2, 2, 0, 0 };

        while (!stack.empty()) {
            Point curr = stack.top();
            std::vector<int> dirs;

            for (int i = 0; i < 4; ++i) {
                int nx = curr.x + dx[i];
                int ny = curr.y + dy[i];
                if (nx > 0 && nx < MAP_W - 1 && ny > 0 && ny < MAP_H - 1) {
                    if (worldMap[ny][nx].wallType == 1) dirs.push_back(i);
                }
            }

            if (!dirs.empty()) {
                int d = dirs[rand() % dirs.size()];
                worldMap[curr.y + dy[d] / 2][curr.x + dx[d] / 2].wallType = 0;
                worldMap[curr.y + dy[d]][curr.x + dx[d]].wallType = 0;
                stack.push({ curr.x + dx[d], curr.y + dy[d] });
            } else {
                stack.pop();
            }
        }

        int highZoneX = (rand() % 2 == 0) ? (MAP_W / 2) : 1;
        int highZoneY = (rand() % 2 == 0) ? (MAP_H / 2) : 1;
        int zoneW = MAP_W / 2;
        int zoneH = MAP_H / 2;

        for (int y = highZoneY; y < highZoneY + zoneH; ++y) {
            for (int x = highZoneX; x < highZoneX + zoneW; ++x) {
                if (worldMap[y][x].wallType == 0) {
                    worldMap[y][x].floorH = 1.0f;
                    worldMap[y][x].ceilH = 3.2f;
                }
            }
        }

        for (int y = 1; y < MAP_H - 1; ++y) {
            for (int x = 1; x < MAP_W - 1; ++x) {
                if (worldMap[y][x].wallType == 0 && worldMap[y][x].floorH == 1.0f) {
                    const int nx[4] = { 1, -1, 0, 0 };
                    const int ny[4] = { 0, 0, 1, -1 };
                    for (int i = 0; i < 4; ++i) {
                        int adjX = x + nx[i];
                        int adjY = y + ny[i];
                        if (worldMap[adjY][adjX].wallType == 0 && worldMap[adjY][adjX].floorH == 0.0f) {
                            worldMap[y][x].floorH = 0.5f;
                            worldMap[y][x].isStairs = true;
                            break; 
                        }
                    }
                }
            }
        }

        std::vector<Point> potentialVents;
        std::vector<Point> emptyFloorSpaces;
        for (int y = 1; y < MAP_H - 1; ++y) {
            for (int x = 1; x < MAP_W - 1; ++x) {
                if (worldMap[y][x].wallType == 1) {
                    bool horiz = (worldMap[y][x-1].wallType == 0 && worldMap[y][x+1].wallType == 0 
                                  && worldMap[y][x-1].floorH < 0.5f && worldMap[y][x+1].floorH < 0.5f);
                    bool vert  = (worldMap[y-1][x].wallType == 0 && worldMap[y+1][x].wallType == 0 
                                  && worldMap[y-1][x].floorH < 0.5f && worldMap[y+1][x].floorH < 0.5f);
                    
                    if (horiz || vert) potentialVents.push_back({x, y});
                }
                if (worldMap[y][x].wallType == 0) emptyFloorSpaces.push_back({x, y});
            }
        }

        int numVents = 1 + (rand() % 2); 
        while (numVents > 0 && !potentialVents.empty()) {
            int idx = rand() % potentialVents.size();
            Point v = potentialVents[idx];
            worldMap[v.y][v.x].wallType = 3;
            worldMap[v.y][v.x].floorH = 0.0f; 
            worldMap[v.y][v.x].ceilH = 2.0f;
            potentialVents[idx] = potentialVents.back();
            potentialVents.pop_back();
            numVents--;
        }

        int numBread = 2 + (rand() % 3);
        int numMeds = 1 + (rand() % 2);
        int numPebbles = 3 + (rand() % 3);

        for (int i = 0; i < numBread && !emptyFloorSpaces.empty(); ++i) {
            int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
            itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, ITEM_BREAD});
            emptyFloorSpaces[idx] = emptyFloorSpaces.back(); emptyFloorSpaces.pop_back();
        }
        for (int i = 0; i < numMeds && !emptyFloorSpaces.empty(); ++i) {
            int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
            itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, ITEM_MEDS});
            emptyFloorSpaces[idx] = emptyFloorSpaces.back(); emptyFloorSpaces.pop_back();
        }
        for (int i = 0; i < numPebbles && !emptyFloorSpaces.empty(); ++i) {
            int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
            itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, ITEM_PEBBLE});
            emptyFloorSpaces[idx] = emptyFloorSpaces.back(); emptyFloorSpaces.pop_back();
        }

        endPos = { MAP_W - 2, MAP_H - 2 };
        worldMap[endPos.y][endPos.x].wallType = 2;

        player.posX = startPos.x + 0.5f;
        player.posY = startPos.y + 0.5f;
        player.posZ = 0.0f;
        player.targetPosZ = 0.0f;
        player.pitch = 0.0f;
        player.dirX = 1.0f;
        player.dirY = 0.0f;
        player.planeX = 0.0f;
        player.planeY = 0.66f;
        player.stepAccumulator = 0.0f;
        player.toxicTimer = 0.0f;

        stalker.x = MAP_W / 2 + 0.5f;
        stalker.y = MAP_H / 2 + 0.5f;
        stalker.isChasing = false;
        stalker.enragedTimer = 0.0f;
        stalker.investigateTimer = 0.0f;
    }

    void startNewGame() {
        currentLevel = 1;
        totalSteps = 0;
        levelTime = 0.0f;
        corruptionLevel = 0.0f; 
        player.sanity = 100.0f;
        player.health = 100.0f;
        
        for (int i = 0; i < 3; ++i) player.inventory[i] = ITEM_NONE;

        generateProceduralMultiLevelMaze();
        currentState = STATE_PLAYING;
        setCaptureMouse(true);

        SDL_LockAudioDevice(audioDevice);
        audioState.inGame = true;
        audioState.isJumpscare = false;
        audioState.sanity = 100.0f;
        audioState.monsterDist = 20.0f;
        audioState.corruption = corruptionLevel;
        SDL_UnlockAudioDevice(audioDevice);
    }

    void nextLevel() {
        currentLevel++;
        player.sanity = std::min(100.0f, player.sanity + 30.0f);
        player.health = std::min(100.0f, player.health + 30.0f);
        corruptionLevel = std::min(1.0f, (currentLevel - 1) * 0.06f);
        generateProceduralMultiLevelMaze();
        currentState = STATE_PLAYING;
        setCaptureMouse(true);

        SDL_LockAudioDevice(audioDevice);
        audioState.inGame = true;
        audioState.isJumpscare = false;
        audioState.corruption = corruptionLevel;
        SDL_UnlockAudioDevice(audioDevice);
    }

    void drawGlyphStandard(int col, int row, char c, uint32_t fgColor) {
        unsigned char uc = static_cast<unsigned char>(c); if (uc < 32 || uc > 127) return;
        const uint8_t* glyph = FONT_8X8[uc - 32];
        int startX = col * 8;
        int startY = row * 8;

        for (int y = 0; y < 8; ++y) {
            int drawY = startY + y;
            if (drawY < 0 || drawY >= NATIVE_HEIGHT) continue; 
            for (int x = 0; x < 8; ++x) {
                int drawX = startX + x;
                if (drawX < 0 || drawX >= NATIVE_WIDTH) continue; 
                if ((glyph[y] >> (7 - x)) & 1) {
                    pixelBuffer[drawY * NATIVE_WIDTH + drawX] = fgColor;
                }
            }
        }
    }

    void drawTextStandard(int col, int row, const std::string& text, uint32_t color) {
        for (size_t i = 0; i < text.size(); ++i) {
            if (col + i < TOTAL_COLS) {
                drawGlyphStandard(col + i, row, text[i], color);
            }
        }
    }

    void drawGlyphFine(int col, int row, char c, uint32_t fgColor) {
        unsigned char uc = static_cast<unsigned char>(c); if (uc < 32 || uc > 127) return;
        const uint8_t* glyph = FONT_8X8[uc - 32];
        int startX = col * CHAR_W;
        int startY = row * CHAR_H;

        for (int y = 0; y < CHAR_H; ++y) {
            int drawY = startY + y;
            if (drawY < 0 || drawY >= NATIVE_HEIGHT) continue; 
            for (int x = 0; x < CHAR_W; ++x) {
                int drawX = startX + x;
                if (drawX < 0 || drawX >= NATIVE_WIDTH) continue; 
                
                int srcX = (x * 8) / CHAR_W;
                int srcY = (y * 8) / CHAR_H;
                if ((glyph[srcY] >> (7 - srcX)) & 1) {
                    pixelBuffer[drawY * NATIVE_WIDTH + drawX] = fgColor;
                }
            }
        }
    }

    void drawTextFine(int col, int row, const std::string& text, uint32_t color) {
        for (size_t i = 0; i < text.size(); ++i) {
            if (col + i < TOTAL_COLS) {
                drawGlyphFine(col + i, row, text[i], color);
            }
        }
    }

    void drawRectFilled(int startCol, int startRow, int numCols, int numRows, uint32_t color) {
        int x0 = startCol * CHAR_W;
        int y0 = startRow * CHAR_H;
        int w = numCols * CHAR_W;
        int h = numRows * CHAR_H;

        for (int y = y0; y < y0 + h; ++y) {
            if (y < 0 || y >= NATIVE_HEIGHT) continue;
            for (int x = x0; x < x0 + w; ++x) {
                if (x < 0 || x >= NATIVE_WIDTH) continue;
                pixelBuffer[y * NATIVE_WIDTH + x] = color;
            }
        }
    }

public:
    bool init() {
        srand(static_cast<unsigned int>(time(nullptr)));

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
            return false;
        }

        window = SDL_CreateWindow(
            "Walk ASCII 3D Horror",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            RESOLUTION_PRESETS[currentResIndex].width,
            RESOLUTION_PRESETS[currentResIndex].height,
            SDL_WINDOW_SHOWN
        );
        if (!window) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        }
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
            return false;
        }

        screenTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            NATIVE_WIDTH, NATIVE_HEIGHT
        );
        if (!screenTexture) {
            std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
            return false;
        }

        pixelBuffer.resize(NATIVE_WIDTH * NATIVE_HEIGHT, 0xFF000000);

        SDL_AudioSpec wantedSpec;
        SDL_zero(wantedSpec);
        wantedSpec.freq = AUDIO_SAMPLE_RATE;
        wantedSpec.format = AUDIO_S16SYS;
        wantedSpec.channels = 1;
        wantedSpec.samples = AUDIO_BUFFER_SIZE;
        wantedSpec.callback = audioCallback;
        wantedSpec.userdata = &audioState;

        audioDevice = SDL_OpenAudioDevice(nullptr, 0, &wantedSpec, nullptr, 0);
        if (audioDevice != 0) SDL_PauseAudioDevice(audioDevice, 0);

        isRunning = true;
        return true;
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) isRunning = false;

            if (currentState == STATE_PLAYING && event.type == SDL_MOUSEMOTION) {
                float rotAngle = event.motion.xrel * player.mouseSensitivity;

                float oldDirX = player.dirX;
                player.dirX = player.dirX * cos(rotAngle) - player.dirY * sin(rotAngle);
                player.dirY = oldDirX * sin(rotAngle) + player.dirY * cos(rotAngle);

                float oldPlaneX = player.planeX;
                player.planeX = player.planeX * cos(rotAngle) - player.planeY * sin(rotAngle);
                player.planeY = oldPlaneX * sin(rotAngle) + player.planeY * cos(rotAngle);

                player.pitch -= event.motion.yrel * 0.12f;
                player.pitch = std::clamp(player.pitch, -22.0f, 22.0f);
            }

            if (event.type == SDL_KEYDOWN) {
                if (currentState == STATE_TITLE) {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) menuCursor = (menuCursor - 1 + 3) % 3;
                    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) menuCursor = (menuCursor + 1) % 3;
                    
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                        if (menuCursor == 0) startNewGame();
                        else if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                        else if (menuCursor == 2) {
                            currentResIndex = (currentResIndex + 1) % RESOLUTION_PRESETS.size();
                            updateWindowScale();
                        }
                    }
                    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
                        if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                        if (menuCursor == 2) {
                            int count = RESOLUTION_PRESETS.size();
                            currentResIndex = (event.key.keysym.sym == SDLK_RIGHT)
                                ? ((currentResIndex + 1) % count)
                                : ((currentResIndex - 1 + count) % count);
                            updateWindowScale();
                        }
                    }
                }
                else if (currentState == STATE_PLAYING) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = STATE_PAUSED;
                        setCaptureMouse(false);
                        SDL_LockAudioDevice(audioDevice);
                        audioState.inGame = false;
                        SDL_UnlockAudioDevice(audioDevice);
                    }
                    else if (event.key.keysym.sym == SDLK_e) {
                        for (auto it = itemsInWorld.begin(); it != itemsInWorld.end(); ++it) {
                            if (std::hypot(player.posX - it->x, player.posY - it->y) < 1.5f) {
                                for (int i = 0; i < 3; ++i) {
                                    if (player.inventory[i] == ITEM_NONE) {
                                        player.inventory[i] = it->type;
                                        itemsInWorld.erase(it);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_3) {
                        int slot = event.key.keysym.sym - SDLK_1;
                        bool dropping = (SDL_GetModState() & KMOD_SHIFT);
                        
                        if (player.inventory[slot] != ITEM_NONE) {
                            if (dropping) {
                                itemsInWorld.push_back({player.posX, player.posY, player.inventory[slot]});
                                player.inventory[slot] = ITEM_NONE;
                            } else {
                                ItemType type = player.inventory[slot];
                                player.inventory[slot] = ITEM_NONE;
                                
                                if (type == ITEM_PEBBLE) {
                                    activePebble.x = player.posX;
                                    activePebble.y = player.posY;
                                    activePebble.z = player.posZ + player.eyeHeight;
                                    activePebble.vx = player.dirX * 6.0f;
                                    activePebble.vy = player.dirY * 6.0f;
                                    activePebble.vz = 2.5f + (player.pitch / 22.0f) * 2.0f;
                                    activePebble.active = true;
                                } else if (type == ITEM_BREAD) {
                                    player.health = std::min(100.0f, player.health + 40.0f);
                                    stalker.enragedTimer = 10.0f; 
                                } else if (type == ITEM_MEDS) {
                                    player.sanity = std::min(100.0f, player.sanity + 50.0f);
                                    player.health -= 15.0f;       
                                    player.takingDamage = true;
                                    if (rand() % 100 < 20) {
                                        player.toxicTimer = 0.8f;     
                                    }
                                }
                            }
                        }
                    }
                    else if (event.key.keysym.sym >= SDLK_F2 && event.key.keysym.sym <= SDLK_F10) {
                        currentLevel = event.key.keysym.sym - SDLK_F1 + 1;
                        corruptionLevel = std::min(1.0f, (currentLevel - 1) * 0.06f);
                        generateProceduralMultiLevelMaze();
                    }
                }
                else if (currentState == STATE_PAUSED) {
                    if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_r) {
                        currentState = STATE_PLAYING;
                        setCaptureMouse(true);
                        SDL_LockAudioDevice(audioDevice);
                        audioState.inGame = true;
                        SDL_UnlockAudioDevice(audioDevice);
                    }
                    else if (event.key.keysym.sym == SDLK_q) currentState = STATE_TITLE;
                }
                else if (currentState == STATE_SUCCESS || currentState == STATE_GAMEOVER) {
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                        if (currentState == STATE_SUCCESS) nextLevel(); else startNewGame();
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = STATE_TITLE;
                        setCaptureMouse(false);
                    }
                }
            }
        }

        if (currentState == STATE_PLAYING) {
            const uint8_t* state = SDL_GetKeyboardState(NULL);
            player.forward = 0;
            player.strafe = 0;

            if (state[SDL_SCANCODE_W]) player.forward += 1;
            if (state[SDL_SCANCODE_S]) player.forward -= 1;
            if (state[SDL_SCANCODE_A]) player.strafe -= 1; 
            if (state[SDL_SCANCODE_D]) player.strafe += 1; 

            player.isSprinting = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
            player.isCrouching = state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL];

            if (player.isCrouching) {
                player.moveSpeed = player.baseMoveSpeed * 0.45f;
                player.targetEyeHeight = 0.25f;
            } else if (player.isSprinting) {
                player.moveSpeed = player.baseMoveSpeed * 1.75f;
                player.targetEyeHeight = 0.50f;
            } else {
                player.moveSpeed = player.baseMoveSpeed;
                player.targetEyeHeight = 0.50f;
            }
        }
    }

    void update(double dt) {
        if (currentState == STATE_JUMPSCARE) {
            jumpscareTimer -= static_cast<float>(dt);
            if (jumpscareTimer <= 0.0f) {
                currentState = STATE_GAMEOVER;
                SDL_LockAudioDevice(audioDevice);
                audioState.isJumpscare = false;
                SDL_UnlockAudioDevice(audioDevice);
            }
            return;
        }

        if (currentState != STATE_PLAYING) return;

        float dtSec = static_cast<float>(dt);
        levelTime += dtSec;
        player.takingDamage = false;

        if (player.toxicTimer > 0.0f) player.toxicTimer -= dtSec;

        player.eyeHeight += (player.targetEyeHeight - player.eyeHeight) * 0.2f;

        if (player.forward != 0 || player.strafe != 0) {
            float forwardStep = player.forward * player.moveSpeed * dtSec;
            float strafeStep  = player.strafe  * (player.moveSpeed * 0.85f) * dtSec;

            float moveX = player.dirX * forwardStep - player.dirY * strafeStep;
            float moveY = player.dirY * forwardStep + player.dirX * strafeStep;

            float bufX = (moveX > 0) ? 0.32f : -0.32f;
            float bufY = (moveY > 0) ? 0.32f : -0.32f;

            float prevX = player.posX;
            float prevY = player.posY;

            int currTileX = int(player.posX);
            int currTileY = int(player.posY);
            float currFloor = worldMap[currTileY][currTileX].floorH;

            int nextTileX = std::clamp(int(player.posX + moveX + bufX), 0, MAP_W - 1);
            int nextTileY = std::clamp(int(player.posY + moveY + bufY), 0, MAP_H - 1);

            bool canMoveX = false;
            int typeX = worldMap[currTileY][nextTileX].wallType;
            if (typeX != 1 && (typeX != 3 || player.isCrouching)) {
                float hDiff = std::abs(worldMap[currTileY][nextTileX].floorH - currFloor);
                if (hDiff <= 1.1f) canMoveX = true;
            }

            bool canMoveY = false;
            int typeY = worldMap[nextTileY][currTileX].wallType;
            if (typeY != 1 && (typeY != 3 || player.isCrouching)) {
                float hDiff = std::abs(worldMap[nextTileY][currTileX].floorH - currFloor);
                if (hDiff <= 1.1f) canMoveY = true;
            }

            if (canMoveX) player.posX += moveX;
            if (canMoveY) player.posY += moveY;

            player.stepAccumulator += std::hypot(player.posX - prevX, player.posY - prevY);
            if (player.stepAccumulator >= 1.0f) {
                totalSteps++;
                player.stepAccumulator = 0.0f;
            }
        }

        if (worldMap[int(player.posY)][int(player.posX)].wallType == 3) {
            player.isCrouching = true;
            player.moveSpeed = player.baseMoveSpeed * 0.45f;
            player.targetEyeHeight = 0.25f;
        }

        int standingX = int(player.posX);
        int standingY = int(player.posY);
        player.targetPosZ = worldMap[standingY][standingX].floorH;
        player.posZ += (player.targetPosZ - player.posZ) * 0.25f;

        if (activePebble.active) {
            activePebble.x += activePebble.vx * dtSec;
            activePebble.y += activePebble.vy * dtSec;
            activePebble.vz -= 15.0f * dtSec; 
            activePebble.z += activePebble.vz * dtSec;

            int px = int(activePebble.x);
            int py = int(activePebble.y);
            
            if (px >= 0 && px < MAP_W && py >= 0 && py < MAP_H && worldMap[py][px].wallType == 1) {
                activePebble.vx = 0.0f; activePebble.vy = 0.0f; 
            }

            float distToProj = std::hypot(activePebble.x - stalker.x, activePebble.y - stalker.y);
            if (distToProj < 0.8f && activePebble.z < 2.0f) {
                stalker.enragedTimer = 5.0f; 
                activePebble.active = false;
            }

            float floorH = (px >= 0 && px < MAP_W && py >= 0 && py < MAP_H) ? worldMap[py][px].floorH : 0.0f;
            if (activePebble.z <= floorH && activePebble.active) {
                activePebble.active = false;
                itemsInWorld.push_back({activePebble.x, activePebble.y, ITEM_PEBBLE}); 
                
                if (stalker.enragedTimer <= 0.0f) {
                    stalker.investigateX = activePebble.x;
                    stalker.investigateY = activePebble.y;
                    stalker.investigateTimer = 5.0f; 
                }
            }
        }

        float distToMonster = std::hypot(player.posX - stalker.x, player.posY - stalker.y);

        bool playerMakingNoise = (player.forward != 0 || player.strafe != 0) && !player.isCrouching;
        float noiseRadius = player.isSprinting ? 14.0f : (playerMakingNoise ? 6.0f : 1.5f);
        bool noticedPlayer = (distToMonster < noiseRadius) && (corruptionLevel > 0.5f);

        if (stalker.enragedTimer > 0.0f) {
            stalker.enragedTimer -= dtSec;
            stalker.speed = 3.5f; 
            stalker.investigateTimer = 0.0f; 
        } else {
            stalker.speed = 1.8f; 
        }

        if (noticedPlayer || stalker.enragedTimer > 0.0f) {
            stalker.isChasing = true;
            stalker.investigateTimer = 0.0f;

            stalker.animTimer += dtSec;
            if (stalker.animTimer > 0.25f) { stalker.currentFrame = 1 - stalker.currentFrame; stalker.animTimer = 0.0f; }

            float dx = (player.posX - stalker.x) / distToMonster;
            float dy = (player.posY - stalker.y) / distToMonster;
            moveStalkerToward(dx, dy, dtSec);

            player.sanity -= (6.0f / std::max(1.0f, distToMonster)) * dtSec;
            if (distToMonster < 0.75f) {
                deathReason = "CAUGHT IN THE DARK BY THE ENTITY";
                currentState = STATE_JUMPSCARE;
                jumpscareTimer = 2.5f; 
                setCaptureMouse(false);
                SDL_LockAudioDevice(audioDevice);
                audioState.inGame = false; audioState.isJumpscare = true;
                SDL_UnlockAudioDevice(audioDevice);
                return;
            }
        } 
        else if (stalker.investigateTimer > 0.0f) {
            stalker.isChasing = false; 
            stalker.investigateTimer -= dtSec;
            
            float distToTarget = std::hypot(stalker.investigateX - stalker.x, stalker.investigateY - stalker.y);
            if (distToTarget > 0.5f) {
                stalker.animTimer += dtSec;
                if (stalker.animTimer > 0.25f) { stalker.currentFrame = 1 - stalker.currentFrame; stalker.animTimer = 0.0f; }

                float dx = (stalker.investigateX - stalker.x) / distToTarget;
                float dy = (stalker.investigateY - stalker.y) / distToTarget;
                moveStalkerToward(dx, dy, dtSec);
            }
        } 
        else {
            stalker.isChasing = false;
        }

        player.sanity = std::max(0.0f, player.sanity);
        player.health = std::max(0.0f, player.health);

        if (player.sanity <= 0.0f) {
            deathReason = "LOST TO THE TERROR (SANITY DEPLETED)";
            currentState = STATE_GAMEOVER;
            setCaptureMouse(false);
        } else if (player.health <= 0.0f) {
            deathReason = "SLAIN BY THE STALKER (HEALTH DEPLETED)";
            currentState = STATE_GAMEOVER;
            setCaptureMouse(false);
        } else if (standingX == endPos.x && standingY == endPos.y) {
            currentState = STATE_SUCCESS;
            setCaptureMouse(false);
        }

        if (currentState != STATE_PLAYING) {
            SDL_LockAudioDevice(audioDevice);
            audioState.inGame = false;
            SDL_UnlockAudioDevice(audioDevice);
            return;
        }

        SDL_LockAudioDevice(audioDevice);
        float activeCorr = (player.toxicTimer > 0.0f) ? 0.9f : corruptionLevel;
        audioState.sanity = player.sanity;
        audioState.monsterDist = distToMonster;
        audioState.isChasing = stalker.isChasing;
        audioState.corruption = activeCorr;
        audioState.isMoving = (player.forward != 0 || player.strafe != 0);
        audioState.isSprinting = player.isSprinting;
        audioState.isCrouching = player.isCrouching;
        SDL_UnlockAudioDevice(audioDevice);
    }

   void renderItems(const std::vector<float>& zBuffer) {
        int viewWidth = (currentDifficulty == DIFF_EASY) ? 68 : TOTAL_COLS;
        
        std::vector<std::pair<float, ItemEntity>> sortedItems;
        for(const auto& it : itemsInWorld) {
            float dist = std::pow(player.posX - it.x, 2) + std::pow(player.posY - it.y, 2);
            sortedItems.push_back({dist, it});
        }
        std::sort(sortedItems.begin(), sortedItems.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

        for(const auto& pair : sortedItems) {
            const ItemEntity& item = pair.second;
            float spriteX = item.x - player.posX;
            float spriteY = item.y - player.posY;

            float invDet = 1.0f / (player.planeX * player.dirY - player.dirX * player.planeY);
            float transformX = invDet * (player.dirY * spriteX - player.dirX * spriteY);
            float transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY);

            if (transformY <= 0.2f) continue;

            int screenX = int((viewWidth / 2) * (1.0f + transformX / transformY));
            float floorH = worldMap[int(item.y)][int(item.x)].floorH;
            float totalPlayerZ = player.posZ + player.eyeHeight;
            int horizon = int(ROWS / 2 + player.pitch);
            
            int screenY = horizon - int(((floorH - totalPlayerZ) * ROWS) / transformY);
            
            const std::vector<std::string>* activeSpritePtr = &spriteBread;
            if (item.type == ITEM_MEDS) activeSpritePtr = &spriteMeds;
            else if (item.type == ITEM_PEBBLE) activeSpritePtr = &spritePebble;

            const auto& activeSprite = *activeSpritePtr;
            int rowCount = activeSprite.size();
            int colCount = activeSprite[0].size();
            
            // ==========================================
            // TWEAK THIS NUMBER TO FIX THE STRETCHING
            // Lower number (e.g. 0.25f) = Thinner Item
            // Higher number (e.g. 0.60f) = Wider Item
            // ==========================================
            float aspectMultiplier = 0.35f; 
            
            float aspect = ((float)colCount / (float)rowCount) * aspectMultiplier;
            int spriteHeight = std::abs(int(ROWS / transformY / ((item.type == ITEM_PEBBLE) ? 4.0f : 2.5f))); 
            if (spriteHeight == 0) continue;
            
            int spriteWidth = int(spriteHeight * aspect);
            int drawStartY = screenY - spriteHeight;
            int drawEndY = screenY;
            int drawStartX = screenX - spriteWidth / 2;
            int drawEndX = screenX + spriteWidth / 2;

            uint32_t color = (item.type == ITEM_BREAD) ? 0xFFF59E0B : ((item.type == ITEM_MEDS) ? 0xFF06B6D4 : 0xFF94A3B8);
            int vOffset = (player.toxicTimer > 0.0f) ? (rand() % 5) - 2 : 0;

            for (int stripe = drawStartX; stripe < drawEndX; ++stripe) {
                if (stripe < 0 || stripe >= viewWidth || transformY > zBuffer[stripe]) continue;
                int texX = int((stripe - drawStartX) * colCount / spriteWidth);
                if (texX < 0 || texX >= colCount) continue;

                for (int y = drawStartY; y < drawEndY; ++y) {
                    if (y < 0 || y >= ROWS) continue;
                    int texY = int((y - drawStartY) * rowCount / spriteHeight);
                    if (texY < 0 || texY >= rowCount) continue;

                    char glyph = activeSprite[texY][texX];
                    if (glyph != ' ' && glyph != '.') {
                        drawRectFilled(stripe, y + vOffset, 1, 1, 0xFF000000);
                        drawGlyphFine(stripe, y + vOffset, glyph, color);
                    } else {
                        bool nearGlyph = false;
                        for (int dy = -1; dy <= 1; ++dy) {
                            for (int dx = -1; dx <= 1; ++dx) {
                                if (dx == 0 && dy == 0) continue;
                                int ny = texY + dy; int nx = texX + dx;
                                if (ny >= 0 && ny < rowCount && nx >= 0 && nx < colCount) {
                                    char n = activeSprite[ny][nx];
                                    if (n != ' ' && n != '.') { nearGlyph = true; break; }
                                }
                            }
                            if (nearGlyph) break;
                        }
                        if (nearGlyph) drawRectFilled(stripe, y + vOffset, 1, 1, 0xFF000000);
                    }
                }
            }
        }
        
        // Render Active Flying Projectile
        if (activePebble.active) {
            float spriteX = activePebble.x - player.posX;
            float spriteY = activePebble.y - player.posY;
            float invDet = 1.0f / (player.planeX * player.dirY - player.dirX * player.planeY);
            float transformX = invDet * (player.dirY * spriteX - player.dirX * spriteY);
            float transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY);

            if (transformY > 0.2f) {
                int screenX = int((viewWidth / 2) * (1.0f + transformX / transformY));
                float totalPlayerZ = player.posZ + player.eyeHeight;
                int horizon = int(ROWS / 2 + player.pitch);
                int screenY = horizon - int(((activePebble.z - totalPlayerZ) * ROWS) / transformY);
                if (screenX >= 0 && screenX < viewWidth && transformY <= zBuffer[screenX] && screenY >= 0 && screenY < ROWS) {
                    drawGlyphFine(screenX, screenY, 'o', 0xFF94A3B8);
                }
            }
        }
    }
    
    void renderStalkerSprite(const std::vector<float>& zBuffer) {
        float spriteX = stalker.x - player.posX;
        float spriteY = stalker.y - player.posY;

        float invDet = 1.0f / (player.planeX * player.dirY - player.dirX * player.planeY);
        float transformX = invDet * (player.dirY * spriteX - player.dirX * spriteY);
        float transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY);

        if (transformY <= 0.2f) return;

        int viewWidth = (currentDifficulty == DIFF_EASY) ? 68 : TOTAL_COLS;
        int screenX = int((viewWidth / 2) * (1.0f + transformX / transformY));

        int spriteHeight = std::abs(int(ROWS / transformY));
        int drawStartY = -spriteHeight / 2 + ROWS / 2 + int(player.pitch);
        int drawEndY = spriteHeight / 2 + ROWS / 2 + int(player.pitch);

        int spriteWidth = std::abs(int(ROWS / transformY * 1.5f));
        int drawStartX = -spriteWidth / 2 + screenX;
        int drawEndX = spriteWidth / 2 + screenX;

        const auto& currentSprite = (stalker.currentFrame == 0) ? stalker.frame0 : stalker.frame1;
        int rowCount = currentSprite.size();
        int colCount = currentSprite[0].size();

        for (int stripe = drawStartX; stripe < drawEndX; ++stripe) {
            if (stripe < 0 || stripe >= viewWidth || transformY > zBuffer[stripe]) continue;

            int texX = int((stripe - drawStartX) * colCount / spriteWidth);
            if (texX < 0 || texX >= colCount) continue;

            for (int y = drawStartY; y < drawEndY; ++y) {
                if (y < 0 || y >= ROWS) continue;

                int texY = int((y - drawStartY) * rowCount / (drawEndY - drawStartY));
                if (texY < 0 || texY >= rowCount) continue;

                char glyph = currentSprite[texY][texX];
                if (glyph != ' ' && glyph != '.') {
                    uint32_t color = (transformY < 3.0f) ? CORRUPT_BRIGHT : TIER_HIGH_BRIGHT;
                    drawGlyphFine(stripe, y, glyph, color);
                }
            }
        }
    }

    void renderJumpscareScreen() {
        const auto& currentSprite = stalker.frame0;
        int rowCount = currentSprite.size();
        int colCount = currentSprite[0].size();

        int centerXOffset = (TOTAL_COLS - colCount) / 2;
        int centerYOffset = (ROWS - rowCount) / 2;

        for (int y = 0; y < rowCount; ++y) {
            for (int x = 0; x < colCount; ++x) {
                int screenX = centerXOffset + x;
                int screenY = centerYOffset + y;

                if ((rand() % 100) < 5) screenX += (rand() % 5) - 2;

                if (screenX >= 0 && screenX < TOTAL_COLS && screenY >= 0 && screenY < ROWS) {
                    char glyph = currentSprite[y][x];
                    if (glyph != ' ' && glyph != '.') {
                        uint32_t flashCol = ((rand() % 2) == 0) ? RED_GOAL_BRIGHT : 0xFFFFFFFF;
                        drawGlyphFine(screenX, screenY, glyph, flashCol);
                    }
                }
            }
        }

        std::vector<std::string> creepyPhrases = {
            "I SAW YOU", "YOU CANT HIDE", "HE IS HERE", "NO ESCAPE", "LOOK AT ME", "DEATH AWAITS"
        };
        for (int i = 0; i < 5; ++i) {
            int rx = rand() % (TOTAL_COLS - 15);
            int ry = rand() % (ROWS - 2);
            drawTextFine(rx, ry, creepyPhrases[rand() % creepyPhrases.size()], RED_GOAL_BRIGHT);
        }
    }

    void render3DView() {
        int viewWidth = (currentDifficulty == DIFF_EASY) ? 68 : TOTAL_COLS;
        float totalPlayerZ = player.posZ + player.eyeHeight;
        int horizon = int(ROWS / 2 + player.pitch);
        
        std::vector<float> zBuffer(viewWidth, 1e30f);

        for (int col = 0; col < viewWidth; ++col) {
            float cameraX = 2.0f * col / float(viewWidth) - 1.0f;
            float rayDirX = player.dirX + player.planeX * cameraX;
            float rayDirY = player.dirY + player.planeY * cameraX;

            int mapX = int(player.posX);
            int mapY = int(player.posY);

            float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1.0f / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1.0f / rayDirY);
            float sideDistX, sideDistY, perpWallDist;
            int stepX, stepY, hit = 0, side = 0;

            if (rayDirX < 0) { stepX = -1; sideDistX = (player.posX - mapX) * deltaDistX; }
            else             { stepX =  1; sideDistX = (mapX + 1.0f - player.posX) * deltaDistX; }
            if (rayDirY < 0) { stepY = -1; sideDistY = (player.posY - mapY) * deltaDistY; }
            else             { stepY =  1; sideDistY = (mapY + 1.0f - player.posY) * deltaDistY; }

            float prevFloorH = worldMap[mapY][mapX].floorH;
            float stepRiserDist = -1.0f;
            float stepFloorDiff = 0.0f;
            bool hitStepRiser = false;
            float stepElevation = 0.0f;

            while (hit == 0) {
                if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
                else                       { sideDistY += deltaDistY; mapY += stepY; side = 1; }

                if (mapX >= 0 && mapX < MAP_W && mapY >= 0 && mapY < MAP_H) {
                    float currCellFloor = worldMap[mapY][mapX].floorH;
                    if (!hitStepRiser && worldMap[mapY][mapX].wallType == 0 && std::abs(currCellFloor - prevFloorH) > 0.1f) {
                        hitStepRiser = true;
                        stepFloorDiff = currCellFloor - prevFloorH;
                        stepRiserDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
                        stepElevation = currCellFloor;
                    }
                    prevFloorH = currCellFloor;

                    if (worldMap[mapY][mapX].wallType > 0) hit = worldMap[mapY][mapX].wallType;
                } else {
                    break;
                }
            }

            if (side == 0) perpWallDist = (sideDistX - deltaDistX);
            else           perpWallDist = (sideDistY - deltaDistY);
            if (perpWallDist < 0.05f) perpWallDist = 0.05f;

            zBuffer[col] = perpWallDist;

            float activeCorr = (player.toxicTimer > 0.0f) ? 0.9f : corruptionLevel;
            int vOffset = 0;
            if (activeCorr > 0.5f && (rand() % 100) < int(activeCorr * 20)) {
                vOffset = (rand() % 5) - 2; 
            }

            for (int r = horizon + 1; r < ROWS; ++r) {
                float p = r - horizon;
                float straightDist = (ROWS * totalPlayerZ) / p;
                float weight = straightDist / perpWallDist;

                float currentFloorX = weight * (player.posX + rayDirX * perpWallDist) + (1.0f - weight) * player.posX;
                float currentFloorY = weight * (player.posY + rayDirY * perpWallDist) + (1.0f - weight) * player.posY;

                int fTileX = int(currentFloorX);
                int fTileY = int(currentFloorY);

                if (fTileX >= 0 && fTileX < MAP_W && fTileY >= 0 && fTileY < MAP_H) {
                    float sampledFloorH = worldMap[fTileY][fTileX].floorH;
                    uint32_t floorColor = getElevationColor(sampledFloorH, straightDist, 0);

                    char floorGlyph = ' ';
                    
                    if (worldMap[fTileY][fTileX].isStairs) {
                        floorGlyph = (int(straightDist * 3.0f) % 2 == 0) ? '=' : '_';
                    } 
                    else if (sampledFloorH > 0.8f) {
                        bool isCheck = ((fTileX + fTileY) % 2 == 0);
                        floorGlyph = isCheck ? '#' : '-';
                        floorColor = isCheck ? TIER_HIGH_BRIGHT : TIER_HIGH_DARK;
                        if (activeCorr >= 0.85f) floorColor = isCheck ? CORRUPT_BRIGHT : CORRUPT_DARK;
                        if (straightDist > 6.0f && (col % 2 != 0)) floorGlyph = ' ';
                    } 
                    else {
                        if (straightDist < 8.0f && ((fTileX + fTileY) % 2 == 0) && (col % 2 == 0)) {
                            floorGlyph = '.';
                        }
                    }

                    if (activeCorr > 0.3f && floorGlyph != ' ' && (rand() % 100) < int(activeCorr * 10)) {
                        floorGlyph = "?!@#$%^&*"[rand() % 9];
                    }

                    if (floorGlyph != ' ') {
                        drawGlyphFine(col, r + vOffset, floorGlyph, floorColor);
                    }
                }
            }

            float baseFloor = (mapY >= 0 && mapY < MAP_H && mapX >= 0 && mapX < MAP_W) ? worldMap[mapY][mapX].floorH : 0.0f;
            float baseCeil  = (mapY >= 0 && mapY < MAP_H && mapX >= 0 && mapX < MAP_W) ? worldMap[mapY][mapX].ceilH : 2.0f;

            char wallGlyph = ' ';
            uint32_t wallColor;

            if (hit == 2) {
                wallColor = (side == 0) ? RED_GOAL_BRIGHT : RED_GOAL_DARK;
                wallGlyph = (perpWallDist <= 2.50f) ? '#' : '%';
            } 
            else if (hit == 3) {
                baseFloor += 0.5f;  
                wallColor = getElevationColor(baseFloor, perpWallDist + 2.0f, side); 
                wallGlyph = '#'; 
            }
            else {
                wallColor = getElevationColor(baseFloor, perpWallDist, side);
                if (perpWallDist <= 1.25f)      wallGlyph = '@';
                else if (perpWallDist <= 2.50f) wallGlyph = '#';
                else if (perpWallDist <= 4.00f) wallGlyph = '%';
                else if (perpWallDist <= 5.80f) wallGlyph = '*';
                else if (perpWallDist <= 7.50f) wallGlyph = '+';
                else if (perpWallDist <= 9.00f) wallGlyph = '-';
                else if (perpWallDist <= 11.0f) wallGlyph = '.';
            }

            if (activeCorr > 0.3f && wallGlyph != ' ' && hit != 3 && (rand() % 100) < int(activeCorr * 10)) {
                wallGlyph = "?!@#$%^&*"[rand() % 9];
            }

            int drawStart = horizon - int(((baseCeil - totalPlayerZ) * ROWS) / perpWallDist);
            int drawEnd   = horizon - int(((baseFloor - totalPlayerZ) * ROWS) / perpWallDist);

            for (int r = 0; r < ROWS; ++r) {
                if (r >= drawStart && r <= drawEnd && wallGlyph != ' ') {
                    char finalGlyph = wallGlyph;
                    if (hit == 3) {
                        finalGlyph = ((r + col) % 2 == 0) ? '\\' : '/';
                    }
                    drawGlyphFine(col, r + vOffset, finalGlyph, wallColor);
                }
            }

            if (hitStepRiser && stepRiserDist > 0.1f && stepRiserDist < perpWallDist) {
                float lowH = std::min(prevFloorH, prevFloorH + stepFloorDiff);
                float highH = std::max(prevFloorH, prevFloorH + stepFloorDiff);

                int stepTop = horizon - int(((highH - totalPlayerZ) * ROWS) / stepRiserDist);
                int stepBottom = horizon - int(((lowH - totalPlayerZ) * ROWS) / stepRiserDist);

                uint32_t stepColor = getElevationColor(stepElevation, stepRiserDist, 0);
                char stepGlyph = (stepFloorDiff > 0) ? '=' : 'v';

                for (int r = stepTop; r <= stepBottom; ++r) {
                    if (r >= 0 && r < ROWS) {
                        drawGlyphFine(col, r + vOffset, stepGlyph, stepColor);
                    }
                }
            }
        }

        renderItems(zBuffer);

        if (corruptionLevel > 0.5f || stalker.enragedTimer > 0.0f) {
            renderStalkerSprite(zBuffer);
        }

        int cx = viewWidth / 2;
        int cy = horizon;
        drawGlyphFine(cx, cy, '+', 0xFF94A3B8);

        if (player.takingDamage) {
            drawRectFilled(34, 27, 16, 3, 0xFF050505);
            drawTextFine(36, 28, "! ATTACKED !", RED_GOAL_BRIGHT);
        }

        drawRectFilled(1, 1, 52, 13, 0xFF050505); 

        std::string elevStr;
        uint32_t elevColor;
        if (player.posZ > 0.7f) {
            elevStr = "OVERPASS [HIGH]";
            elevColor = TIER_HIGH_BRIGHT;
        } else if (player.posZ > 0.2f) {
            elevStr = "STAIRS [MID]";
            elevColor = TIER_MID_BRIGHT;
        } else {
            elevStr = "GROUND [LOW]";
            elevColor = TIER_LOW_BRIGHT;
        }

        std::string themeName = getCurrentThemeName();
        drawTextFine(2, 2, "AREA: " + themeName + " | LVL: " + std::to_string(currentLevel), elevColor);
        
        if (corruptionLevel < 0.5f && player.toxicTimer <= 0.0f) {
            drawTextFine(2, 4, "TEST MAZE UTILITY v1.0", TIER_LOW_BRIGHT);
            drawTextFine(2, 6, "SYS: CLEAN", TIER_LOW_BRIGHT);
        } else {
            uint32_t hpCol = (player.health < 30.0f) ? RED_GOAL_BRIGHT : ((player.health < 60.0f) ? 0xFFF59E0B : TIER_HIGH_BRIGHT);
            drawTextFine(2, 4, "SYS ERR: HEALTH: " + std::to_string(int(player.health)) + "%", hpCol);

            uint32_t sanCol = (player.sanity < 30.0f) ? RED_GOAL_BRIGHT : ((player.sanity < 60.0f) ? 0xFFF59E0B : TIER_HIGH_BRIGHT);
            drawTextFine(2, 6, "SYS ERR: SANITY: " + std::to_string(int(player.sanity)) + "%", sanCol);
        }

        drawTextFine(2, 8, "L-SHIFT: Sprint | L-CTRL: Crouch", TIER_MID_BRIGHT);

        auto getItemName = [](ItemType type) -> std::string {
            if (type == ITEM_BREAD) return "BREAD";
            if (type == ITEM_MEDS)  return "MEDS ";
            if (type == ITEM_PEBBLE) return "PEBBL";
            return "---- ";
        };
        
        drawTextFine(2, 10, "INV: [1] " + getItemName(player.inventory[0]) + " [2] " + getItemName(player.inventory[1]) + " [3] " + getItemName(player.inventory[2]), 0xFF94A3B8);
        drawTextFine(2, 12, "[E] Pick Up | [1-3] Use/Throw | [SHIFT+1-3] Drop", TIER_MID_BRIGHT);

        if (currentDifficulty == DIFF_EASY) {
            renderSidebarMinimap();
        }
    }

    void renderSidebarMinimap() {
        for (int r = 0; r < ROWS; ++r) drawGlyphFine(68, r, '|', 0xFF334155);

        int miniStartX = 72;
        int miniStartY = 3;

        for (int r = 0; r < MAP_H; ++r) {
            for (int c = 0; c < MAP_W; ++c) {
                char mapCh = ' ';
                uint32_t mapCol = 0xFF1E293B;

                if (r == startPos.y && c == startPos.x) { mapCh = 'S'; mapCol = TIER_HIGH_BRIGHT; } 
                else if (r == endPos.y && c == endPos.x) { mapCh = 'E'; mapCol = RED_GOAL_BRIGHT; } 
                else if (worldMap[r][c].wallType == 1) { mapCh = '#'; mapCol = 0xFF475569; } 
                else if (worldMap[r][c].wallType == 3) { mapCh = 'X'; mapCol = 0xFFF59E0B; } 
                else if (worldMap[r][c].isStairs) { mapCh = '='; mapCol = TIER_MID_BRIGHT; } 
                else if (worldMap[r][c].floorH > 0.7f) { mapCh = '^'; mapCol = TIER_HIGH_BRIGHT; } 

                drawGlyphFine(miniStartX + c, miniStartY + r, mapCh, mapCol);
            }
        }

        for (const auto& it : itemsInWorld) {
            if (it.x >= 0 && it.x < MAP_W && it.y >= 0 && it.y < MAP_H) {
                char ch = (it.type == ITEM_BREAD) ? 'B' : ((it.type == ITEM_MEDS) ? '+' : 'o');
                uint32_t color = (it.type == ITEM_BREAD) ? 0xFFF59E0B : ((it.type == ITEM_MEDS) ? 0xFF06B6D4 : 0xFF94A3B8);
                drawGlyphFine(miniStartX + int(it.x), miniStartY + int(it.y), ch, color);
            }
        }

        int pMapX = int(player.posX);
        int pMapY = int(player.posY);
        int lookAheadX = int(player.posX + player.dirX * 1.2f);
        int lookAheadY = int(player.posY + player.dirY * 1.2f);
        if (lookAheadX >= 0 && lookAheadX < MAP_W && lookAheadY >= 0 && lookAheadY < MAP_H) {
            drawGlyphFine(miniStartX + lookAheadX, miniStartY + lookAheadY, '^', 0xFFF59E0B);
        }
        drawGlyphFine(miniStartX + pMapX, miniStartY + pMapY, 'O', 0xFF38BDF8);

        if (stalker.isChasing || stalker.investigateTimer > 0.0f) {
            int mX = int(stalker.x);
            int mY = int(stalker.y);
            if (mX >= 0 && mX < MAP_W && mY >= 0 && mY < MAP_H) {
                char gChar = "!@#$%&*X?"[rand() % 9];
                uint32_t mCol = stalker.enragedTimer > 0.0f ? RED_GOAL_BRIGHT : (stalker.isChasing ? 0xFFBE123C : 0xFFF59E0B);
                drawGlyphFine(miniStartX + mX, miniStartY + mY, gChar, mCol);
            }
        }

        drawTextFine(72, 32, "MODE: EASY (MINIMAP)", 0xFF94A3B8);
        drawTextFine(72, 34, "[=] Stairs (Mid)", TIER_MID_BRIGHT);
        drawTextFine(72, 36, "[^] Overpass (High)", TIER_HIGH_BRIGHT);
        drawTextFine(72, 38, "[X] Crawlspace Grate", 0xFFF59E0B);
        drawTextFine(72, 40, "[S] Start  [E] End", 0xFF64748B);
    }

    void renderTitleScreen() {
        drawTextStandard(34, 12, "==============================", TIER_HIGH_BRIGHT);
        drawTextStandard(34, 14, "     WALK ASCII 3D HORROR     ", TIER_HIGH_BRIGHT);
        drawTextStandard(34, 16, "==============================", TIER_HIGH_BRIGHT);

        std::string diffStr = (currentDifficulty == DIFF_NORMAL) ? "NORMAL (NO MINIMAP)" : "EASY (WITH MINIMAP)";
        std::string resStr = RESOLUTION_PRESETS[currentResIndex].label;

        std::string options[3] = { "START GAME", "DIFFICULTY: " + diffStr, "RESOLUTION: " + resStr };

        for (int i = 0; i < 3; ++i) {
            uint32_t col = (i == menuCursor) ? TIER_HIGH_BRIGHT : 0xFF64748B;
            std::string prefix = (i == menuCursor) ? "-> " : "   ";
            drawTextStandard(32, 24 + i * 4, prefix + options[i], col);
        }
        drawTextStandard(26, 44, "UP/DOWN: SELECT | LEFT/RIGHT: CHANGE | ENTER: START", 0xFF334155);
    }

    void renderPauseScreen() {
        drawRectFilled(30, 18, 40, 24, 0xEE050505);
        drawTextFine(38, 22, "========================", TIER_MID_BRIGHT);
        drawTextFine(38, 24, "      GAME PAUSED       ", TIER_MID_BRIGHT);
        drawTextFine(38, 26, "========================", TIER_MID_BRIGHT);

        drawTextFine(35, 32, "[R / ESC] RESUME GAME", TIER_HIGH_BRIGHT);
        drawTextFine(35, 36, "[Q] QUIT TO TITLE", RED_GOAL_BRIGHT);
        drawTextFine(34, 40, "DEV: [F2-F10] SET LEVEL", 0xFF64748B);
    }

    void renderSuccessScreen() {
        drawTextStandard(36, 12, "****************************", TIER_HIGH_BRIGHT);
        drawTextStandard(36, 14, "      MAZE COMPLETED!       ", TIER_HIGH_BRIGHT);
        drawTextStandard(36, 16, "****************************", TIER_HIGH_BRIGHT);

        drawTextStandard(34, 22, "COMPLETED LEVEL:  " + std::to_string(currentLevel), 0xFFFFFFFF);
        drawTextStandard(34, 25, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFFFFFFF);
        drawTextStandard(34, 28, "TIME TAKEN:       " + std::to_string(int(levelTime)) + " SECONDS", 0xFFFFFFFF);
        
        if (corruptionLevel >= 0.5f) {
            drawTextStandard(34, 31, "REMAINING HEALTH: " + std::to_string(int(player.health)) + "%", TIER_HIGH_BRIGHT);
            drawTextStandard(34, 34, "REMAINING SANITY: " + std::to_string(int(player.sanity)) + "%", TIER_HIGH_BRIGHT);
        }

        drawTextStandard(28, 44, "PRESS [ENTER / SPACE] TO ADVANCE TO NEXT LEVEL", TIER_LOW_BRIGHT);
        drawTextStandard(38, 47, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);
    }

    void renderGameOverScreen() {
        drawTextStandard(36, 10, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);
        drawTextStandard(36, 12, "         GAME OVER          ", RED_GOAL_BRIGHT);
        drawTextStandard(36, 14, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);

        drawTextStandard(28, 20, deathReason, RED_GOAL_BRIGHT);

        drawTextStandard(34, 26, "DIED AT LEVEL:    " + std::to_string(currentLevel), 0xFFCBD5E1);
        drawTextStandard(34, 29, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFCBD5E1);
        drawTextStandard(34, 32, "SURVIVED TIME:    " + std::to_string(int(levelTime)) + " SECONDS", 0xFFCBD5E1);

        drawTextStandard(32, 42, "PRESS [ENTER / SPACE] TO TRY AGAIN", TIER_HIGH_BRIGHT);
        drawTextStandard(38, 45, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);
    }

    void render() {
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), 0xFF080C14);

        if (currentState == STATE_TITLE) renderTitleScreen();
        else if (currentState == STATE_PLAYING) render3DView();
        else if (currentState == STATE_PAUSED) {
            render3DView();
            renderPauseScreen();
        }
        else if (currentState == STATE_JUMPSCARE) {
            renderJumpscareScreen();
        }
        else if (currentState == STATE_SUCCESS) renderSuccessScreen();
        else if (currentState == STATE_GAMEOVER) renderGameOverScreen();

        SDL_UpdateTexture(screenTexture, nullptr, pixelBuffer.data(), NATIVE_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    void run() {
        uint32_t previousTime = SDL_GetTicks();
        double lag = 0.0;

        while (isRunning) {
            uint32_t currentTime = SDL_GetTicks();
            lag += static_cast<double>(currentTime - previousTime);
            previousTime = currentTime;

            handleEvents();

            while (lag >= FIXED_TIMESTEP) {
                update(FIXED_TIMESTEP / 1000.0);
                lag -= FIXED_TIMESTEP;
            }

            render();
            SDL_Delay(1);
        }
    }

    void cleanup() {
        setCaptureMouse(false);
        if (audioDevice != 0) SDL_CloseAudioDevice(audioDevice);
        if (screenTexture) SDL_DestroyTexture(screenTexture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    WalkAsciiElevationEngine engine;
    if (engine.init()) engine.run();
    engine.cleanup();
    return 0;
}