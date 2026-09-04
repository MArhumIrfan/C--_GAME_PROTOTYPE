#pragma once

#include <cstdint> // <-- ADDED THIS to define uint32_t

// Game Constants
constexpr int CHAR_W = 8;
constexpr int CHAR_H = 8;
constexpr int TOTAL_COLS = 133;
constexpr int ROWS = 80;
constexpr int NATIVE_WIDTH = TOTAL_COLS * CHAR_W;
constexpr int NATIVE_HEIGHT = ROWS * CHAR_H;
constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;
constexpr int MAP_W = 27;
constexpr int MAP_H = 27;

// Gameplay & Balance Constants
constexpr float SPRINT_SPEED_MULTIPLIER = 1.75f;
constexpr float CROUCH_SPEED_MULTIPLIER = 0.45f;
constexpr float PLAYER_BASE_SPEED = 3.2f;
constexpr float LANTERN_FUEL_MAX = 100.0f;
constexpr float LANTERN_FUEL_CONSUMPTION_RATE = 0.5f; // Per second
constexpr float OIL_TIN_RESTORE_AMOUNT = 50.0f;
constexpr float CRAWLSPACE_HEAL_RATE = 2.0f; // HP per second
constexpr float SANITY_DRAIN_RATE = 6.0f;
constexpr float MIST_DAMAGE_RATE = 6.0f;
constexpr float STALKER_WATCHER_SPEED = 1.0f;
constexpr float STALKER_HUNTER_SPEED = 1.9f;
constexpr float STALKER_LUNGE_SPEED = 4.0f;

// Color Palettes
constexpr uint32_t THEME_INTRO_BRIGHT = 0xFF22D3EE;
constexpr uint32_t THEME_INTRO_MID    = 0xFF0891B2;
constexpr uint32_t THEME_INTRO_DARK   = 0xFF155E75;

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

constexpr uint32_t TIER_HIGH_BRIGHT = 0xFF86EFAC;
constexpr uint32_t TIER_MID_BRIGHT  = 0xFF38BDF8;
constexpr uint32_t TIER_LOW_BRIGHT  = 0xFF16A34A;

constexpr uint32_t CORRUPT_BRIGHT   = 0xFFF43F5E;
constexpr uint32_t CORRUPT_MID      = 0xFFBE123C;
constexpr uint32_t CORRUPT_DARK     = 0xFF881337;

constexpr uint32_t RED_GOAL_BRIGHT  = 0xFFF43F5E;
constexpr uint32_t RED_GOAL_DARK    = 0xFFBE123C;

// Audio
constexpr int AUDIO_SAMPLE_RATE = 44100;
constexpr int AUDIO_BUFFER_SIZE = 1024;