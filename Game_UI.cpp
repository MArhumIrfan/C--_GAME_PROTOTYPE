#include "Game.h"
#include "Font.h"
#include "Sprites.h"

#include <cmath>
#include <algorithm>
#include <cstdlib>

std::string WalkAsciiElevationEngine::getCurrentThemeName() {
    if (currentLevel <= 10) return "TUTORIAL";
    int theme = (currentLevel - 11) % 4;
    if (theme == 0) return "STANDARD MAZE";
    if (theme == 1) return "MANSION";
    if (theme == 2) return "HEDGE MAZE";
    return "COURTYARD";
}

std::string WalkAsciiElevationEngine::scramble(std::string text, float sanity) {
    if (sanity > 50.0f) return text;
    float glitchChance = (50.0f - sanity) * 0.5f;
    for(char& c : text) {
        if (c != ' ' && (visualRand() % 100) < glitchChance) c = "!@#$%^&*"[visualRand() % 8];
    }
    return text;
}

void WalkAsciiElevationEngine::renderJumpscareScreen() {
    // Swap art based on what killed you
    const auto& currentSprite = (deathReason == "DONT BLINK") ? spriteStatueJumpscare : spriteStalker0;

    int rowCount = currentSprite.size();
    int colCount = currentSprite[0].size();

    int centerXOffset = (TOTAL_COLS - colCount) / 2;
    int centerYOffset = (ROWS - rowCount) / 2;

    for (int y = 0; y < rowCount; ++y) {
        for (int x = 0; x < colCount; ++x) {
            int screenX = centerXOffset + x;
            int screenY = centerYOffset + y;

            // Violent screen shaking
            if ((visualRand() % 100) < 5) screenX += (visualRand() % 5) - 2;

            if (screenX >= 0 && screenX < TOTAL_COLS && screenY >= 0 && screenY < ROWS) {
                char glyph = currentSprite[y][x];
                if (glyph != ' ' && glyph != '.') {
                    uint32_t flashCol = ((visualRand() % 2) == 0) ? RED_GOAL_BRIGHT : 0xFFFFFFFF;
                    drawGlyphFine(screenX, screenY, glyph, flashCol);
                }
            }
        }
    }

    // Swap the flashing text phrases based on the enemy
    std::vector<std::string> creepyPhrases;
    if (deathReason == "DONT BLINK") {
        creepyPhrases = { "DONT BLINK", "OPEN YOUR EYES", "IT MOVED", "TOO SLOW", "STAY STILL", "WE ARE STONE" };
    } else {
        creepyPhrases = { "I SAW YOU", "YOU CANT HIDE", "HE IS HERE", "NO ESCAPE", "LOOK AT ME", "DEATH AWAITS" };
    }

    // Draw random flashing text around the screen
    for (int i = 0; i < 5; ++i) {
        int rx = visualRand() % (TOTAL_COLS - 15);
        int ry = visualRand() % (ROWS - 2);
        drawTextFine(rx, ry, creepyPhrases[visualRand() % creepyPhrases.size()], RED_GOAL_BRIGHT);
    }
}

void WalkAsciiElevationEngine::renderSidebarMinimap() {
    for (int r = 0; r < ROWS; ++r) drawGlyphFine(100, r, '|', 0xFF334155);

    int miniStartX = 104;
    int miniStartY = 3;

    for (int r = 0; r < MAP_H; ++r) {
        for (int c = 0; c < MAP_W; ++c) {
            if (!isMapVisible(c + 0.5f, r + 0.5f)) continue;

            char mapCh = ' ';
            uint32_t mapCol = 0xFF1E293B;

            if (r == startPos.y && c == startPos.x) { mapCh = 'S'; mapCol = TIER_HIGH_BRIGHT; } 
            else if (r == endPos.y && c == endPos.x) { mapCh = 'E'; mapCol = RED_GOAL_BRIGHT; } 
            else if (worldMap[r][c].wallType == 1) { mapCh = '#'; mapCol = 0xFF475569; } 
            else if (worldMap[r][c].wallType == 3) { mapCh = 'X'; mapCol = 0xFFF59E0B; } 
            else if (worldMap[r][c].wallType == 4) { mapCh = '+'; mapCol = 0xFF8B4513; } 
            else if (worldMap[r][c].wallType == 5) { mapCh = '\''; mapCol = 0xFF8B4513; } 
            else if (worldMap[r][c].wallType == 6) { mapCh = '='; mapCol = 0xFF94A3B8; } 

            drawGlyphFine(miniStartX + c, miniStartY + r, mapCh, mapCol);
        }
    }

    for (const auto& it : itemsInWorld) {
        if (it.x >= 0 && it.x < MAP_W && it.y >= 0 && it.y < MAP_H && isMapVisible(it.x, it.y)) {
            char ch = (it.type == ITEM_BREAD) ? 'B' : ((it.type == ITEM_MEDS) ? '+' : ((it.type == ITEM_PEBBLE) ? 'o' : 'K'));
            uint32_t color = (it.type == ITEM_BREAD) ? 0xFFF59E0B : ((it.type == ITEM_MEDS) ? 0xFF06B6D4 : ((it.type == ITEM_PEBBLE) ? 0xFF94A3B8 : 0xFFFDE047));
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

    if (stalker.active && stalker.mode > 0 && isMapVisible(stalker.x, stalker.y)) {
        char gChar = '!';
        uint32_t mCol = stalker.isChasing ? RED_GOAL_BRIGHT : 0xFFF59E0B;
        drawGlyphFine(miniStartX + int(stalker.x), miniStartY + int(stalker.y), gChar, mCol);
    }

    if (mistEnemy.active && isMapVisible(mistEnemy.x, mistEnemy.y)) {
        char gChar = '~';
        uint32_t mCol = 0xFF8B5CF6;
        drawGlyphFine(miniStartX + int(mistEnemy.x), miniStartY + int(mistEnemy.y), gChar, mCol);
    }

    drawTextFine(miniStartX, 32, "MODE: EASY (MINIMAP)", 0xFF94A3B8);
    drawTextFine(miniStartX, 34, "[X] Crawl Grate", 0xFFF59E0B);
    drawTextFine(miniStartX, 36, "[+] Door [=] Locker", 0xFF8B4513);
    drawTextFine(miniStartX, 38, "[S] Start [E] End", 0xFF64748B);
}

void WalkAsciiElevationEngine::renderTitleScreen() {
    drawTextStandard(34, 12, "==============================", TIER_HIGH_BRIGHT);
    drawTextStandard(34, 14, "     WALK ASCII 3D HORROR     ", TIER_HIGH_BRIGHT);
    drawTextStandard(34, 16, "==============================", TIER_HIGH_BRIGHT);

    std::string diffStr = (currentDifficulty == DIFF_NORMAL) ? "NORMAL (NO MINIMAP)" : "EASY (WITH MINIMAP)";
    std::string resStr = RESOLUTION_PRESETS[currentResIndex].label;

    auto renderSlider = [&](int y, const std::string& label, float value, bool selected) {
        std::string bar = "[";
        int barLength = 20;
        int filled = static_cast<int>(value * barLength);
        for (int i = 0; i < barLength; ++i) {
            bar += (i < filled) ? "=" : "-";
        }
        bar += "]";
        uint32_t col = selected ? TIER_HIGH_BRIGHT : 0xFF64748B;
        std::string prefix = selected ? "-> " : "   ";
        drawTextStandard(32, y, prefix + label, col);
        drawTextStandard(32 + label.length() + 4, y, bar, col);
    };

    std::string options[6] = { "START GAME", "DIFFICULTY: " + diffStr, "VOLUME", "MOUSE SENSITIVITY", "RESOLUTION: " + resStr, "QUIT GAME" };

    for (int i = 0; i < 6; ++i) {
        uint32_t col = (i == menuCursor) ? TIER_HIGH_BRIGHT : 0xFF64748B;
        std::string prefix = (i == menuCursor) ? "-> " : "   ";
        if (i == 2) {
            renderSlider(24 + i * 4, options[i] + " ", audioState.masterVolume, i == menuCursor);
        } else if (i == 3) {
             renderSlider(24 + i * 4, options[i] + " ", player.mouseSensitivity, i == menuCursor);
        }
        else {
            drawTextStandard(32, 24 + i * 4, prefix + options[i], col);
        }
    }
    drawTextStandard(26, 50, "UP/DOWN: SELECT | LEFT/RIGHT: CHANGE | ENTER: START", 0xFF334155);
}

void WalkAsciiElevationEngine::renderPauseScreen() {
    // Alpha-blended overlay baked into the pixel buffer (SDL_RenderCopy does
    // not blend, so 0xEE050505 wouldn't have any visible transparency).
    drawRectFilledBlended(30, 18, 40, 24, 0xFF050505, 0.85f);
    drawTextFine(38, 22, "========================", TIER_MID_BRIGHT);
    drawTextFine(38, 24, "      GAME PAUSED       ", TIER_MID_BRIGHT);
    drawTextFine(38, 26, "========================", TIER_MID_BRIGHT);

    drawTextFine(35, 32, "[R / ESC] RESUME GAME", TIER_HIGH_BRIGHT);
    drawTextFine(35, 36, "[Q] QUIT TO TITLE", RED_GOAL_BRIGHT);
    drawTextFine(34, 40, "DEV: [PgUp/PgDn] SET LEVEL", 0xFF64748B);
}

void WalkAsciiElevationEngine::renderSuccessScreen() {
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

void WalkAsciiElevationEngine::renderGameOverScreen() {
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