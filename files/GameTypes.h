#pragma once

#include <cstdint>
#include <string>
#include <vector>

// ==========================================
// Display / world constants
// ==========================================
constexpr int CHAR_W = 8;
constexpr int CHAR_H = 8;
constexpr int TOTAL_COLS = 133;
constexpr int ROWS = 80;
constexpr int NATIVE_WIDTH = TOTAL_COLS * CHAR_W;
constexpr int NATIVE_HEIGHT = ROWS * CHAR_H;

constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;
constexpr int MAP_W = 27;
constexpr int MAP_H = 27;

// ==========================================
// Theme / palette constants
// ==========================================
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

// ==========================================
// Core enums
// ==========================================
enum GameState { STATE_TITLE, STATE_PLAYING, STATE_PAUSED, STATE_JUMPSCARE, STATE_SUCCESS, STATE_GAMEOVER };
enum Difficulty { DIFF_NORMAL = 0, DIFF_EASY = 1 };
enum ItemType { ITEM_NONE, ITEM_BREAD, ITEM_MEDS, ITEM_PEBBLE, ITEM_KEY };

// ==========================================
// Small POD types shared across the engine,
// Player, and Enemy
// ==========================================
struct Point {
    int x, y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

struct MapCell { int wallType = 0; float floorH = 0.0f; float ceilH = 2.0f; bool isStairs = false; };

struct ItemEntity {
    float x, y;
    ItemType type;
};

struct Projectile {
    float x, y, z;
    float vx, vy, vz;
    ItemType type;
    bool active = false;
};

struct ResolutionPreset {
    int width;
    int height;
    std::string label;
};

// `inline` (C++17) so this can be defined in a header without violating the
// one-definition rule across translation units.
inline const std::vector<ResolutionPreset> RESOLUTION_PRESETS = {
    { 800,  480, "800x480 (1X)" },
    { 1280, 720, "1280x720 (HD)" },
    { 1366, 768, "1366x768 (WXGA)" },
    { 1600, 960, "1600x960 (2X)" },
    { 1920, 1080, "1920x1080 (FHD)" }
};
