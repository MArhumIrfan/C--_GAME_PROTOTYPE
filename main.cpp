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

// Virtual Text Grid Resolution (Native 800x480)[cite: 2]
constexpr int CHAR_W = 8;[cite: 2]
constexpr int CHAR_H = 8;[cite: 2]
constexpr int TOTAL_COLS = 100;[cite: 2]
constexpr int ROWS = 60;[cite: 2]
constexpr int NATIVE_WIDTH = TOTAL_COLS * CHAR_W;  // 800[cite: 2]
constexpr int NATIVE_HEIGHT = ROWS * CHAR_H;       // 480[cite: 2]

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

constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;[cite: 2]

constexpr int MAP_W = 27;
constexpr int MAP_H = 27;

// High-contrast CRT green palette[cite: 2]
constexpr uint32_t CRT_LIGHT_BRIGHT = 0xFF4ADE80; // Bright green (E-W walls up close)[cite: 2]
constexpr uint32_t CRT_LIGHT_MID    = 0xFF22C55E; // Standard green (E-W mid)[cite: 2]
constexpr uint32_t CRT_LIGHT_DIM    = 0xFF16A34A; // Dim green (E-W far)[cite: 2]

constexpr uint32_t CRT_DARK_BRIGHT  = 0xFF15803D; // Dark green (N-S walls up close)[cite: 2]
constexpr uint32_t CRT_DARK_MID     = 0xFF166534; // Dark green (N-S mid)[cite: 2]
constexpr uint32_t CRT_DARK_DIM     = 0xFF14532D; // Dark green (N-S far)[cite: 2]

// Distinct colors for elevation & steps
constexpr uint32_t CRT_STAIR_BRIGHT = 0xFF38BDF8; // Cyan stair treads
constexpr uint32_t CRT_STAIR_DARK   = 0xFF0284C7; // Dim cyan
constexpr uint32_t CRT_LEDGE_COLOR  = 0xFFF59E0B; // Amber warning for drop-off ledges
constexpr uint32_t RED_GOAL_BRIGHT  = 0xFFF43F5E;[cite: 2]
constexpr uint32_t RED_GOAL_DARK    = 0xFFBE123C;[cite: 2]

constexpr int AUDIO_SAMPLE_RATE = 44100;[cite: 2]
constexpr int AUDIO_BUFFER_SIZE = 1024;[cite: 2]

enum GameState {[cite: 2]
    STATE_TITLE,[cite: 2]
    STATE_PLAYING,[cite: 2]
    STATE_SUCCESS,[cite: 2]
    STATE_GAMEOVER[cite: 2]
};[cite: 2]

enum Difficulty {[cite: 2]
    DIFF_NORMAL = 0,[cite: 2]
    DIFF_EASY   = 1[cite: 2]
};[cite: 2]

// 8x8 Minimal Bitmap Font[cite: 2]
const uint8_t FONT_8X8[96][8] = {[cite: 2]
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},[cite: 2]
    {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00},[cite: 2]
    {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, {0x00,0x66,0xAC,0xD8,0x36,0x6A,0x00,0x00},[cite: 2]
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00},[cite: 2]
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},[cite: 2]
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},[cite: 2]
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},[cite: 2]
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},[cite: 2]
    {0x7C,0xC6,0xCE,0xD6,0xE6,0xC6,0x7C,0x00}, {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},[cite: 2]
    {0x7C,0xC6,0x06,0x1C,0x30,0x66,0xFE,0x00}, {0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00},[cite: 2]
    {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00}, {0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00},[cite: 2]
    {0x7C,0xC6,0xC0,0xFC,0xC6,0xC6,0x7C,0x00}, {0xFE,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},[cite: 2]
    {0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00}, {0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00},[cite: 2]
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30},[cite: 2]
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00},[cite: 2]
    {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00}, {0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00},[cite: 2]
    {0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00}, {0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00},[cite: 2]
    {0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00}, {0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00},[cite: 2]
    {0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00}, {0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00},[cite: 2]
    {0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00}, {0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00},[cite: 2]
    {0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},[cite: 2]
    {0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00}, {0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00},[cite: 2]
    {0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00}, {0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00},[cite: 2]
    {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00}, {0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00},[cite: 2]
    {0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00}, {0x7C,0xC6,0xC6,0xC6,0xC6,0xCE,0x7C,0x06},[cite: 2]
    {0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE6,0x00}, {0x7C,0xC6,0x60,0x38,0x0C,0xC6,0x7C,0x00},[cite: 2]
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, {0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00},[cite: 2]
    {0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x00}, {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00},[cite: 2]
    {0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00}, {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},[cite: 2]
    {0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00}, {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},[cite: 2]
    {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}[cite: 2]
};[cite: 2]

struct Point { int x, y; };[cite: 2]

struct MapCell {
    int wallType = 0;      // 0 = Air, 1 = Wall, 2 = Goal
    float floorH = 0.0f;   // 0.0 = Ground, 0.5 = Stairs, 1.0 = Overpass
    float ceilH = 2.0f;
    bool isStairs = false;
};

struct AudioState {[cite: 2]
    float ambientPhase = 0.0f;[cite: 2]
    float heartbeatPhase = 0.0f;[cite: 2]
    float monsterPhase = 0.0f;[cite: 2]
    float sanity = 100.0f;[cite: 2]
    float monsterDist = 20.0f;[cite: 2]
    bool isChasing = false;[cite: 2]
    bool inGame = false;[cite: 2]
};[cite: 2]

void audioCallback(void* userdata, Uint8* stream, int len) {[cite: 2]
    AudioState* audio = static_cast<AudioState*>(userdata);[cite: 2]
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);[cite: 2]
    int samples = len / sizeof(int16_t);[cite: 2]

    for (int i = 0; i < samples; ++i) {[cite: 2]
        if (!audio->inGame) {[cite: 2]
            buffer[i] = 0;[cite: 2]
            continue;[cite: 2]
        }[cite: 2]

        audio->ambientPhase += (42.0f * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;[cite: 2]
        if (audio->ambientPhase > 2.0f * 3.14159265f) audio->ambientPhase -= 2.0f * 3.14159265f;[cite: 2]
        float ambient = std::sin(audio->ambientPhase) * 0.08f;[cite: 2]

        float heartBPM = 1.0f + (100.0f - audio->sanity) / 100.0f * 2.0f;[cite: 2]
        audio->heartbeatPhase += (heartBPM * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;[cite: 2]
        if (audio->heartbeatPhase > 2.0f * 3.14159265f) audio->heartbeatPhase -= 2.0f * 3.14159265f;[cite: 2]

        float beatEnv = 0.0f;[cite: 2]
        float cyclePos = audio->heartbeatPhase / (2.0f * 3.14159265f);[cite: 2]
        if (cyclePos < 0.15f) beatEnv = std::sin(cyclePos / 0.15f * 3.14159265f);[cite: 2]
        else if (cyclePos > 0.22f && cyclePos < 0.35f) beatEnv = std::sin((cyclePos - 0.22f) / 0.13f * 3.14159265f) * 0.7f;[cite: 2]

        float heartbeat = std::sin(audio->heartbeatPhase * 40.0f) * beatEnv * (0.35f + (100.0f - audio->sanity) / 100.0f * 0.50f);[cite: 2]

        float monsterAudio = 0.0f;[cite: 2]
        if (audio->monsterDist < 10.0f) {[cite: 2]
            float proxVol = 1.0f - (audio->monsterDist / 10.0f);[cite: 2]
            float breathFreq = audio->isChasing ? 2.5f : 0.8f;[cite: 2]
            audio->monsterPhase += (breathFreq * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;[cite: 2]
            if (audio->monsterPhase > 2.0f * 3.14159265f) audio->monsterPhase -= 2.0f * 3.14159265f;[cite: 2]

            float noise = ((rand() % 2000) / 1000.0f - 1.0f);[cite: 2]
            monsterAudio = noise * (std::sin(audio->monsterPhase) * 0.5f + 0.5f) * proxVol * 0.4f;[cite: 2]
        }[cite: 2]

        buffer[i] = static_cast<int16_t>(std::clamp(ambient + heartbeat + monsterAudio, -1.0f, 1.0f) * 32767.0f);[cite: 2]
    }[cite: 2]
}[cite: 2]

class WalkAsciiElevationEngine {
private:
    SDL_Window* window = nullptr;[cite: 2]
    SDL_Renderer* renderer = nullptr;[cite: 2]
    SDL_Texture* screenTexture = nullptr;[cite: 2]
    SDL_AudioDeviceID audioDevice = 0;[cite: 2]
    std::vector<uint32_t> pixelBuffer;[cite: 2]
    bool isRunning = false;[cite: 2]

    GameState currentState = STATE_TITLE;[cite: 2]
    Difficulty currentDifficulty = DIFF_NORMAL;[cite: 2]
    int currentResIndex = 2; // Default 1366x768
    int menuCursor = 0;[cite: 2]

    AudioState audioState;[cite: 2]
    MapCell worldMap[MAP_H][MAP_W];
    Point startPos;[cite: 2]
    Point endPos;[cite: 2]

    int currentLevel = 1;[cite: 2]
    int totalSteps = 0;[cite: 2]
    float levelTime = 0.0f;[cite: 2]
    std::string deathReason = "";[cite: 2]

    struct Player {
        float posX = 1.5f;[cite: 2]
        float posY = 1.5f;[cite: 2]
        float posZ = 0.0f;
        float targetPosZ = 0.0f;
        float eyeHeight = 0.5f;
        float pitch = 0.0f;

        float dirX = 1.0f;[cite: 2]
        float dirY = 0.0f;[cite: 2]
        float planeX = 0.0f;[cite: 2]
        float planeY = 0.66f;[cite: 2]
        float moveSpeed = 3.2f;[cite: 2]
        float mouseSensitivity = 0.0022f;

        int forward = 0; // W/S (+1 / -1)
        int strafe = 0;  // A/D (-1 / +1)
        float stepAccumulator = 0.0f;[cite: 2]
        
        float sanity = 100.0f;[cite: 2]
        float health = 100.0f;[cite: 2]
        bool takingDamage = false;[cite: 2]
    } player;

    struct Monster {
        float x = 12.5f;[cite: 2]
        float y = 12.5f;[cite: 2]
        float speed = 1.8f;[cite: 2]
        bool isChasing = false;[cite: 2]
    } stalker;

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

    void generateMazeWithOverpass() {
        for (int r = 0; r < MAP_H; ++r) {
            for (int c = 0; c < MAP_W; ++c) {
                worldMap[r][c].wallType = 1;
                worldMap[r][c].floorH = 0.0f;
                worldMap[r][c].ceilH = 2.0f;
                worldMap[r][c].isStairs = false;
            }
        }

        std::stack<Point> stack;[cite: 2]
        startPos = { 1, 1 };[cite: 2]
        worldMap[startPos.y][startPos.x].wallType = 0;
        stack.push(startPos);[cite: 2]

        const int dx[4] = { 0, 0, 2, -2 };[cite: 2]
        const int dy[4] = { -2, 2, 0, 0 };[cite: 2]

        while (!stack.empty()) {[cite: 2]
            Point curr = stack.top();[cite: 2]
            std::vector<int> dirs;[cite: 2]

            for (int i = 0; i < 4; ++i) {[cite: 2]
                int nx = curr.x + dx[i];[cite: 2]
                int ny = curr.y + dy[i];[cite: 2]
                if (nx > 0 && nx < MAP_W - 1 && ny > 0 && ny < MAP_H - 1) {[cite: 2]
                    if (worldMap[ny][nx].wallType == 1) dirs.push_back(i);[cite: 2]
                }[cite: 2]
            }[cite: 2]

            if (!dirs.empty()) {[cite: 2]
                int d = dirs[rand() % dirs.size()];[cite: 2]
                worldMap[curr.y + dy[d] / 2][curr.x + dx[d] / 2].wallType = 0;
                worldMap[curr.y + dy[d]][curr.x + dx[d]].wallType = 0;
                stack.push({ curr.x + dx[d], curr.y + dy[d] });[cite: 2]
            } else {[cite: 2]
                stack.pop();[cite: 2]
            }[cite: 2]
        }[cite: 2]

        int midX = MAP_W / 2;
        int midY = MAP_H / 2;

        for (int x = midX - 3; x <= midX + 3; ++x) {
            worldMap[midY][x].wallType = 0;
            worldMap[midY][x].floorH = 1.0f;
            worldMap[midY][x].ceilH = 3.0f;
        }

        worldMap[midY][midX - 4].wallType = 0;
        worldMap[midY][midX - 4].floorH = 0.5f;
        worldMap[midY][midX - 4].isStairs = true;

        worldMap[midY][midX + 4].wallType = 0;
        worldMap[midY][midX + 4].floorH = 0.5f;
        worldMap[midY][midX + 4].isStairs = true;

        endPos = { MAP_W - 2, MAP_H - 2 };[cite: 2]
        worldMap[endPos.y][endPos.x].wallType = 2;

        player.posX = startPos.x + 0.5f;[cite: 2]
        player.posY = startPos.y + 0.5f;[cite: 2]
        player.posZ = 0.0f;
        player.targetPosZ = 0.0f;
        player.pitch = 0.0f;
        player.dirX = 1.0f;[cite: 2]
        player.dirY = 0.0f;[cite: 2]
        player.planeX = 0.0f;[cite: 2]
        player.planeY = 0.66f;[cite: 2]
        player.stepAccumulator = 0.0f;[cite: 2]

        stalker.x = MAP_W / 2 + 0.5f;[cite: 2]
        stalker.y = MAP_H / 2 + 0.5f;[cite: 2]
        stalker.isChasing = false;[cite: 2]
    }

    void startNewGame() {[cite: 2]
        currentLevel = 1;[cite: 2]
        totalSteps = 0;[cite: 2]
        levelTime = 0.0f;[cite: 2]
        player.sanity = 100.0f;[cite: 2]
        player.health = 100.0f;[cite: 2]
        generateMazeWithOverpass();
        currentState = STATE_PLAYING;[cite: 2]
        setCaptureMouse(true);

        SDL_LockAudioDevice(audioDevice);[cite: 2]
        audioState.inGame = true;[cite: 2]
        audioState.sanity = 100.0f;[cite: 2]
        audioState.monsterDist = 20.0f;[cite: 2]
        SDL_UnlockAudioDevice(audioDevice);[cite: 2]
    }[cite: 2]

    void nextLevel() {[cite: 2]
        currentLevel++;[cite: 2]
        player.sanity = std::min(100.0f, player.sanity + 30.0f);[cite: 2]
        player.health = std::min(100.0f, player.health + 30.0f);[cite: 2]
        generateMazeWithOverpass();
        currentState = STATE_PLAYING;[cite: 2]
        setCaptureMouse(true);

        SDL_LockAudioDevice(audioDevice);[cite: 2]
        audioState.inGame = true;[cite: 2]
        SDL_UnlockAudioDevice(audioDevice);[cite: 2]
    }[cite: 2]

    void drawGlyph(int col, int row, char c, uint32_t fgColor) {[cite: 2]
        if (c < 32 || c > 127) return;[cite: 2]
        const uint8_t* glyph = FONT_8X8[c - 32];[cite: 2]
        int startX = col * CHAR_W;[cite: 2]
        int startY = row * CHAR_H;[cite: 2]

        for (int y = 0; y < CHAR_H; ++y) {[cite: 2]
            for (int x = 0; x < CHAR_W; ++x) {[cite: 2]
                if ((glyph[y] >> (7 - x)) & 1) {[cite: 2]
                    pixelBuffer[(startY + y) * NATIVE_WIDTH + (startX + x)] = fgColor;[cite: 2]
                }[cite: 2]
            }[cite: 2]
        }[cite: 2]
    }[cite: 2]

    void drawText(int col, int row, const std::string& text, uint32_t color) {[cite: 2]
        for (size_t i = 0; i < text.size(); ++i) {[cite: 2]
            if (col + i < TOTAL_COLS) {[cite: 2]
                drawGlyph(col + i, row, text[i], color);[cite: 2]
            }[cite: 2]
        }[cite: 2]
    }[cite: 2]

public:
    bool init() {[cite: 2]
        srand(static_cast<unsigned int>(time(nullptr)));[cite: 2]

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) return false;[cite: 2]

        window = SDL_CreateWindow(
            "Walk ASCII 3D: Multi-Floor & Stairs Engine",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            RESOLUTION_PRESETS[currentResIndex].width,
            RESOLUTION_PRESETS[currentResIndex].height,
            SDL_WINDOW_SHOWN
        );

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);[cite: 2]
        screenTexture = SDL_CreateTexture([cite: 2]
            renderer,[cite: 2]
            SDL_PIXELFORMAT_ARGB8888,[cite: 2]
            SDL_TEXTUREACCESS_STREAMING,[cite: 2]
            NATIVE_WIDTH, NATIVE_HEIGHT[cite: 2]
        );[cite: 2]

        pixelBuffer.resize(NATIVE_WIDTH * NATIVE_HEIGHT, 0xFF000000);[cite: 2]

        SDL_AudioSpec wantedSpec;[cite: 2]
        SDL_zero(wantedSpec);[cite: 2]
        wantedSpec.freq = AUDIO_SAMPLE_RATE;[cite: 2]
        wantedSpec.format = AUDIO_S16SYS;[cite: 2]
        wantedSpec.channels = 1;[cite: 2]
        wantedSpec.samples = AUDIO_BUFFER_SIZE;[cite: 2]
        wantedSpec.callback = audioCallback;[cite: 2]
        wantedSpec.userdata = &audioState;[cite: 2]

        audioDevice = SDL_OpenAudioDevice(nullptr, 0, &wantedSpec, nullptr, 0);[cite: 2]
        if (audioDevice != 0) SDL_PauseAudioDevice(audioDevice, 0);[cite: 2]

        isRunning = true;[cite: 2]
        return true;[cite: 2]
    }

    void handleEvents() {[cite: 2]
        SDL_Event event;[cite: 2]
        while (SDL_PollEvent(&event)) {[cite: 2]
            if (event.type == SDL_QUIT) isRunning = false;[cite: 2]

            // 1. Mouse Motion for Smooth Looking
            if (currentState == STATE_PLAYING && event.type == SDL_MOUSEMOTION) {
                float rotAngle = event.motion.xrel * player.mouseSensitivity;

                // Horizontal Yaw
                float oldDirX = player.dirX;
                player.dirX = player.dirX * cos(rotAngle) - player.dirY * sin(rotAngle);
                player.dirY = oldDirX * sin(rotAngle) + player.dirY * cos(rotAngle);

                float oldPlaneX = player.planeX;
                player.planeX = player.planeX * cos(rotAngle) - player.planeY * sin(rotAngle);
                player.planeY = oldPlaneX * sin(rotAngle) + player.planeY * cos(rotAngle);

                // Vertical Pitch
                player.pitch -= event.motion.yrel * 0.12f;
                player.pitch = std::clamp(player.pitch, -22.0f, 22.0f);
            }

            if (event.type == SDL_KEYDOWN) {[cite: 2]
                if (currentState == STATE_TITLE) {[cite: 2]
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {[cite: 2]
                        menuCursor = (menuCursor - 1 + 3) % 3;[cite: 2]
                    }[cite: 2]
                    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {[cite: 2]
                        menuCursor = (menuCursor + 1) % 3;[cite: 2]
                    }[cite: 2]
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {[cite: 2]
                        if (menuCursor == 0) startNewGame();[cite: 2]
                        else if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;[cite: 2]
                        else if (menuCursor == 2) {
                            currentResIndex = (currentResIndex + 1) % RESOLUTION_PRESETS.size();
                            updateWindowScale();
                        }
                    }
                    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {[cite: 2]
                        if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;[cite: 2]
                        if (menuCursor == 2) {
                            int count = RESOLUTION_PRESETS.size();
                            currentResIndex = (event.key.keysym.sym == SDLK_RIGHT)
                                ? ((currentResIndex + 1) % count)
                                : ((currentResIndex - 1 + count) % count);
                            updateWindowScale();
                        }
                    }
                }[cite: 2]
                else if (currentState == STATE_SUCCESS) {[cite: 2]
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) nextLevel();[cite: 2]
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = STATE_TITLE;[cite: 2]
                        setCaptureMouse(false);
                    }
                }[cite: 2]
                else if (currentState == STATE_GAMEOVER) {[cite: 2]
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) startNewGame();[cite: 2]
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = STATE_TITLE;[cite: 2]
                        setCaptureMouse(false);
                    }
                }[cite: 2]
                else if (currentState == STATE_PLAYING) {[cite: 2]
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = STATE_TITLE;[cite: 2]
                        setCaptureMouse(false);
                    }
                }[cite: 2]
            }[cite: 2]
        }[cite: 2]

        // 2. Keyboard Polling for WASD Movement & Strafe
        if (currentState == STATE_PLAYING) {[cite: 2]
            const uint8_t* state = SDL_GetKeyboardState(NULL);[cite: 2]
            player.forward = 0;
            player.strafe = 0;

            if (state[SDL_SCANCODE_W]) player.forward += 1;
            if (state[SDL_SCANCODE_S]) player.forward -= 1;
            if (state[SDL_SCANCODE_A]) player.strafe -= 1; // Strafe left
            if (state[SDL_SCANCODE_D]) player.strafe += 1; // Strafe right
        }[cite: 2]
    }[cite: 2]

    void update(double dt) {[cite: 2]
        if (currentState != STATE_PLAYING) return;[cite: 2]

        float dtSec = static_cast<float>(dt);[cite: 2]
        levelTime += dtSec;[cite: 2]
        player.takingDamage = false;[cite: 2]

        // 1. WASD Vector Movement (Forward + Strafe)
        if (player.forward != 0 || player.strafe != 0) {
            float forwardStep = player.forward * player.moveSpeed * dtSec;
            float strafeStep  = player.strafe  * (player.moveSpeed * 0.85f) * dtSec; // Slightly slower strafe

            // Strafe vector is perpendicular to dir vector: (dirY, -dirX)
            float moveX = player.dirX * forwardStep + player.dirY * strafeStep;
            float moveY = player.dirY * forwardStep - player.dirX * strafeStep;

            float bufX = (moveX > 0) ? 0.32f : -0.32f;
            float bufY = (moveY > 0) ? 0.32f : -0.32f;

            float prevX = player.posX;
            float prevY = player.posY;

            int nextTileX = int(player.posX + moveX + bufX);
            int nextTileY = int(player.posY + moveY + bufY);

            // Move along X with step-height tolerance
            if (worldMap[int(player.posY)][nextTileX].wallType != 1) {
                float hDiff = std::abs(worldMap[int(player.posY)][nextTileX].floorH - player.posZ);
                if (hDiff <= 0.65f) player.posX += moveX;
            }

            // Move along Y with step-height tolerance
            if (worldMap[nextTileY][int(player.posX)].wallType != 1) {
                float hDiff = std::abs(worldMap[nextTileY][int(player.posX)].floorH - player.posZ);
                if (hDiff <= 0.65f) player.posY += moveY;
            }

            player.stepAccumulator += std::hypot(player.posX - prevX, player.posY - prevY);[cite: 2]
            if (player.stepAccumulator >= 1.0f) {[cite: 2]
                totalSteps++;[cite: 2]
                player.stepAccumulator = 0.0f;[cite: 2]
            }[cite: 2]
        }

        // 2. Vertical Stair Step Interpolation
        int currTileX = int(player.posX);
        int currTileY = int(player.posY);
        player.targetPosZ = worldMap[currTileY][currTileX].floorH;
        player.posZ += (player.targetPosZ - player.posZ) * 0.25f;

        // 3. Stalker AI
        float distToMonster = std::hypot(player.posX - stalker.x, player.posY - stalker.y);[cite: 2]

        if (distToMonster < 8.5f) {[cite: 2]
            stalker.isChasing = true;[cite: 2]
            float dx = (player.posX - stalker.x) / distToMonster;[cite: 2]
            float dy = (player.posY - stalker.y) / distToMonster;[cite: 2]

            float nx = stalker.x + dx * stalker.speed * dtSec;[cite: 2]
            float ny = stalker.y + dy * stalker.speed * dtSec;[cite: 2]

            if (worldMap[int(ny)][int(nx)].wallType == 0) {
                stalker.x = nx;
                stalker.y = ny;
            }

            player.sanity -= (6.0f / std::max(1.0f, distToMonster)) * dtSec;[cite: 2]
            if (distToMonster < 1.1f) {[cite: 2]
                player.health -= 28.0f * dtSec;[cite: 2]
                player.takingDamage = true;[cite: 2]
            }[cite: 2]
        } else {[cite: 2]
            stalker.isChasing = false;[cite: 2]
            player.sanity -= 0.08f * dtSec;[cite: 2]
            if (distToMonster > 12.0f) {[cite: 2]
                player.sanity = std::min(100.0f, player.sanity + 1.0f * dtSec);[cite: 2]
                player.health = std::min(100.0f, player.health + 0.8f * dtSec);[cite: 2]
            }[cite: 2]
        }[cite: 2]

        player.sanity = std::max(0.0f, player.sanity);[cite: 2]
        player.health = std::max(0.0f, player.health);[cite: 2]

        if (player.sanity <= 0.0f) {[cite: 2]
            deathReason = "LOST TO THE TERROR (SANITY DEPLETED)";[cite: 2]
            currentState = STATE_GAMEOVER;[cite: 2]
            setCaptureMouse(false);
            SDL_LockAudioDevice(audioDevice);[cite: 2]
            audioState.inGame = false;[cite: 2]
            SDL_UnlockAudioDevice(audioDevice);[cite: 2]
            return;[cite: 2]
        }[cite: 2]

        if (player.health <= 0.0f) {[cite: 2]
            deathReason = "SLAIN BY THE STALKER (HEALTH DEPLETED)";[cite: 2]
            currentState = STATE_GAMEOVER;[cite: 2]
            setCaptureMouse(false);
            SDL_LockAudioDevice(audioDevice);[cite: 2]
            audioState.inGame = false;[cite: 2]
            SDL_UnlockAudioDevice(audioDevice);[cite: 2]
            return;[cite: 2]
        }[cite: 2]

        if (currTileX == endPos.x && currTileY == endPos.y) {[cite: 2]
            currentState = STATE_SUCCESS;[cite: 2]
            setCaptureMouse(false);
            SDL_LockAudioDevice(audioDevice);[cite: 2]
            audioState.inGame = false;[cite: 2]
            SDL_UnlockAudioDevice(audioDevice);[cite: 2]
            return;[cite: 2]
        }[cite: 2]

        SDL_LockAudioDevice(audioDevice);[cite: 2]
        audioState.sanity = player.sanity;[cite: 2]
        audioState.monsterDist = distToMonster;[cite: 2]
        audioState.isChasing = stalker.isChasing;[cite: 2]
        SDL_UnlockAudioDevice(audioDevice);[cite: 2]
    }[cite: 2]

    // --- STEP RISER & ELEVATION RAYCASTER ---
    void render3DView() {
        int viewWidth = (currentDifficulty == DIFF_EASY) ? 68 : TOTAL_COLS;
        float totalPlayerZ = player.posZ + player.eyeHeight;
        int horizon = int(ROWS / 2 + player.pitch);

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
            bool stepIsStair = false;

            while (hit == 0) {
                if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
                else                       { sideDistY += deltaDistY; mapY += stepY; side = 1; }

                if (mapX >= 0 && mapX < MAP_W && mapY >= 0 && mapY < MAP_H) {
                    float currCellFloor = worldMap[mapY][mapX].floorH;
                    if (!hitStepRiser && worldMap[mapY][mapX].wallType == 0 && std::abs(currCellFloor - prevFloorH) > 0.1f) {
                        hitStepRiser = true;
                        stepFloorDiff = currCellFloor - prevFloorH;
                        stepRiserDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
                        stepIsStair = worldMap[mapY][mapX].isStairs;
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

            // 1. Draw Textured Floor
            for (int r = horizon; r < ROWS; ++r) {
                float p = r - horizon;
                if (p > 0) {
                    float rowDist = (ROWS * totalPlayerZ) / p;
                    if (rowDist < 8.0f && (int(rowDist * 4.0f) % 2 == 0) && (col % 3 == 0)) {
                        drawGlyph(col, r, '.', 0xFF14532D);
                    }
                }
            }

            // 2. Draw Solid Walls
            float wallFloorH = (mapY >= 0 && mapY < MAP_H && mapX >= 0 && mapX < MAP_W) ? worldMap[mapY][mapX].floorH : 0.0f;
            float wallCeilH  = (mapY >= 0 && mapY < MAP_H && mapX >= 0 && mapX < MAP_W) ? worldMap[mapY][mapX].ceilH : 2.0f;

            int drawStart = horizon - int(((wallCeilH - totalPlayerZ) * ROWS) / perpWallDist);
            int drawEnd   = horizon - int(((wallFloorH - totalPlayerZ) * ROWS) / perpWallDist);

            char wallGlyph = ' ';
            if (perpWallDist <= 1.25f)      wallGlyph = '@';
            else if (perpWallDist <= 2.50f) wallGlyph = '#';
            else if (perpWallDist <= 4.00f) wallGlyph = '%';
            else if (perpWallDist <= 5.80f) wallGlyph = '*';
            else if (perpWallDist <= 7.50f) wallGlyph = '+';
            else if (perpWallDist <= 9.00f) wallGlyph = '-';
            else if (perpWallDist <= 11.0f) wallGlyph = '.';

            uint32_t wallColor;
            if (hit == 2) {
                wallColor = (side == 0) ? RED_GOAL_BRIGHT : RED_GOAL_DARK;
            } else {
                if (side == 0) {
                    if (perpWallDist < 3.0f)      wallColor = CRT_LIGHT_BRIGHT;
                    else if (perpWallDist < 6.5f) wallColor = CRT_LIGHT_MID;
                    else                          wallColor = CRT_LIGHT_DIM;
                } else {
                    if (perpWallDist < 3.0f)      wallColor = CRT_DARK_BRIGHT;
                    else if (perpWallDist < 6.5f) wallColor = CRT_DARK_MID;
                    else                          wallColor = CRT_DARK_DIM;
                }
            }

            for (int r = 0; r < ROWS; ++r) {
                if (r >= drawStart && r <= drawEnd && wallGlyph != ' ') {
                    drawGlyph(col, r, wallGlyph, wallColor);
                }
            }

            // 3. Render Elevation Risers (Stairs & Ledges)
            if (hitStepRiser && stepRiserDist > 0.1f && stepRiserDist < perpWallDist) {
                float lowH = std::min(prevFloorH, prevFloorH + stepFloorDiff);
                float highH = std::max(prevFloorH, prevFloorH + stepFloorDiff);

                int stepTop = horizon - int(((highH - totalPlayerZ) * ROWS) / stepRiserDist);
                int stepBottom = horizon - int(((lowH - totalPlayerZ) * ROWS) / stepRiserDist);

                char stepGlyph = '=';
                uint32_t stepColor = CRT_STAIR_BRIGHT;

                if (stepIsStair) {
                    stepGlyph = (int(stepRiserDist * 4) % 2 == 0) ? '=' : '_';
                    stepColor = (stepRiserDist < 3.0f) ? CRT_STAIR_BRIGHT : CRT_STAIR_DARK;
                } else {
                    stepGlyph = (stepFloorDiff > 0) ? '^' : 'v';
                    stepColor = CRT_LEDGE_COLOR;
                }

                for (int r = stepTop; r <= stepBottom; ++r) {
                    if (r >= 0 && r < ROWS) {
                        drawGlyph(col, r, stepGlyph, stepColor);
                    }
                }
            }
        }

        // Center Crosshair
        int cx = viewWidth / 2;
        int cy = horizon;
        drawGlyph(cx, cy, '+', 0xFF94A3B8);

        if (player.takingDamage) {[cite: 2]
            drawText(36, 28, "! ATTACKED !", RED_GOAL_BRIGHT);[cite: 2]
        }[cite: 2]

        // Live HUD
        std::string elevStr = (player.posZ > 0.7f) ? "OVERPASS [HIGH]" : ((player.posZ > 0.2f) ? "STAIRS [MID]" : "GROUND [LOW]");
        drawText(2, 2, "ELEVATION: " + elevStr + " | STEPS: " + std::to_string(totalSteps), CRT_LIGHT_MID);
        
        uint32_t hpCol = (player.health < 30.0f) ? RED_GOAL_BRIGHT : ((player.health < 60.0f) ? 0xFFF59E0B : CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(2, 4, "HEALTH: " + std::to_string(int(player.health)) + "%", hpCol);[cite: 2]

        uint32_t sanCol = (player.sanity < 30.0f) ? RED_GOAL_BRIGHT : ((player.sanity < 60.0f) ? 0xFFF59E0B : CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(2, 6, "SANITY: " + std::to_string(int(player.sanity)) + "%", sanCol);[cite: 2]

        if (currentDifficulty == DIFF_EASY) {[cite: 2]
            renderSidebarMinimap();[cite: 2]
        }[cite: 2]
    }

    void renderSidebarMinimap() {[cite: 2]
        for (int r = 0; r < ROWS; ++r) {[cite: 2]
            drawGlyph(68, r, '|', 0xFF334155);[cite: 2]
        }[cite: 2]

        int miniStartX = 72;[cite: 2]
        int miniStartY = 3;[cite: 2]

        for (int r = 0; r < MAP_H; ++r) {[cite: 2]
            for (int c = 0; c < MAP_W; ++c) {[cite: 2]
                char mapCh = ' ';[cite: 2]
                uint32_t mapCol = 0xFF1E293B;[cite: 2]

                if (worldMap[r][c].wallType == 1) {
                    mapCh = '#';[cite: 2]
                    mapCol = 0xFF475569;[cite: 2]
                } else if (worldMap[r][c].isStairs) {
                    mapCh = '=';
                    mapCol = CRT_STAIR_BRIGHT;
                } else if (worldMap[r][c].floorH > 0.7f) {
                    mapCh = '^';
                    mapCol = CRT_LIGHT_BRIGHT;
                } else if (r == startPos.y && c == startPos.x) {[cite: 2]
                    mapCh = 'S';[cite: 2]
                    mapCol = CRT_LIGHT_BRIGHT;[cite: 2]
                } else if (r == endPos.y && c == endPos.x) {[cite: 2]
                    mapCh = 'E';[cite: 2]
                    mapCol = RED_GOAL_BRIGHT;[cite: 2]
                }[cite: 2]

                drawGlyph(miniStartX + c, miniStartY + r, mapCh, mapCol);[cite: 2]
            }[cite: 2]
        }[cite: 2]

        drawGlyph(miniStartX + int(player.posX), miniStartY + int(player.posY), 'O', 0xFF38BDF8);[cite: 2]

        drawText(72, 32, "MODE: EASY (MINIMAP)", 0xFF94A3B8);[cite: 2]
        drawText(72, 34, "[=] Stairs (Climbable)", CRT_STAIR_BRIGHT);
        drawText(72, 36, "[^] Overpass Platform", CRT_LIGHT_BRIGHT);
        drawText(72, 38, "[S] Start  [E] End", 0xFF64748B);[cite: 2]
    }

    void renderTitleScreen() {[cite: 2]
        drawText(34, 12, "==============================", CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(34, 14, "     WALK ASCII 3D HORROR     ", CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(34, 16, "==============================", CRT_LIGHT_BRIGHT);[cite: 2]

        std::string diffStr = (currentDifficulty == DIFF_NORMAL) ? "NORMAL (NO MINIMAP)" : "EASY (WITH MINIMAP)";[cite: 2]
        std::string resStr = RESOLUTION_PRESETS[currentResIndex].label;

        std::string options[3] = {[cite: 2]
            "START GAME",[cite: 2]
            "DIFFICULTY: " + diffStr,[cite: 2]
            "RESOLUTION: " + resStr
        };[cite: 2]

        for (int i = 0; i < 3; ++i) {[cite: 2]
            uint32_t col = (i == menuCursor) ? CRT_LIGHT_BRIGHT : 0xFF64748B;[cite: 2]
            std::string prefix = (i == menuCursor) ? "-> " : "   ";[cite: 2]
            drawText(32, 24 + i * 4, prefix + options[i], col);[cite: 2]
        }[cite: 2]

        drawText(26, 44, "UP/DOWN: SELECT | LEFT/RIGHT: CHANGE | ENTER: START", 0xFF334155);[cite: 2]
    }

    void renderSuccessScreen() {[cite: 2]
        drawText(36, 12, "****************************", CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(36, 14, "      MAZE COMPLETED!       ", CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(36, 16, "****************************", CRT_LIGHT_BRIGHT);[cite: 2]

        drawText(34, 22, "COMPLETED LEVEL:  " + std::to_string(currentLevel), 0xFFFFFFFF);[cite: 2]
        drawText(34, 25, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFFFFFFF);[cite: 2]
        drawText(34, 28, "TIME TAKEN:       " + std::to_string(int(levelTime)) + " SECONDS", 0xFFFFFFFF);[cite: 2]
        drawText(34, 31, "REMAINING HEALTH: " + std::to_string(int(player.health)) + "%", CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(34, 34, "REMAINING SANITY: " + std::to_string(int(player.sanity)) + "%", CRT_LIGHT_BRIGHT);[cite: 2]

        drawText(28, 44, "PRESS [ENTER / SPACE] TO ADVANCE TO NEXT LEVEL", CRT_LIGHT_MID);[cite: 2]
        drawText(38, 47, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);[cite: 2]
    }

    void renderGameOverScreen() {[cite: 2]
        drawText(36, 10, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);[cite: 2]
        drawText(36, 12, "         GAME OVER          ", RED_GOAL_BRIGHT);[cite: 2]
        drawText(36, 14, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);[cite: 2]

        drawText(28, 20, deathReason, RED_GOAL_BRIGHT);[cite: 2]

        drawText(34, 26, "DIED AT LEVEL:    " + std::to_string(currentLevel), 0xFFCBD5E1);[cite: 2]
        drawText(34, 29, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFCBD5E1);[cite: 2]
        drawText(34, 32, "SURVIVED TIME:    " + std::to_string(int(levelTime)) + " SECONDS", 0xFFCBD5E1);[cite: 2]

        drawText(32, 42, "PRESS [ENTER / SPACE] TO TRY AGAIN", CRT_LIGHT_BRIGHT);[cite: 2]
        drawText(38, 45, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);[cite: 2]
    }

    void render() {[cite: 2]
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), 0xFF080C14);[cite: 2]

        if (currentState == STATE_TITLE) renderTitleScreen();[cite: 2]
        else if (currentState == STATE_PLAYING) render3DView();[cite: 2]
        else if (currentState == STATE_SUCCESS) renderSuccessScreen();[cite: 2]
        else if (currentState == STATE_GAMEOVER) renderGameOverScreen();[cite: 2]

        SDL_UpdateTexture(screenTexture, nullptr, pixelBuffer.data(), NATIVE_WIDTH * sizeof(uint32_t));[cite: 2]
        SDL_RenderClear(renderer);[cite: 2]
        SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);[cite: 2]
        SDL_RenderPresent(renderer);[cite: 2]
    }

    void run() {[cite: 2]
        uint32_t previousTime = SDL_GetTicks();[cite: 2]
        double lag = 0.0;[cite: 2]

        while (isRunning) {[cite: 2]
            uint32_t currentTime = SDL_GetTicks();[cite: 2]
            lag += static_cast<double>(currentTime - previousTime);[cite: 2]
            previousTime = currentTime;[cite: 2]

            handleEvents();[cite: 2]

            while (lag >= FIXED_TIMESTEP) {[cite: 2]
                update(FIXED_TIMESTEP / 1000.0);[cite: 2]
                lag -= FIXED_TIMESTEP;[cite: 2]
            }[cite: 2]

            render();[cite: 2]
            SDL_Delay(1);[cite: 2]
        }[cite: 2]
    }

    void cleanup() {[cite: 2]
        setCaptureMouse(false);
        if (audioDevice != 0) SDL_CloseAudioDevice(audioDevice);[cite: 2]
        if (screenTexture) SDL_DestroyTexture(screenTexture);[cite: 2]
        if (renderer) SDL_DestroyRenderer(renderer);[cite: 2]
        if (window) SDL_DestroyWindow(window);[cite: 2]
        SDL_Quit();[cite: 2]
    }
};

int main(int argc, char* argv[]) {[cite: 2]
    WalkAsciiElevationEngine engine;
    if (engine.init()) engine.run();
    engine.cleanup();[cite: 2]
    return 0;[cite: 2]
}