#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <string>

// Virtual Screen Setup: 80x60 text grid (at 8x8 font = 640x480 native resolution)
constexpr int CHAR_W = 8;
constexpr int CHAR_H = 8;
constexpr int COLS = 80;
constexpr int ROWS = 60;
constexpr int SCREEN_WIDTH = COLS * CHAR_W;  // 640
constexpr int SCREEN_HEIGHT = ROWS * CHAR_H; // 480
constexpr int SCALE = 2;                     // Window: 1280x960

constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;

// World Map (0 = Empty, 1 = Concrete Wall, 2 = Iron Door, 3 = Goal Bunker Exit)
constexpr int MAP_W = 24;
constexpr int MAP_H = 24;

const int WORLD_MAP[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,1,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,2,0,0,0,0,0,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,1,0,1,1,0,0,1,0,1},
    {1,0,1,1,1,2,1,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,2,1,1,1,1},
    {1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,1,0,0,1,0,0,1,0,1,1,1,1,1,0,0,1,0,1},
    {1,0,0,2,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,1},
    {1,0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,3,0,1,0,0,1,0,1},
    {1,0,0,1,1,1,1,0,0,1,1,1,1,0,1,0,0,0,1,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1},
    {1,0,1,1,0,1,0,0,1,0,1,1,1,0,0,1,0,0,1,0,1,1,0,1},
    {1,0,0,1,0,2,0,0,2,0,1,0,1,0,0,2,0,0,2,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,0,1,0,1,0,0,1},
    {1,0,1,1,0,1,1,1,1,0,1,1,1,0,0,1,1,1,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// ASCII Luminance Ramp
const char ASCII_RAMP[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const int RAMP_LEN = sizeof(ASCII_RAMP) - 1;

// 8x8 Standard Bitmap Font
const uint8_t FONT_8X8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // Space
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // !
    {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, // "
    {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // #
    {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, // $
    {0x00,0x66,0xAC,0xD8,0x36,0x6A,0x00,0x00}, // %
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, // &
    {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00}, // '
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // (
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // )
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // *
    {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00}, // +
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, // ,
    {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // -
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // .
    {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00}, // /
    {0x7C,0xC6,0xCE,0xD6,0xE6,0xC6,0x7C,0x00}, // 0
    {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, // 1
    {0x7C,0xC6,0x06,0x1C,0x30,0x66,0xFE,0x00}, // 2
    {0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00}, // 3
    {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00}, // 4
    {0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00}, // 5
    {0x7C,0xC6,0xC0,0xFC,0xC6,0xC6,0x7C,0x00}, // 6
    {0xFE,0x06,0x0C,0x18,0x30,0x30,0x30,0x00}, // 7
    {0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00}, // 8
    {0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00}, // 9
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // :
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}, // ;
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, // <
    {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, // =
    {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00}, // >
    {0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00}, // ?
    {0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00}, // @
    {0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, // A
    {0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00}, // B
    {0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00}, // C
    {0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00}, // D
    {0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00}, // E
    {0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00}, // F
    {0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00}, // G
    {0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, // H
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // I
    {0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00}, // J
    {0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00}, // K
    {0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00}, // L
    {0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00}, // M
    {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00}, // N
    {0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // O
    {0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00}, // P
    {0x7C,0xC6,0xC6,0xC6,0xC6,0xCE,0x7C,0x06}, // Q
    {0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE6,0x00}, // R
    {0x7C,0xC6,0x60,0x38,0x0C,0xC6,0x7C,0x00}, // S
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // T
    {0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // U
    {0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x00}, // V
    {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00}, // W
    {0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00}, // X
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}, // Y
    {0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00}  // Z
};

struct Monster {
    float x = 12.5f;
    float y = 12.5f;
    float speed = 1.6f;
    bool isChasing = false;
};

class AsciiHorrorEngine {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* screenTexture = nullptr;
    std::vector<uint32_t> pixelBuffer;
    bool isRunning = false;

    // Depth buffer for 3D sprite occlusion
    float zBuffer[COLS];

    int worldMap[MAP_H][MAP_W];

    struct Player {
        float posX = 1.5f;
        float posY = 1.5f;
        float dirX = 1.0f;
        float dirY = 0.0f;
        float planeX = 0.0f;
        float planeY = 0.66f;
        float moveSpeed = 2.8f;
        float rotSpeed = 2.4f;

        int forward = 0;
        int rotate = 0;
        int keysHeld = 1;
        float sanity = 100.0f;
        bool hasEscaped = false;
    } player;

    Monster stalker;

    void drawGlyph(int col, int row, char c, uint32_t fgColor) {
        if (c < 32 || c > 127) return;
        const uint8_t* glyph = FONT_8X8[c - 32];
        int startX = col * CHAR_W;
        int startY = row * CHAR_H;

        for (int y = 0; y < CHAR_H; ++y) {
            for (int x = 0; x < CHAR_W; ++x) {
                if ((glyph[y] >> (7 - x)) & 1) {
                    pixelBuffer[(startY + y) * SCREEN_WIDTH + (startX + x)] = fgColor;
                }
            }
        }
    }

    void drawText(int col, int row, const std::string& text, uint32_t color) {
        for (size_t i = 0; i < text.size(); ++i) {
            if (col + i < COLS) {
                drawGlyph(col + i, row, text[i], color);
            }
        }
    }

public:
    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return false;

        window = SDL_CreateWindow(
            "ASCII 3D HORROR: THE FACILITY",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE,
            SDL_WINDOW_SHOWN
        );

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        screenTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT
        );

        pixelBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000);

        for (int r = 0; r < MAP_H; ++r) {
            for (int c = 0; c < MAP_W; ++c) {
                worldMap[r][c] = WORLD_MAP[r][c];
            }
        }

        isRunning = true;
        return true;
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) isRunning = false;

            if (event.type == SDL_KEYDOWN) {
                // Interact / Unlock Doors ('F' or 'E')
                if (event.key.keysym.sym == SDLK_f || event.key.keysym.sym == SDLK_e) {
                    int frontX = int(player.posX + player.dirX * 1.2f);
                    int frontY = int(player.posY + player.dirY * 1.2f);

                    if (worldMap[frontY][frontX] == 2) { // Iron Door
                        if (player.keysHeld > 0) {
                            worldMap[frontY][frontX] = 0; // Unlock & Open
                            player.keysHeld--;
                        }
                    } else if (worldMap[frontY][frontX] == 3) { // Goal Bunker Exit
                        player.hasEscaped = true;
                    }
                }
            }
        }

        const uint8_t* state = SDL_GetKeyboardState(NULL);
        player.forward = 0;
        player.rotate = 0;

        if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])    player.forward += 1;
        if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])  player.forward -= 1;
        if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])  player.rotate += 1;
        if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) player.rotate -= 1;
    }

    void update(double dt) {
        if (player.hasEscaped) return;

        float dtSec = static_cast<float>(dt);

        // 1. Player Rotation
        if (player.rotate != 0) {
            float rot = player.rotate * player.rotSpeed * dtSec;
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(rot) - player.dirY * sin(rot);
            player.dirY = oldDirX * sin(rot) + player.dirY * cos(rot);

            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(rot) - player.planeY * sin(rot);
            player.planeY = oldPlaneX * sin(rot) + player.planeY * cos(rot);
        }

        // 2. Player Movement with Collision
        if (player.forward != 0) {
            float step = player.forward * player.moveSpeed * dtSec;
            float buf = (step > 0) ? 0.3f : -0.3f;

            if (worldMap[int(player.posY)][int(player.posX + player.dirX * (step + buf))] == 0)
                player.posX += player.dirX * step;
            if (worldMap[int(player.posY + player.dirY * (step + buf))][int(player.posX)] == 0)
                player.posY += player.dirY * step;
        }

        // 3. Stalker AI (Horror Stalk & Chase)
        float distToPlayer = std::hypot(player.posX - stalker.x, player.posY - stalker.y);

        // If player gets close or shines light directly on it, it starts chasing
        if (distToPlayer < 7.0f) stalker.isChasing = true;

        if (stalker.isChasing) {
            float dx = (player.posX - stalker.x) / distToPlayer;
            float dy = (player.posY - stalker.y) / distToPlayer;

            float mx = stalker.x + dx * stalker.speed * dtSec;
            float my = stalker.y + dy * stalker.speed * dtSec;

            if (worldMap[int(my)][int(mx)] == 0) {
                stalker.x = mx;
                stalker.y = my;
            }

            // Sanity drain when stalker is near
            player.sanity -= (12.0f / distToPlayer) * dtSec;
            player.sanity = std::max(0.0f, player.sanity);
        }
    }

    void renderAsciiWorld() {
        // Clear framebuffer (Pitch Black Void)
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), 0xFF000000);

        // 1. Raycast 80 text columns
        for (int col = 0; col < COLS; ++col) {
            float cameraX = 2.0f * col / float(COLS) - 1.0f;
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
                    if (worldMap[mapY][mapX] > 0) hit = worldMap[mapY][mapX];
                }
            }

            if (side == 0) perpWallDist = (sideDistX - deltaDistX);
            else           perpWallDist = (sideDistY - deltaDistY);

            zBuffer[col] = perpWallDist; // Save distance for monster sprite depth occlusion

            int lineHeight = int(ROWS / perpWallDist);
            int drawStart = -lineHeight / 2 + ROWS / 2;
            int drawEnd = lineHeight / 2 + ROWS / 2;

            // Flashlight attenuation model: intense center beam fading with distance
            float flashlightCone = (1.0f - std::abs(cameraX) * 0.45f);
            float lightIntensity = (1.0f / (1.0f + perpWallDist * perpWallDist * 0.12f)) * flashlightCone;

            if (side == 1) lightIntensity *= 0.70f; // Shadow one side of walls

            for (int r = 0; r < ROWS; ++r) {
                if (r >= drawStart && r <= drawEnd) {
                    // Wall ASCII Glyph
                    char glyph;
                    uint32_t color = 0xFFCCCCCC; // Pale green/white phosphor terminal tint

                    if (hit == 2) {
                        glyph = '#'; // Iron Door
                        color = 0xFFF59E0B;
                    } else if (hit == 3) {
                        glyph = '@'; // Escape Bunker Hatch
                        color = 0xFF10B981;
                    } else {
                        // Pick ASCII character based on flashlight intensity
                        int rampIndex = int(lightIntensity * (RAMP_LEN - 1));
                        rampIndex = std::clamp(rampIndex, 0, RAMP_LEN - 1);
                        glyph = ASCII_RAMP[rampIndex];
                    }

                    if (lightIntensity > 0.05f) {
                        drawGlyph(col, r, glyph, color);
                    }
                } else if (r > drawEnd) {
                    // Dark gritty floor dots
                    float floorDist = ROWS / (2.0f * r - ROWS);
                    float floorLight = (1.0f / (1.0f + floorDist * floorDist * 0.25f)) * flashlightCone;
                    if (floorLight > 0.12f && (col % 2 == 0 && r % 2 == 0)) {
                        drawGlyph(col, r, '.', 0xFF334155);
                    }
                }
            }
        }

        // 2. Render Stalker Billboard Sprite
        float spriteX = stalker.x - player.posX;
        float spriteY = stalker.y - player.posY;

        float invDet = 1.0f / (player.planeX * player.dirY - player.dirX * player.planeY);
        float transformX = invDet * (player.dirY * spriteX - player.dirX * spriteY);
        float transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY); // Depth

        if (transformY > 0.3f) {
            int spriteScreenX = int((COLS / 2) * (1.0f + transformX / transformY));
            int spriteHeight = std::abs(int(ROWS / transformY));
            int spriteWidth = spriteHeight; // Square monster frame

            int drawStartY = -spriteHeight / 2 + ROWS / 2;
            int drawEndY = spriteHeight / 2 + ROWS / 2;
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            int drawEndX = spriteWidth / 2 + spriteScreenX;

            for (int stripe = drawStartX; stripe < drawEndX; ++stripe) {
                if (stripe >= 0 && stripe < COLS && transformY < zBuffer[stripe]) {
                    for (int y = drawStartY; y < drawEndY; ++y) {
                        if (y >= 0 && y < ROWS) {
                            // Creepy skull / eye silhouette
                            char monsterGlyph = (y % 2 == 0) ? '&' : '$';
                            drawGlyph(stripe, y, monsterGlyph, 0xFFE11D48); // Blood Crimson
                        }
                    }
                }
            }
        }

        // 3. Horror UI & Subtitles
        drawText(2, 2, "FACILITY SECTOR 04 [BUNKER]", 0xFF64748B);
        drawText(2, 4, "KEYS: " + std::to_string(player.keysHeld), 0xFFF59E0B);
        drawText(2, 6, "SANITY: " + std::to_string(int(player.sanity)) + "%", (player.sanity < 30) ? 0xFFEF4444 : 0xFF10B981);

        if (stalker.isChasing) {
            drawText(28, 52, "* DISTANT SHUFFLING FOOTSTEPS *", 0xFFEF4444);
        }

        if (player.hasEscaped) {
            drawText(26, 28, "BUNKER AIRLOCK SEALED. YOU ESCAPED.", 0xFF22C55E);
        }
    }

    void render() {
        renderAsciiWorld();

        SDL_UpdateTexture(screenTexture, nullptr, pixelBuffer.data(), SCREEN_WIDTH * sizeof(uint32_t));
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
        if (screenTexture) SDL_DestroyTexture(screenTexture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    AsciiHorrorEngine game;
    if (game.init()) game.run();
    game.cleanup();
    return 0;
}