#pragma once

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
#include <queue>

#include "GameTypes.h"
#include "AudioState.h"
#include "Player.h"
#include "Enemy.h"

// ==========================================
// MAIN ENGINE CLASS
// ==========================================
// Declaration only - method bodies live in Game_Init.cpp, Game_Update.cpp,
// Game_Render.cpp, Game_UI.cpp, and Game_Loop.cpp.
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
    std::vector<Projectile> activeProjectiles; 

    int currentLevel = 1;
    int totalSteps = 0;
    float levelTime = 0.0f;
    float corruptionLevel = 0.0f; 
    std::string deathReason = "";
    float jumpscareTimer = 0.0f;

        Player player;

        Enemy stalker, mistEnemy, statue;

    

    

    
    
    

    
    
    
    

    

        void initializeSprites();

    struct AStarNode {
        int x, y;
        int g, h;
        AStarNode* parent;
        int f() const { return g + h; }
    };

        std::vector<Point> findPath(Point start, Point end);

    
        bool hasLineOfSight(float x1, float y1, float x2, float y2);

        std::string getCurrentThemeName();

        void updateWindowScale();

        void setCaptureMouse(bool capture);

        void moveEnemyToward(Enemy& e, float targetX, float targetY, float dtSec);

        uint32_t applyShadow(uint32_t hexColor, float brightness);

        float calculateVisibility(int col, int row, float dist, int viewWidth, float pitch);

    
        float getVignette(int col, int row, int viewWidth, float sanity, float damageShake);

        bool isPixelVisible(int col, int row, float dist, int viewWidth, float pitch);

        uint32_t getWallColor(float dist, int side);

        bool isMapVisible(float mapX, float mapY);

    
        std::string scramble(std::string text, float sanity);

        void generateProceduralMultiLevelMaze();

        void startNewGame();

        void nextLevel();

        void drawGlyphStandard(int col, int row, char c, uint32_t fgColor);

        void drawTextStandard(int col, int row, const std::string& text, uint32_t color);

        void drawGlyphFine(int col, int row, char c, uint32_t fgColor);

        void drawTextFine(int col, int row, const std::string& text, uint32_t color);

        void drawRectFilled(int startCol, int startRow, int numCols, int numRows, uint32_t color);

public:
        bool init();

        void handleEvents();

        void update(double dt);

        void render3DView();

        void renderItems(const std::vector<float>& zBuffer);

        void renderEnemySprite(const std::vector<float>& zBuffer, const Enemy& e, const std::vector<std::string>& f0, const std::vector<std::string>& f1, float heightMultiplier);

       void renderJumpscareScreen();

    
        void renderSidebarMinimap();

        void renderTitleScreen();

        void renderPauseScreen();

        void renderSuccessScreen();

        void renderGameOverScreen();

        void render();

        void run();

        void cleanup();
};
