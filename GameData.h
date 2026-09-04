#pragma once
#include <string>
#include <vector>
#include "Constants.h" // <-- ADD THIS LINE

// Enums
enum GameState { STATE_TITLE, STATE_PLAYING, STATE_PAUSED, STATE_JUMPSCARE, STATE_SUCCESS, STATE_GAMEOVER, STATE_MESSAGE };
enum Difficulty { DIFF_NORMAL = 0, DIFF_EASY = 1 };
enum ItemType { ITEM_NONE, ITEM_BREAD, ITEM_MEDS, ITEM_PEBBLE, ITEM_OIL };
enum AIState { AI_STATE_IDLE, AI_STATE_WATCHING, AI_STATE_HUNTING, AI_STATE_LUNGING, AI_STATE_INVESTIGATING };

// Structs
struct ResolutionPreset { int width; int height; std::string label; };
struct Point {
    int x, y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};
struct MapCell { int wallType = 0; };
struct ItemEntity { float x, y; ItemType type; };
struct Projectile { float x, y, z, vx, vy, vz; ItemType type; bool active = false; };
struct Ghost { bool active = false; float x, y, timer; };

// Global Constants that need the structs
const std::vector<ResolutionPreset> RESOLUTION_PRESETS = {
    { 800,  480, "800x480 (1X)" }, { 1280, 720, "1280x720 (HD)" }, { 1366, 768, "1366x768 (WXGA)" },
    { 1600, 960, "1600x960 (2X)" }, { 1920, 1080, "1920x1080 (FHD)" }
};