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

// Display resolution: 100x60 text grid (800x480) -> Scaled 2x to 1600x960 window
constexpr int CHAR_W = 8;
constexpr int CHAR_H = 8;
constexpr int VIEW_COLS = 68; // 3D Viewport Width in characters
constexpr int TOTAL_COLS = 100; // Full screen including minimap sidebar
constexpr int ROWS = 60;
constexpr int SCREEN_WIDTH = TOTAL_COLS * CHAR_W;  // 800
constexpr int SCREEN_HEIGHT = ROWS * CHAR_H;       // 480
constexpr int SCALE = 2;

constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;

// World Map (Odd dimensions for perfect grid maze)
constexpr int MAP_W = 25;
constexpr int MAP_H = 25;

// High-contrast CRT green palette
constexpr uint32_t CRT_LIGHT_BRIGHT = 0xFF4ADE80; // Bright green (E-W walls up close)
constexpr uint32_t CRT_LIGHT_MID    = 0xFF22C55E; // Standard green (E-W mid)
constexpr uint32_t CRT_LIGHT_DIM    = 0xFF16A34A; // Dim green (E-W far)

constexpr uint32_t CRT_DARK_BRIGHT  = 0xFF15803D; // Dark green (N-S walls up close)
constexpr uint32_t CRT_DARK_MID     = 0xFF166534; // Dark green (N-S mid)
constexpr uint32_t CRT_DARK_DIM     = 0xFF14532D; // Dark green (N-S far)

constexpr uint32_t RED_GOAL_BRIGHT  = 0xFFF43F5E;
constexpr uint32_t RED_GOAL_DARK    = 0xFFBE123C;

// Audio Configuration
constexpr int AUDIO_SAMPLE_RATE = 44100;
constexpr int AUDIO_BUFFER_SIZE = 1024;

// 8x8 Minimal Bitmap Font
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
    {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}
};

struct Point { int x, y; };

struct AudioState {
    float ambientPhase = 0.0f;
    float heartbeatPhase = 0.0f;
};

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioState* audio = static_cast<AudioState*>(userdata);
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);

    for (int i = 0; i < samples; ++i) {
        audio->ambientPhase += (45.0f * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
        if (audio->ambientPhase > 2.0f * 3.14159265f) audio->ambientPhase -= 2.0f * 3.14159265f;
        float ambient = std::sin(audio->ambientPhase) * 0.08f;

        buffer[i] = static_cast<int16_t>(std::clamp(ambient, -1.0f, 1.0f) * 32767.0f);
    }
}

class WalkAsciiEngine {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* screenTexture = nullptr;
    SDL_AudioDeviceID audioDevice = 0;
    std::vector<uint32_t> pixelBuffer;
    bool isRunning = false;

    AudioState audioState;
    int worldMap[MAP_H][MAP_W];
    Point startPos;
    Point endPos;

    struct Player {
        float posX = 1.5f;
        float posY = 1.5f;
        float dirX = 1.0f;
        float dirY = 0.0f;
        float planeX = 0.0f;
        float planeY = 0.66f; // ~66 deg FOV
        float moveSpeed = 3.2f;
        float rotSpeed = 2.6f;

        int forward = 0;
        int rotate = 0;
        int stepsTaken = 0;
        float prevStepDist = 0.0f;
        bool completed = false;
    } player;

    void generateMaze() {
        for (int r = 0; r < MAP_H; ++r) {
            for (int c = 0; c < MAP_W; ++c) {
                worldMap[r][c] = 1;
            }
        }

        std::stack<Point> stack;
        startPos = { 1, 1 };
        worldMap[startPos.y][startPos.x] = 0;
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
                    if (worldMap[ny][nx] == 1) dirs.push_back(i);
                }
            }

            if (!dirs.empty()) {
                int d = dirs[rand() % dirs.size()];
                worldMap[curr.y + dy[d] / 2][curr.x + dx[d] / 2] = 0;
                worldMap[curr.y + dy[d]][curr.x + dx[d]] = 0;
                stack.push({ curr.x + dx[d], curr.y + dy[d] });
            } else {
                stack.pop();
            }
        }

        endPos = { MAP_W - 2, MAP_H - 2 };
        worldMap[endPos.y][endPos.x] = 2; // Goal Cell (Red Walls)

        player.posX = startPos.x + 0.5f;
        player.posY = startPos.y + 0.5f;
        player.dirX = 1.0f;
        player.dirY = 0.0f;
        player.planeX = 0.0f;
        player.planeY = 0.66f;
        player.stepsTaken = 0;
        player.completed = false;
    }

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
            if (col + i < TOTAL_COLS) {
                drawGlyph(col + i, row, text[i], color);
            }
        }
    }

    void drawRectFilled(int startCol, int startRow, int numCols, int numRows, uint32_t color) {
        int x0 = startCol * CHAR_W;
        int y0 = startRow * CHAR_H;
        int w = numCols * CHAR_W;
        int h = numRows * CHAR_H;

        for (int y = y0; y < y0 + h; ++y) {
            if (y < 0 || y >= SCREEN_HEIGHT) continue;
            for (int x = x0; x < x0 + w; ++x) {
                if (x < 0 || x >= SCREEN_WIDTH) continue;
                pixelBuffer[y * SCREEN_WIDTH + x] = color;
            }
        }
    }

public:
    bool init() {
        srand(static_cast<unsigned int>(time(nullptr)));

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) return false;

        window = SDL_CreateWindow(
            "Walk ASCII 3D Port",
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

        generateMaze();
        isRunning = true;
        return true;
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) isRunning = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_r) generateMaze();
            }
        }

        const uint8_t* state = SDL_GetKeyboardState(NULL);
        player.forward = 0;
        player.rotate = 0;

        if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])    player.forward += 1;
        if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])  player.forward -= 1;
        if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])  player.rotate -= 1;
        if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) player.rotate += 1;
    }

    void update(double dt) {
        float dtSec = static_cast<float>(dt);

        // Rotation
        if (player.rotate != 0) {
            float rot = player.rotate * player.rotSpeed * dtSec;
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(rot) - player.dirY * sin(rot);
            player.dirY = oldDirX * sin(rot) + player.dirY * cos(rot);

            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(rot) - player.planeY * sin(rot);
            player.planeY = oldPlaneX * sin(rot) + player.planeY * cos(rot);
        }

        // Movement with Collision & Step Tracking
        if (player.forward != 0) {
            float step = player.forward * player.moveSpeed * dtSec;
            float buf = (step > 0) ? 0.35f : -0.35f;

            float prevX = player.posX;
            float prevY = player.posY;

            if (worldMap[int(player.posY)][int(player.posX + player.dirX * (step + buf))] != 1)
                player.posX += player.dirX * step;
            if (worldMap[int(player.posY + player.dirY * (step + buf))][int(player.posX)] != 1)
                player.posY += player.dirY * step;

            // Step counter accumulation
            player.prevStepDist += std::hypot(player.posX - prevX, player.posY - prevY);
            if (player.prevStepDist >= 1.0f) {
                player.stepsTaken++;
                player.prevStepDist = 0.0f;
            }
        }

        // Win check
        if (int(player.posX) == endPos.x && int(player.posY) == endPos.y) {
            player.completed = true;
        }
    }

    // --- ACCURATE ORIENTATION & DEPTH SHADER ---
    void render3DView() {
        for (int col = 0; col < VIEW_COLS; ++col) {
            float cameraX = 2.0f * col / float(VIEW_COLS) - 1.0f;
            float rayDirX = player.dirX + player.planeX * cameraX;
            float rayDirY = player.dirY + player.planeY * cameraX;

            int mapX = int(player.posX);
            int mapY = int(player.posY);

            float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1.0f / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1.0f / rayDirY);
            float sideDistX, sideDistY, perpWallDist;
            int stepX, stepY, hit = 0, side = 0; // side 0 = East/West, side 1 = North/South

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

            int lineHeight = int(ROWS / perpWallDist);
            int drawStart = -lineHeight / 2 + ROWS / 2;
            int drawEnd = lineHeight / 2 + ROWS / 2;

            // 1. Determine Wall Glyph based on Distance Layer
            char wallGlyph = ' ';
            if (perpWallDist <= 1.25f)      wallGlyph = '@';
            else if (perpWallDist <= 2.50f) wallGlyph = '#';
            else if (perpWallDist <= 4.00f) wallGlyph = '%';
            else if (perpWallDist <= 5.80f) wallGlyph = '*';
            else if (perpWallDist <= 7.50f) wallGlyph = '+';
            else if (perpWallDist <= 9.00f) wallGlyph = '-';
            else if (perpWallDist <= 11.0f) wallGlyph = '.';
            else                            wallGlyph = ' '; // Pitch black fog beyond 11 units

            // 2. Determine Color by Side (Orientation Shading)
            uint32_t wallColor;
            if (hit == 2) {
                // Goal Wall (Red)
                wallColor = (side == 0) ? RED_GOAL_BRIGHT : RED_GOAL_DARK;
            } else {
                // Normal Maze Wall (Green)
                if (side == 0) {
                    // East/West facing (Light shade)
                    if (perpWallDist < 3.0f)      wallColor = CRT_LIGHT_BRIGHT;
                    else if (perpWallDist < 6.5f) wallColor = CRT_LIGHT_MID;
                    else                          wallColor = CRT_LIGHT_DIM;
                } else {
                    // North/South facing (Dark shade)
                    if (perpWallDist < 3.0f)      wallColor = CRT_DARK_BRIGHT;
                    else if (perpWallDist < 6.5f) wallColor = CRT_DARK_MID;
                    else                          wallColor = CRT_DARK_DIM;
                }
            }

            // Draw Column
            for (int r = 0; r < ROWS; ++r) {
                if (r >= drawStart && r <= drawEnd && wallGlyph != ' ') {
                    drawGlyph(col, r, wallGlyph, wallColor);
                }
            }
        }
    }

    // --- SIDEBAR MINIMAP & UI ---
    void renderSidebarUI() {
        // Vertical Divider line between 3D view and sidebar
        for (int r = 0; r < ROWS; ++r) {
            drawGlyph(VIEW_COLS, r, '|', 0xFF334155);
        }

        // Top Header
        drawText(2, 1, "Walk ASCII 3D", 0xFFFFFFFF);

        // Top-Right Minimap Box
        int miniStartX = 72;
        int miniStartY = 3;

        for (int r = 0; r < MAP_H; ++r) {
            for (int c = 0; c < MAP_W; ++c) {
                char mapCh = ' ';
                uint32_t mapCol = 0xFF1E293B;

                if (worldMap[r][c] == 1) {
                    mapCh = '#';
                    mapCol = 0xFF475569; // Wall
                } else if (r == startPos.y && c == startPos.x) {
                    mapCh = 'S';
                    mapCol = 0xFF22C55E; // Start
                } else if (r == endPos.y && c == endPos.x) {
                    mapCh = 'E';
                    mapCol = 0xFFEF4444; // End
                }

                drawGlyph(miniStartX + c, miniStartY + r, mapCh, mapCol);
            }
        }

        // Player Dot on Minimap
        int pCol = miniStartX + int(player.posX);
        int pRow = miniStartY + int(player.posY);
        drawGlyph(pCol, pRow, 'O', 0xFF38BDF8);

        // Player Forward View Indicator Ray
        int pRayCol = miniStartX + int(player.posX + player.dirX * 1.5f);
        int pRayRow = miniStartY + int(player.posY + player.dirY * 1.5f);
        if (pRayCol != pCol || pRayRow != pRow) {
            drawGlyph(pRayCol, pRayRow, '*', 0xFF0284C7);
        }

        // Controls & Progress Telemetry
        int uiBaseY = 32;
        drawText(72, uiBaseY,     "[S] Start  [E] End  [O] You", 0xFF94A3B8);
        drawText(72, uiBaseY + 3, "W A S D / Arrows to move", 0xFFE2E8F0);

        // Step Counter
        drawText(72, uiBaseY + 7, "Steps: " + std::to_string(player.stepsTaken), 0xFFFFFFFF);

        // Progress bar percentage
        float totalDist = std::hypot(endPos.x - startPos.x, endPos.y - startPos.y);
        float currDist = std::hypot(endPos.x - player.posX, endPos.y - player.posY);
        int progress = std::clamp(int((1.0f - currDist / totalDist) * 100.0f), 0, 100);
        drawText(72, uiBaseY + 9, "Progress: " + std::to_string(progress) + "%", 0xFF22C55E);

        // Win Banner
        if (player.completed) {
            drawRectFilled(12, 22, 44, 8, 0xFF0F172A);
            drawText(20, 24, "MAZE COMPLETED!", 0xFF4ADE80);
            drawText(18, 26, "Total Steps: " + std::to_string(player.stepsTaken) + " | Press R", 0xFFFFFFFF);
        }
    }

    void render() {
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), 0xFF0B0F17); // Dark backdrop

        render3DView();
        renderSidebarUI();

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
        if (audioDevice != 0) SDL_CloseAudioDevice(audioDevice);
        if (screenTexture) SDL_DestroyTexture(screenTexture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    WalkAsciiEngine engine;
    if (engine.init()) engine.run();
    engine.cleanup();
    return 0;
}