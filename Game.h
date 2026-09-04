#pragma once

#include "Constants.h"
#include "GameData.h"
#include "AudioSystem.h"
#include "Player.h"
#include "Enemy.h"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Game {
public:
    Game();
    bool init();
    void run();
    void cleanup();

private:
    void handleEvents();
    void update(double dt);
    void render();

    // Game States
    void startNewGame();
    void nextLevel();
    void generateProceduralMultiLevelMaze();

    // Rendering
    void render3DView();
    void renderItems(const std::vector<float>& zBuffer);
    void renderEnemySprite(const std::vector<float>& zBuffer, const Enemy& e, const std::vector<std::string>& f0, const std::vector<std::string>& f1, float heightMultiplier);
    void renderSidebarMinimap();
    void renderUI();
    void renderTitleScreen();
    void renderPauseScreen();
    void renderSuccessScreen();
    void renderGameOverScreen();
    void renderMessageScreen();
    void renderJumpscareScreen();

    // Drawing
    void drawGlyphStandard(int col, int row, char c, uint32_t fgColor);
    void drawTextStandard(int col, int row, const std::string& text, uint32_t color);
    void drawGlyphFine(int col, int row, char c, uint32_t fgColor);
    void drawTextFine(int col, int row, const std::string& text, uint32_t color);
    void drawRectFilled(int startCol, int startRow, int numCols, int numRows, uint32_t color);

    // Helpers
    void updateWindowScale();
    void setCaptureMouse(bool capture);
    uint32_t applyShadow(uint32_t hexColor, float brightness);
    float calculateVisibility(int col, int row, float dist, int viewWidth);
    uint32_t getWallColor(float dist, int side);
    bool isMapVisible(float mapX, float mapY);
    bool hasLineOfSight(float x1, float y1, float x2, float y2);
    void initializeSprites();

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
    std::string currentMessage = "";
    
    // Game Objects
    AudioSystem audioSystem;
    Player player;
    Enemy stalker, mistEnemy;

    MapCell worldMap[MAP_H][MAP_W];
    Point startPos, endPos;

    std::vector<ItemEntity> itemsInWorld;
    std::vector<Projectile> activeProjectiles;
    Ghost ghost;

    int currentLevel = 1;
    int totalSteps = 0;
    float levelTime = 0.0f;
    float corruptionLevel = 0.0f;
    std::string deathReason = "";
    float jumpscareTimer = 0.0f;

    // Sprites
    std::vector<std::string> spriteStalker0, spriteStalker1, spriteMist0;
    const std::vector<std::string> spriteBread, spriteMeds, spritePebble, spriteOil;
};