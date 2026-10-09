// The 3D raycasting view: walls, floor, sprites, and the low-level glyph/rect drawing primitives.

#include "Game.h"
#include "Font.h"
#include "Sprites.h"

#include <cmath>
#include <algorithm>
#include <cstdlib>

uint32_t WalkAsciiElevationEngine::applyShadow(uint32_t hexColor, float brightness) {
    brightness = std::clamp(brightness, 0.0f, 1.0f);
    uint32_t a = (hexColor >> 24) & 0xFF;
    uint32_t r = static_cast<uint32_t>(((hexColor >> 16) & 0xFF) * brightness);
    uint32_t g = static_cast<uint32_t>(((hexColor >> 8) & 0xFF) * brightness);
    uint32_t b = static_cast<uint32_t>((hexColor & 0xFF) * brightness);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

float WalkAsciiElevationEngine::calculateVisibility(int col, int row, float dist, int viewWidth, float pitch) {
    if (currentLevel <= 10) return 1.0f;
    if (player.inLocker) return (dist < 1.5f) ? 0.8f : 0.0f;

    float lightCenterY = (ROWS / 2.0f) - (pitch * 0.5f); 
    float cameraX = 2.0f * col / float(viewWidth) - 1.0f;
    float cameraY = 2.0f * (row - lightCenterY) / float(ROWS);
    float aspect = (float)viewWidth / (float)ROWS;
    float spotRadius = std::hypot(cameraX * aspect, cameraY);

    float sanityFactor = std::max(0.35f, player.sanity / 100.0f);

    if (player.lanternOn) {
        float maxSpot = 1.5f * sanityFactor;
        float maxDist = 14.0f * sanityFactor;

        if (spotRadius > maxSpot || dist > maxDist) return 0.0f;

        float edgeFade = std::clamp(1.0f - ((spotRadius - (0.6f * sanityFactor)) * (3.3f / sanityFactor)), 0.0f, 1.0f);
        float distFade = 1.0f / (1.0f + 0.15f * dist + 0.3f * dist * dist);
        distFade = std::clamp(distFade * 1.5f, 0.0f, 1.0f); 

        float flickerThresh = 0.05f + ((1.0f - sanityFactor) * 0.25f);
        float flicker = 1.0f - ((visualRand() % 100) / 100.0f) * flickerThresh;

        return edgeFade * distFade * flicker;
    } else {
        float blindRadius = 0.8f * sanityFactor;
        if (dist > blindRadius) return 0.0f;
        float distFade = std::clamp(1.0f - (dist / blindRadius), 0.0f, 1.0f);
        return distFade;
    }
}

float WalkAsciiElevationEngine::getVignette(int col, int row, int viewWidth, float sanity, float damageShake) {
    float nx = (float)(col - viewWidth / 2) / (viewWidth / 2);
    float ny = (float)(row - ROWS / 2) / (ROWS / 2);
    float dist = std::hypot(nx, ny);
    float intensity = 1.0f - (sanity / 100.0f);
    intensity = std::max(intensity, damageShake);

    if (dist < 0.4f) return 1.0f;
    float fade = 1.0f - ((dist - 0.4f) * 1.5f * intensity);
    return std::clamp(fade, 0.0f, 1.0f);
}

bool WalkAsciiElevationEngine::isPixelVisible(int col, int row, float dist, int viewWidth, float pitch) {
    return calculateVisibility(col, row, dist, viewWidth, pitch) > 0.02f;
}

uint32_t WalkAsciiElevationEngine::getWallColor(float dist, int side) {
    uint32_t cBright, cMid, cDark;
    float activeCorruption = (player.toxicTimer > 0.0f) ? 0.9f : corruptionLevel;

    if (activeCorruption >= 0.85f) {
        cBright = CORRUPT_BRIGHT; cMid = CORRUPT_MID; cDark = CORRUPT_DARK;
    } else if (currentLevel <= 10) {
        cBright = THEME_INTRO_BRIGHT; cMid = THEME_INTRO_MID; cDark = THEME_INTRO_DARK;
    }
    else {
        int theme = (currentLevel - 11) % 4;
        if (theme == 0)      { cBright = THEME0_BRIGHT; cMid = THEME0_MID; cDark = THEME0_DARK; }
        else if (theme == 1) { cBright = THEME1_BRIGHT; cMid = THEME1_MID; cDark = THEME1_DARK; }
        else if (theme == 2) { cBright = THEME2_BRIGHT; cMid = THEME2_MID; cDark = THEME2_DARK; }
        else                 { cBright = THEME3_BRIGHT; cMid = THEME3_MID; cDark = THEME3_DARK; }
    }

    if (activeCorruption >= 0.4f && (visualRand() % 100) < int(activeCorruption * 5)) return CORRUPT_BRIGHT;

    if (side == 0) { 
        if (dist < 3.0f) return cBright;
        if (dist < 6.5f) return cMid;
        return cDark;
    } else { 
        if (dist < 3.0f) return cMid;
        return cDark;
    }
}

// ==========================================
// Low-level glyph / rect primitives
// ==========================================

void WalkAsciiElevationEngine::drawGlyphStandard(int col, int row, char c, uint32_t fgColor) {
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

void WalkAsciiElevationEngine::drawTextStandard(int col, int row, const std::string& text, uint32_t color) {
    for (size_t i = 0; i < text.size(); ++i) {
        if (col + i < TOTAL_COLS) {
            drawGlyphStandard(col + i, row, text[i], color);
        }
    }
}

void WalkAsciiElevationEngine::drawGlyphFine(int col, int row, char c, uint32_t fgColor) {
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

void WalkAsciiElevationEngine::drawTextFine(int col, int row, const std::string& text, uint32_t color) {
    for (size_t i = 0; i < text.size(); ++i) {
        if (col + i < TOTAL_COLS) {
            drawGlyphFine(col + i, row, text[i], color);
        }
    }
}

void WalkAsciiElevationEngine::drawRectFilled(int startCol, int startRow, int numCols, int numRows, uint32_t color) {
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

void WalkAsciiElevationEngine::drawRectFilledBlended(int startCol, int startRow, int numCols, int numRows, uint32_t color, float alpha) {
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    int x0 = startCol * CHAR_W;
    int y0 = startRow * CHAR_H;
    int w = numCols * CHAR_W;
    int h = numRows * CHAR_H;

    uint32_t cr = (color >> 16) & 0xFF;
    uint32_t cg = (color >> 8) & 0xFF;
    uint32_t cb = color & 0xFF;

    for (int y = y0; y < y0 + h; ++y) {
        if (y < 0 || y >= NATIVE_HEIGHT) continue;
        for (int x = x0; x < x0 + w; ++x) {
            if (x < 0 || x >= NATIVE_WIDTH) continue;
            uint32_t& dst = pixelBuffer[y * NATIVE_WIDTH + x];
            uint32_t dr = (dst >> 16) & 0xFF;
            uint32_t dg = (dst >> 8) & 0xFF;
            uint32_t db = dst & 0xFF;
            uint32_t r = uint32_t(dr * (1.0f - alpha) + cr * alpha);
            uint32_t g = uint32_t(dg * (1.0f - alpha) + cg * alpha);
            uint32_t b = uint32_t(db * (1.0f - alpha) + cb * alpha);
            dst = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }
}

// ==========================================
// 3D view
// ==========================================

void WalkAsciiElevationEngine::render3DView() {
    int viewWidth = (currentDifficulty == DIFF_EASY) ? 100 : TOTAL_COLS;

    float bobOffset = 0.0f;
    if ((player.forward != 0 || player.strafe != 0) && !player.inLocker) {
        bobOffset = std::sin(levelTime * (player.isSprinting ? 12.0f : 8.0f)) * 0.05f;
        if (player.isCrouching) bobOffset *= 0.5f;
    }

    float shakeOffset = 0.0f;
    if (player.damageShake > 0.0f) {
        shakeOffset = std::sin(levelTime * 50.0f) * player.damageShake * 4.0f;
    }

    int horizon = int(ROWS / 2 + player.pitch + shakeOffset);
    float totalPlayerZ = player.posZ + player.eyeHeight + bobOffset;

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

        while (hit == 0) {
            if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
            else                       { sideDistY += deltaDistY; mapY += stepY; side = 1; }

            if (mapX >= 0 && mapX < MAP_W && mapY >= 0 && mapY < MAP_H) {
                int wType = worldMap[mapY][mapX].wallType;
                if (wType > 0 && wType != 5) hit = wType;
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
        if (activeCorr > 0.5f && (visualRand() % 100) < int(activeCorr * 20)) {
            vOffset = (visualRand() % 5) - 2; 
        }

        int camZOffset = int((totalPlayerZ - 0.5f) * ROWS / perpWallDist);
        int lineHeight = int(ROWS / perpWallDist);
        int drawStart = -lineHeight / 2 + horizon + camZOffset;
        int drawEnd = lineHeight / 2 + horizon + camZOffset;

        for (int r = horizon + 1; r < ROWS; ++r) {
            float p = r - horizon;

            float straightDist = (ROWS * totalPlayerZ) / p;

            float currentFloorX = player.posX + rayDirX * straightDist;
            float currentFloorY = player.posY + rayDirY * straightDist;

            int fTileX = std::clamp(int(currentFloorX), 0, MAP_W - 1);
            int fTileY = std::clamp(int(currentFloorY), 0, MAP_H - 1);

            float vis = calculateVisibility(col, r, straightDist, viewWidth, player.pitch);
            float vignetteMult = getVignette(col, r, viewWidth, player.sanity, player.damageShake);
            vis *= vignetteMult;
            if (vis <= 0.02f) continue;

            uint32_t floorColor = getWallColor(straightDist, 0);
            char floorGlyph = ' ';

            if (worldMap[fTileY][fTileX].wallType == 5) {
                floorGlyph = '=';
                floorColor = 0xFF8B4513; 
            } else if (straightDist < 8.0f && ((fTileX + fTileY) % 2 == 0) && (col % 2 == 0)) {
                floorGlyph = '.';
            }

            if (activeCorr > 0.3f && floorGlyph != ' ' && (visualRand() % 100) < int(activeCorr * 10)) {
                floorGlyph = "?!@#$%^&*"[visualRand() % 9];
            }

            if (floorGlyph != ' ') {
                uint32_t finalFloorColor = applyShadow(floorColor, vis);
                drawGlyphFine(col, r + vOffset, floorGlyph, finalFloorColor);
            }
        }

        char wallGlyph = ' ';
        uint32_t wallColor;

        if (hit == 2) {
            wallColor = (side == 0) ? RED_GOAL_BRIGHT : RED_GOAL_DARK;
            wallGlyph = (perpWallDist <= 2.50f) ? '#' : '%';
        } else if (hit == 3) {
            wallColor = getWallColor(perpWallDist + 2.0f, side); 
            wallGlyph = '#'; 
        } else if (hit == 4) { 
            wallColor = applyShadow(0xFF8B4513, 1.2f); 
            wallGlyph = (side == 1) ? '|' : '-';
        } else if (hit == 6) { 
            wallColor = applyShadow(0xFF475569, 1.5f); 
            wallGlyph = '='; 
        } else {
            wallColor = getWallColor(perpWallDist, side);
            if (side == 1) { 
                if (perpWallDist <= 2.50f) wallGlyph = ':';
                else if (perpWallDist <= 5.80f) wallGlyph = ';';
                else wallGlyph = '.';
            } else { 
                if (perpWallDist <= 1.25f)      wallGlyph = '@';
                else if (perpWallDist <= 2.50f) wallGlyph = '#';
                else if (perpWallDist <= 4.00f) wallGlyph = '%';
                else if (perpWallDist <= 5.80f) wallGlyph = '*';
                else if (perpWallDist <= 7.50f) wallGlyph = '+';
                else if (perpWallDist <= 9.00f) wallGlyph = '-';
                else if (perpWallDist <= 11.0f) wallGlyph = '.';
            }
        }

        if (activeCorr > 0.3f && wallGlyph != ' ' && hit != 3 && hit != 4 && hit != 6 && (visualRand() % 100) < int(activeCorr * 10)) {
            wallGlyph = "?!@#$%^&*"[visualRand() % 9];
        }

        float wallX;
        if (side == 0) wallX = player.posY + perpWallDist * rayDirY;
        else           wallX = player.posX + perpWallDist * rayDirX;
        wallX -= std::floor(wallX);

        float cornerShade = std::clamp(std::min(wallX, 1.0f - wallX) * 10.0f, 0.05f, 1.0f);
        bool isCorner = (wallX <= 0.035f || wallX >= 0.965f);

        for (int r = 0; r < ROWS; ++r) {
            if (r >= drawStart && r <= drawEnd && wallGlyph != ' ') {
                float vis = calculateVisibility(col, r, perpWallDist, viewWidth, player.pitch);
                float vignetteMult = getVignette(col, r, viewWidth, player.sanity, player.damageShake);
                vis *= vignetteMult;
                if (vis <= 0.02f) continue;

                char finalGlyph = wallGlyph;
                uint32_t finalColor = wallColor;

                if (hit == 4) {
                    float wallY = 0.5f;
                    if (drawEnd != drawStart) wallY = (float)(r - drawStart) / (float)(drawEnd - drawStart);

                    if (std::fmod(wallX * 4.0f, 1.0f) < 0.08f) {
                        finalColor = 0xFF110800; 
                        finalGlyph = '|';
                    } else {
                        finalColor = 0xFF8B4513; 
                        finalGlyph = '=';
                    }

                    float handleDist = std::hypot(wallX - 0.8f, (wallY - 0.5f) * 1.5f);
                    if (handleDist < 0.04f) {
                        finalColor = 0xFFDAA520; 
                        finalGlyph = 'O';
                    } else if (handleDist < 0.06f) {
                        finalColor = 0xFF050505; 
                        finalGlyph = 'O';
                    }
                } else if (hit == 3) {
                    finalGlyph = ((r + col) % 2 == 0) ? '\\' : '/';
                }

                if (isCorner && hit != 3 && hit != 2 && hit != 4 && hit != 6) {
                    drawRectFilled(col, r + vOffset, 1, 1, 0xFF050505); 
                    drawGlyphFine(col, r + vOffset, '|', 0xFF1E293B); 
                } else {
                    finalColor = applyShadow(finalColor, vis * cornerShade);
                    drawGlyphFine(col, r + vOffset, finalGlyph, finalColor);
                }
            }
        }
    }

    renderItems(zBuffer);

    if (stalker.active) renderEnemySprite(zBuffer, stalker, spriteStalker0, spriteStalker1, 0.85f);
    if (mistEnemy.active) renderEnemySprite(zBuffer, mistEnemy, spriteMist0, spriteMist1, 0.75f);
    if (statue.active) renderEnemySprite(zBuffer, statue, spriteStatue, spriteStatue, 0.80f);

    if (mistEnemy.active && !player.inLocker) {
        float distToMonster = std::hypot(player.posX - mistEnemy.x, player.posY - mistEnemy.y);
        float mistIntensity = std::clamp(1.0f - (distToMonster / 10.0f), 0.0f, 1.0f);

        if (mistIntensity > 0.0f) {
            int numParticles = int(mistIntensity * viewWidth * ROWS * 0.15f);
            for (int i = 0; i < numParticles; ++i) {
                int rx = visualRand() % viewWidth;
                int ry = visualRand() % ROWS;

                float wave = std::sin(levelTime * 2.0f + rx * 0.1f + ry * 0.05f);
                int waveY = ry + int(wave * 2.0f);

                if (waveY >= 0 && waveY < ROWS) {
                    char mChar = (visualRand() % 2 == 0) ? '~' : '-';
                    uint32_t mColor = (visualRand() % 2 == 0) ? 0xFF10B981 : 0xFF8B5CF6; 
                    drawGlyphFine(rx, waveY, mChar, applyShadow(mColor, mistIntensity * 0.8f));
                }
            }
        }
    }

    if (player.inLocker) {
        for (int r = 0; r < ROWS; ++r) {
            if (r % 4 != 0 && r % 4 != 1) { 
                drawRectFilled(0, r, viewWidth, 1, 0xFF020202); 
            }
        }
        drawRectFilled(0, 0, 10, ROWS, 0xFF020202); 
        drawRectFilled(viewWidth - 10, 0, 10, ROWS, 0xFF020202);
    }

    int cx = viewWidth / 2;
    int cy = horizon;
    drawGlyphFine(cx, cy, '+', 0xFF94A3B8);

    if (player.takingDamage) {
        drawRectFilled(34, 27, 16, 3, 0xFF050505);
        drawTextFine(36, 28, "! ATTACKED !", RED_GOAL_BRIGHT);
    }

    if (player.showLockedMessage > 0.0f) {
        drawRectFilled(28, 38, 30, 3, 0xFF050505);
        drawTextFine(30, 39, "HATCH LOCKED. FIND KEY.", RED_GOAL_BRIGHT);
    }

    drawRectFilled(1, 1, 52, 17, 0xFF050505); 

    std::string themeName = scramble(getCurrentThemeName(), player.sanity);
    drawTextFine(2, 2, scramble("AREA: ", player.sanity) + themeName + " | LVL: " + scramble(std::to_string(currentLevel), player.sanity), TIER_HIGH_BRIGHT);

    uint32_t hpCol = (player.health < 30.0f) ? RED_GOAL_BRIGHT : ((player.health < 60.0f) ? 0xFFF59E0B : TIER_HIGH_BRIGHT);
    drawTextFine(2, 4, scramble("HEALTH: ", player.sanity) + std::to_string(int(player.health)) + "%", hpCol);

    uint32_t sanCol = (player.sanity < 30.0f) ? RED_GOAL_BRIGHT : ((player.sanity < 60.0f) ? 0xFFF59E0B : TIER_HIGH_BRIGHT);
    drawTextFine(2, 6, scramble("SANITY: ", player.sanity) + std::to_string(int(player.sanity)) + "%", sanCol);

    std::string stamBar = "[";
    int filled = int((player.stamina / 100.0f) * 10);
    for(int i=0; i<10; i++) stamBar += (i < filled) ? "=" : " ";
    stamBar += "]";
    uint32_t stamCol = (player.stamina < 30.0f) ? RED_GOAL_BRIGHT : TIER_MID_BRIGHT;
    drawTextFine(2, 8, scramble("STAMINA: ", player.sanity) + stamBar, stamCol);

    drawTextFine(2, 10, scramble("L-SHIFT: Sprint | L-CTRL: Crouch", player.sanity), TIER_MID_BRIGHT);

    auto getItemName = [](ItemType type) -> std::string {
        if (type == ITEM_BREAD) return "BREAD";
        if (type == ITEM_MEDS)  return "MEDS ";
        if (type == ITEM_PEBBLE) return "PEBBL";
        if (type == ITEM_KEY) return "KEY  ";
        return "---- ";
    };

    drawTextFine(2, 12, scramble("INV: [1] " + getItemName(player.inventory[0]) + " [2] " + getItemName(player.inventory[1]) + " [3] " + getItemName(player.inventory[2]), player.sanity), 0xFF94A3B8);
    drawTextFine(2, 14, scramble("[E] Interact | [1-3] Use | [SHIFT+1-3] Throw", player.sanity), TIER_MID_BRIGHT);

    std::string lanStr = player.lanternBroken ? ("BROKEN (" + std::to_string(player.reigniteClicks) + "/3)") : (player.lanternOn ? "ON" : "OFF");
    std::string keyStr = (currentLevel >= 5) ? (" | KEY: " + std::string(player.hasKey ? "YES" : "NO")) : ""; 
    drawTextFine(2, 16, scramble("[F] Lantern: " + lanStr, player.sanity) + scramble(keyStr, player.sanity), (player.lanternBroken ? RED_GOAL_BRIGHT : (player.lanternOn ? TIER_HIGH_BRIGHT : 0xFF94A3B8)));

    if (currentDifficulty == DIFF_EASY) {
        renderSidebarMinimap();
    }
}

void WalkAsciiElevationEngine::renderItems(const std::vector<float>& zBuffer) {
    int viewWidth = (currentDifficulty == DIFF_EASY) ? 100 : TOTAL_COLS;

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

        const std::vector<std::string>* activeSpritePtr = &spriteBread;
        if (item.type == ITEM_MEDS) activeSpritePtr = &spriteMeds;
        else if (item.type == ITEM_PEBBLE) activeSpritePtr = &spritePebble;
        else if (item.type == ITEM_KEY) activeSpritePtr = &spriteKey;

        const auto& activeSprite = *activeSpritePtr;
        int rowCount = activeSprite.size();
        int colCount = activeSprite[0].size();

        float heightDivisor = (item.type == ITEM_PEBBLE || item.type == ITEM_KEY) ? 4.0f : 1.5f; 
        int spriteHeight = std::abs(int(ROWS / transformY / heightDivisor)); 
        if (spriteHeight == 0) continue;

        float aspectMultiplier = 0.50f; 
        float aspect = ((float)colCount / (float)rowCount) * aspectMultiplier;
        int spriteWidth = int(spriteHeight * aspect);

        float bobOffset = (player.forward != 0 || player.strafe != 0) && !player.inLocker ? std::sin(levelTime * (player.isSprinting ? 12.0f : 8.0f)) * 0.05f : 0.0f;
        float shakeOffset = (player.damageShake > 0.0f) ? std::sin(levelTime * 50.0f) * player.damageShake * 4.0f : 0.0f;

        int horizon = int(ROWS / 2 + player.pitch + shakeOffset);
        int floorScreenY = horizon + int(ROWS * (player.eyeHeight + bobOffset) / transformY);

        int drawStartY = floorScreenY - spriteHeight;
        int drawEndY = floorScreenY;
        int drawStartX = screenX - spriteWidth / 2;
        int drawEndX = screenX + spriteWidth / 2;

        uint32_t color = 0xFF94A3B8;
        if (item.type == ITEM_BREAD) color = 0xFFF59E0B;
        else if (item.type == ITEM_MEDS) color = 0xFF06B6D4;
        else if (item.type == ITEM_KEY) color = 0xFFFDE047;

        int vOffset = (player.toxicTimer > 0.0f) ? (visualRand() % 5) - 2 : 0;

        for (int stripe = drawStartX; stripe < drawEndX; ++stripe) {
            if (stripe < 0 || stripe >= viewWidth || transformY > zBuffer[stripe]) continue;
            int texX = int((float)(stripe - drawStartX) / spriteWidth * colCount);
            if (texX < 0 || texX >= colCount) continue;

            for (int y = drawStartY; y < drawEndY; ++y) {
                if (y < 0 || y >= ROWS) continue;

                float vis = calculateVisibility(stripe, y, transformY, viewWidth, player.pitch);
                float vignetteMult = getVignette(stripe, y, viewWidth, player.sanity, player.damageShake);
                vis *= vignetteMult;
                if (vis <= 0.02f) continue;

                int texY = int((float)(y - drawStartY) / spriteHeight * rowCount);
                if (texY < 0 || texY >= rowCount) continue;

                char glyph = activeSprite[texY][texX];

                if (glyph != ' ') {
                    drawRectFilled(stripe, y + vOffset, 1, 1, 0xFF000000);
                    drawGlyphFine(stripe, y + vOffset, glyph, applyShadow(color, vis));
                }
            }
        }
    }

    for (const auto& proj : activeProjectiles) {
        float spriteX = proj.x - player.posX;
        float spriteY = proj.y - player.posY;
        float invDet = 1.0f / (player.planeX * player.dirY - player.dirX * player.planeY);
        float transformX = invDet * (player.dirY * spriteX - player.dirX * spriteY);
        float transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY);

        if (transformY > 0.2f) {
            int screenX = int((viewWidth / 2) * (1.0f + transformX / transformY));

            float bobOffset = (player.forward != 0 || player.strafe != 0) && !player.inLocker ? std::sin(levelTime * (player.isSprinting ? 12.0f : 8.0f)) * 0.05f : 0.0f;
            float shakeOffset = (player.damageShake > 0.0f) ? std::sin(levelTime * 50.0f) * player.damageShake * 4.0f : 0.0f;

            int horizon = int(ROWS / 2 + player.pitch + shakeOffset);
            int screenY = horizon + int(ROWS * (player.eyeHeight + bobOffset - proj.z) / transformY);

            if (screenX >= 0 && screenX < viewWidth && transformY <= zBuffer[screenX] && screenY >= 0 && screenY < ROWS) {
                float vis = calculateVisibility(screenX, screenY, transformY, viewWidth, player.pitch);
                float vignetteMult = getVignette(screenX, screenY, viewWidth, player.sanity, player.damageShake);
                vis *= vignetteMult;
                if (vis > 0.02f) {
                    char c = (proj.type == ITEM_PEBBLE) ? 'o' : (proj.type == ITEM_BREAD ? 'B' : '+');
                    uint32_t col = (proj.type == ITEM_PEBBLE) ? 0xFF94A3B8 : (proj.type == ITEM_BREAD ? 0xFFF59E0B : 0xFF06B6D4);
                    drawGlyphFine(screenX, screenY, c, applyShadow(col, vis));
                }
            }
        }
    }
}

void WalkAsciiElevationEngine::renderEnemySprite(const std::vector<float>& zBuffer, const Enemy& e, const std::vector<std::string>& f0, const std::vector<std::string>& f1, float heightMultiplier) {
    float spriteX = e.x - player.posX;
    float spriteY = e.y - player.posY;

    float invDet = 1.0f / (player.planeX * player.dirY - player.dirX * player.planeY);
    float transformX = invDet * (player.dirY * spriteX - player.dirX * spriteY);
    float transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY);

    if (transformY <= 0.2f) return;

    int viewWidth = (currentDifficulty == DIFF_EASY) ? 100 : TOTAL_COLS;
    int screenX = int((viewWidth / 2) * (1.0f + transformX / transformY));

    float bobOffset = (player.forward != 0 || player.strafe != 0) && !player.inLocker ? std::sin(levelTime * (player.isSprinting ? 12.0f : 8.0f)) * 0.05f : 0.0f;
    float shakeOffset = (player.damageShake > 0.0f) ? std::sin(levelTime * 50.0f) * player.damageShake * 4.0f : 0.0f;

    int horizon = int(ROWS / 2 + player.pitch + shakeOffset);
    int floorScreenY = horizon + int(ROWS * (player.eyeHeight + bobOffset) / transformY);

    int spriteHeight = std::abs(int(ROWS / transformY * heightMultiplier));

    int drawStartY = floorScreenY - spriteHeight;
    int drawEndY = floorScreenY;

    const auto& currentSprite = (e.currentFrame == 0) ? f0 : f1;
    if (currentSprite.empty()) return;

    int rowCount = currentSprite.size();
    int colCount = currentSprite[0].size();

    float aspectMultiplier = 0.45f; 
    float aspect = ((float)colCount / (float)rowCount) * aspectMultiplier;
    int spriteWidth = int(spriteHeight * aspect);

    int drawStartX = screenX - spriteWidth / 2;
    int drawEndX = screenX + spriteWidth / 2;

    for (int stripe = drawStartX; stripe < drawEndX; ++stripe) {
        if (stripe < 0 || stripe >= viewWidth || transformY > zBuffer[stripe]) continue;

        int texX = int((float)(stripe - drawStartX) / spriteWidth * colCount);
        if (texX < 0 || texX >= colCount) continue;

        for (int y = drawStartY; y < drawEndY; ++y) {
            if (y < 0 || y >= ROWS) continue;

            float vis = calculateVisibility(stripe, y, transformY, viewWidth, player.pitch);
            float vignetteMult = getVignette(stripe, y, viewWidth, player.sanity, player.damageShake);
            vis *= vignetteMult;
            if (vis <= 0.02f) continue;

            int texY = int((float)(y - drawStartY) / spriteHeight * rowCount);
            if (texY < 0 || texY >= rowCount) continue;

            char glyph = currentSprite[texY][texX];
            if (glyph != ' ' && glyph != '.') {
                uint32_t color = (transformY < 3.0f) ? CORRUPT_BRIGHT : TIER_HIGH_BRIGHT;
                drawGlyphFine(stripe, y, glyph, applyShadow(color, vis));
            }
        }
    }
}

// ==========================================
// Master render
// ==========================================

void WalkAsciiElevationEngine::render() {
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