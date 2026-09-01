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

// Virtual Text Grid Resolution (Native 800x480)
constexpr int CHAR_W = 8;
constexpr int CHAR_H = 8;
constexpr int TOTAL_COLS = 100;
constexpr int ROWS = 60;
constexpr int NATIVE_WIDTH = TOTAL_COLS * CHAR_W;  // 800
constexpr int NATIVE_HEIGHT = ROWS * CHAR_H;       // 480

constexpr double FIXED_TIMESTEP = 1000.0 / 60.0;

// World Map Dimensions
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

enum GameState {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_SUCCESS,
    STATE_GAMEOVER
};

enum Difficulty {
    DIFF_NORMAL = 0, // Fullscreen 3D view
    DIFF_EASY   = 1  // Includes 2D minimap sidebar
};

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
    float monsterPhase = 0.0f;
    float sanity = 100.0f;
    float monsterDist = 20.0f;
    bool isChasing = false;
    bool inGame = false;
};

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioState* audio = static_cast<AudioState*>(userdata);
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);

    for (int i = 0; i < samples; ++i) {
        if (!audio->inGame) {
            buffer[i] = 0;
            continue;
        }

        // Sub-bass 42Hz drone
        audio->ambientPhase += (42.0f * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
        if (audio->ambientPhase > 2.0f * 3.14159265f) audio->ambientPhase -= 2.0f * 3.14159265f;
        float ambient = std::sin(audio->ambientPhase) * 0.08f;

        // Dynamic Heartbeat based on Sanity
        float heartBPM = 1.0f + (100.0f - audio->sanity) / 100.0f * 2.0f;
        audio->heartbeatPhase += (heartBPM * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
        if (audio->heartbeatPhase > 2.0f * 3.14159265f) audio->heartbeatPhase -= 2.0f * 3.14159265f;

        float beatEnv = 0.0f;
        float cyclePos = audio->heartbeatPhase / (2.0f * 3.14159265f);
        if (cyclePos < 0.15f) {
            beatEnv = std::sin(cyclePos / 0.15f * 3.14159265f);
        } else if (cyclePos > 0.22f && cyclePos < 0.35f) {
            beatEnv = std::sin((cyclePos - 0.22f) / 0.13f * 3.14159265f) * 0.7f;
        }
        float heartbeat = std::sin(audio->heartbeatPhase * 40.0f) * beatEnv * (0.35f + (100.0f - audio->sanity) / 100.0f * 0.50f);

        // Monster Growl Audio (triggers when within 10 units)
        float monsterAudio = 0.0f;
        if (audio->monsterDist < 10.0f) {
            float proxVol = 1.0f - (audio->monsterDist / 10.0f);
            float breathFreq = audio->isChasing ? 2.5f : 0.8f;
            audio->monsterPhase += (breathFreq * 2.0f * 3.14159265f) / AUDIO_SAMPLE_RATE;
            if (audio->monsterPhase > 2.0f * 3.14159265f) audio->monsterPhase -= 2.0f * 3.14159265f;

            float noise = ((rand() % 2000) / 1000.0f - 1.0f);
            monsterAudio = noise * (std::sin(audio->monsterPhase) * 0.5f + 0.5f) * proxVol * 0.4f;
        }

        buffer[i] = static_cast<int16_t>(std::clamp(ambient + heartbeat + monsterAudio, -1.0f, 1.0f) * 32767.0f);
    }
}

class WalkAsciiHorrorEngine {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* screenTexture = nullptr;
    SDL_AudioDeviceID audioDevice = 0;
    std::vector<uint32_t> pixelBuffer;
    bool isRunning = false;

    GameState currentState = STATE_TITLE;
    Difficulty currentDifficulty = DIFF_NORMAL;
    int currentScale = 2;
    int menuCursor = 0;

    AudioState audioState;
    int worldMap[MAP_H][MAP_W];
    Point startPos;
    Point endPos;

    int currentLevel = 1;
    int totalSteps = 0;
    float levelTime = 0.0f;
    std::string deathReason = "";

    struct Player {
        float posX = 1.5f;
        float posY = 1.5f;
        float dirX = 1.0f;
        float dirY = 0.0f;
        float planeX = 0.0f;
        float planeY = 0.66f;
        float moveSpeed = 3.2f;
        float rotSpeed = 2.6f;

        int forward = 0;
        int rotate = 0;
        float stepAccumulator = 0.0f;
        
        float sanity = 100.0f;
        float health = 100.0f;
        bool takingDamage = false;
    } player;

    struct Monster {
        float x = 12.5f;
        float y = 12.5f;
        float speed = 1.8f;
        bool isChasing = false;
    } stalker;

    void updateWindowScale() {
        if (window) {
            SDL_SetWindowSize(window, NATIVE_WIDTH * currentScale, NATIVE_HEIGHT * currentScale);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
    }

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
        worldMap[endPos.y][endPos.x] = 2;

        player.posX = startPos.x + 0.5f;
        player.posY = startPos.y + 0.5f;
        player.dirX = 1.0f;
        player.dirY = 0.0f;
        player.planeX = 0.0f;
        player.planeY = 0.66f;
        player.stepAccumulator = 0.0f;

        stalker.x = MAP_W / 2 + 0.5f;
        stalker.y = MAP_H / 2 + 0.5f;
        stalker.isChasing = false;
    }

    void startNewGame() {
        currentLevel = 1;
        totalSteps = 0;
        levelTime = 0.0f;
        player.sanity = 100.0f;
        player.health = 100.0f;
        generateMaze();
        currentState = STATE_PLAYING;

        SDL_LockAudioDevice(audioDevice);
        audioState.inGame = true;
        audioState.sanity = 100.0f;
        audioState.monsterDist = 20.0f;
        SDL_UnlockAudioDevice(audioDevice);
    }

    void nextLevel() {
        currentLevel++;
        player.sanity = std::min(100.0f, player.sanity + 30.0f);
        player.health = std::min(100.0f, player.health + 30.0f);
        generateMaze();
        currentState = STATE_PLAYING;

        SDL_LockAudioDevice(audioDevice);
        audioState.inGame = true;
        SDL_UnlockAudioDevice(audioDevice);
    }

    void drawGlyph(int col, int row, char c, uint32_t fgColor) {
        if (c < 32 || c > 127) return;
        const uint8_t* glyph = FONT_8X8[c - 32];
        int startX = col * CHAR_W;
        int startY = row * CHAR_H;

        for (int y = 0; y < CHAR_H; ++y) {
            for (int x = 0; x < CHAR_W; ++x) {
                if ((glyph[y] >> (7 - x)) & 1) {
                    pixelBuffer[(startY + y) * NATIVE_WIDTH + (startX + x)] = fgColor;
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

public:
    bool init() {
        srand(static_cast<unsigned int>(time(nullptr)));

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) return false;

        window = SDL_CreateWindow(
            "Walk ASCII 3D Horror",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            NATIVE_WIDTH * currentScale, NATIVE_HEIGHT * currentScale,
            SDL_WINDOW_SHOWN
        );

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        screenTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            NATIVE_WIDTH, NATIVE_HEIGHT
        );

        pixelBuffer.resize(NATIVE_WIDTH * NATIVE_HEIGHT, 0xFF000000);

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

        isRunning = true;
        return true;
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) isRunning = false;

            if (event.type == SDL_KEYDOWN) {
                if (currentState == STATE_TITLE) {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
                        menuCursor = (menuCursor - 1 + 3) % 3;
                    }
                    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
                        menuCursor = (menuCursor + 1) % 3;
                    }
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                        if (menuCursor == 0) startNewGame();
                        else if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                        else if (menuCursor == 2) {
                            currentScale = (currentScale % 4) + 1;
                            updateWindowScale();
                        }
                    }
                    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
                        if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                        if (menuCursor == 2) {
                            currentScale = (event.key.keysym.sym == SDLK_RIGHT) ? ((currentScale % 4) + 1) : ((currentScale - 2 + 4) % 4 + 1);
                            updateWindowScale();
                        }
                    }
                }
                else if (currentState == STATE_SUCCESS) {
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) nextLevel();
                    if (event.key.keysym.sym == SDLK_ESCAPE) currentState = STATE_TITLE;
                }
                else if (currentState == STATE_GAMEOVER) {
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) startNewGame();
                    if (event.key.keysym.sym == SDLK_ESCAPE) currentState = STATE_TITLE;
                }
                else if (currentState == STATE_PLAYING) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) currentState = STATE_TITLE;
                }
            }
        }

        if (currentState == STATE_PLAYING) {
            const uint8_t* state = SDL_GetKeyboardState(NULL);
            player.forward = 0;
            player.rotate = 0;

            if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])    player.forward += 1;
            if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])  player.forward -= 1;
            if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])  player.rotate -= 1;
            if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) player.rotate += 1;
        }
    }

    void update(double dt) {
        if (currentState != STATE_PLAYING) return;

        float dtSec = static_cast<float>(dt);
        levelTime += dtSec;
        player.takingDamage = false;

        // 1. Rotation
        if (player.rotate != 0) {
            float rot = player.rotate * player.rotSpeed * dtSec;
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(rot) - player.dirY * sin(rot);
            player.dirY = oldDirX * sin(rot) + player.dirY * cos(rot);

            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(rot) - player.planeY * sin(rot);
            player.planeY = oldPlaneX * sin(rot) + player.planeY * cos(rot);
        }

        // 2. Movement & Step Accumulation
        if (player.forward != 0) {
            float step = player.forward * player.moveSpeed * dtSec;
            float buf = (step > 0) ? 0.35f : -0.35f;

            float prevX = player.posX;
            float prevY = player.posY;

            if (worldMap[int(player.posY)][int(player.posX + player.dirX * (step + buf))] != 1)
                player.posX += player.dirX * step;
            if (worldMap[int(player.posY + player.dirY * (step + buf))][int(player.posX)] != 1)
                player.posY += player.dirY * step;

            player.stepAccumulator += std::hypot(player.posX - prevX, player.posY - prevY);
            if (player.stepAccumulator >= 1.0f) {
                totalSteps++;
                player.stepAccumulator = 0.0f;
            }
        }

        // 3. Stalker AI, Proximity Audio, Attack & Recovery
        float distToMonster = std::hypot(player.posX - stalker.x, player.posY - stalker.y);

        if (distToMonster < 8.5f) {
            stalker.isChasing = true;

            // Move towards player
            float dx = (player.posX - stalker.x) / distToMonster;
            float dy = (player.posY - stalker.y) / distToMonster;

            float nx = stalker.x + dx * stalker.speed * dtSec;
            float ny = stalker.y + dy * stalker.speed * dtSec;

            if (worldMap[int(ny)][int(nx)] == 0) {
                stalker.x = nx;
                stalker.y = ny;
            }

            // Proximity Fear / Noise drains sanity proportionally
            player.sanity -= (6.0f / std::max(1.0f, distToMonster)) * dtSec;

            // Physical Attack within close contact (< 1.1 units)
            if (distToMonster < 1.1f) {
                player.health -= 28.0f * dtSec; // Monster claws attack
                player.takingDamage = true;
            }
        } else {
            stalker.isChasing = false;

            // Slow ambient darkness drain calibrated for 6-12 min exploration
            player.sanity -= 0.08f * dtSec;

            // Outrun Recovery: If far from monster (> 12 units), recover sanity and health
            if (distToMonster > 12.0f) {
                player.sanity = std::min(100.0f, player.sanity + 1.0f * dtSec);
                player.health = std::min(100.0f, player.health + 0.8f * dtSec);
            }
        }

        // 4. Defeat Conditions
        player.sanity = std::max(0.0f, player.sanity);
        player.health = std::max(0.0f, player.health);

        if (player.sanity <= 0.0f) {
            deathReason = "LOST TO THE TERROR (SANITY DEPLETED)";
            currentState = STATE_GAMEOVER;
            SDL_LockAudioDevice(audioDevice);
            audioState.inGame = false;
            SDL_UnlockAudioDevice(audioDevice);
            return;
        }

        if (player.health <= 0.0f) {
            deathReason = "SLAIN BY THE STALKER (HEALTH DEPLETED)";
            currentState = STATE_GAMEOVER;
            SDL_LockAudioDevice(audioDevice);
            audioState.inGame = false;
            SDL_UnlockAudioDevice(audioDevice);
            return;
        }

        // Win Condition
        if (int(player.posX) == endPos.x && int(player.posY) == endPos.y) {
            currentState = STATE_SUCCESS;
            SDL_LockAudioDevice(audioDevice);
            audioState.inGame = false;
            SDL_UnlockAudioDevice(audioDevice);
            return;
        }

        // Sync Audio Telemetry
        SDL_LockAudioDevice(audioDevice);
        audioState.sanity = player.sanity;
        audioState.monsterDist = distToMonster;
        audioState.isChasing = stalker.isChasing;
        SDL_UnlockAudioDevice(audioDevice);
    }

    void render3DView() {
        int viewWidth = (currentDifficulty == DIFF_EASY) ? 68 : TOTAL_COLS;

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
                    if (worldMap[mapY][mapX] > 0) hit = worldMap[mapY][mapX];
                }
            }

            if (side == 0) perpWallDist = (sideDistX - deltaDistX);
            else           perpWallDist = (sideDistY - deltaDistY);

            int lineHeight = int(ROWS / perpWallDist);
            int drawStart = -lineHeight / 2 + ROWS / 2;
            int drawEnd = lineHeight / 2 + ROWS / 2;

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
        }

        // Damage Flashing HUD Warning
        if (player.takingDamage) {
            drawText(36, 28, "! ATTACKED !", RED_GOAL_BRIGHT);
        }

        // Live HUD
        drawText(2, 2, "LEVEL: " + std::to_string(currentLevel) + " | STEPS: " + std::to_string(totalSteps), CRT_LIGHT_MID);
        
        uint32_t hpCol = (player.health < 30.0f) ? RED_GOAL_BRIGHT : ((player.health < 60.0f) ? 0xFFF59E0B : CRT_LIGHT_BRIGHT);
        drawText(2, 4, "HEALTH: " + std::to_string(int(player.health)) + "%", hpCol);

        uint32_t sanCol = (player.sanity < 30.0f) ? RED_GOAL_BRIGHT : ((player.sanity < 60.0f) ? 0xFFF59E0B : CRT_LIGHT_BRIGHT);
        drawText(2, 6, "SANITY: " + std::to_string(int(player.sanity)) + "%", sanCol);

        if (currentDifficulty == DIFF_EASY) {
            renderSidebarMinimap();
        }
    }

    void renderSidebarMinimap() {
        for (int r = 0; r < ROWS; ++r) {
            drawGlyph(68, r, '|', 0xFF334155);
        }

        int miniStartX = 72;
        int miniStartY = 3;

        for (int r = 0; r < MAP_H; ++r) {
            for (int c = 0; c < MAP_W; ++c) {
                char mapCh = ' ';
                uint32_t mapCol = 0xFF1E293B;

                if (worldMap[r][c] == 1) {
                    mapCh = '#';
                    mapCol = 0xFF475569;
                } else if (r == startPos.y && c == startPos.x) {
                    mapCh = 'S';
                    mapCol = CRT_LIGHT_BRIGHT;
                } else if (r == endPos.y && c == endPos.x) {
                    mapCh = 'E';
                    mapCol = RED_GOAL_BRIGHT;
                }

                drawGlyph(miniStartX + c, miniStartY + r, mapCh, mapCol);
            }
        }

        drawGlyph(miniStartX + int(player.posX), miniStartY + int(player.posY), 'O', 0xFF38BDF8);

        drawText(72, 32, "MODE: EASY (MINIMAP)", 0xFF94A3B8);
        drawText(72, 35, "[S] Start  [E] End", 0xFF64748B);
        drawText(72, 37, "[O] Player Position", 0xFF64748B);
    }

    void renderTitleScreen() {
        drawText(34, 12, "==============================", CRT_LIGHT_BRIGHT);
        drawText(34, 14, "     WALK ASCII 3D HORROR     ", CRT_LIGHT_BRIGHT);
        drawText(34, 16, "==============================", CRT_LIGHT_BRIGHT);

        std::string diffStr = (currentDifficulty == DIFF_NORMAL) ? "NORMAL (NO MINIMAP)" : "EASY (WITH MINIMAP)";
        std::string resStr = std::to_string(NATIVE_WIDTH * currentScale) + "x" + std::to_string(NATIVE_HEIGHT * currentScale) + " (" + std::to_string(currentScale) + "X)";

        std::string options[3] = {
            "START GAME",
            "DIFFICULTY: " + diffStr,
            "RESOLUTION: " + resStr
        };

        for (int i = 0; i < 3; ++i) {
            uint32_t col = (i == menuCursor) ? CRT_LIGHT_BRIGHT : 0xFF64748B;
            std::string prefix = (i == menuCursor) ? "-> " : "   ";
            drawText(32, 24 + i * 4, prefix + options[i], col);
        }

        drawText(26, 44, "UP/DOWN: SELECT | LEFT/RIGHT: CHANGE | ENTER: START", 0xFF334155);
    }

    void renderSuccessScreen() {
        drawText(36, 12, "****************************", CRT_LIGHT_BRIGHT);
        drawText(36, 14, "      MAZE COMPLETED!       ", CRT_LIGHT_BRIGHT);
        drawText(36, 16, "****************************", CRT_LIGHT_BRIGHT);

        drawText(34, 22, "COMPLETED LEVEL:  " + std::to_string(currentLevel), 0xFFFFFFFF);
        drawText(34, 25, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFFFFFFF);
        drawText(34, 28, "TIME TAKEN:       " + std::to_string(int(levelTime)) + " SECONDS", 0xFFFFFFFF);
        drawText(34, 31, "REMAINING HEALTH: " + std::to_string(int(player.health)) + "%", CRT_LIGHT_BRIGHT);
        drawText(34, 34, "REMAINING SANITY: " + std::to_string(int(player.sanity)) + "%", CRT_LIGHT_BRIGHT);

        drawText(28, 44, "PRESS [ENTER / SPACE] TO ADVANCE TO NEXT LEVEL", CRT_LIGHT_MID);
        drawText(38, 47, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);
    }

    void renderGameOverScreen() {
        drawText(36, 10, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);
        drawText(36, 12, "         GAME OVER          ", RED_GOAL_BRIGHT);
        drawText(36, 14, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);

        drawText(28, 20, deathReason, RED_GOAL_BRIGHT);

        drawText(34, 26, "DIED AT LEVEL:    " + std::to_string(currentLevel), 0xFFCBD5E1);
        drawText(34, 29, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFCBD5E1);
        drawText(34, 32, "SURVIVED TIME:    " + std::to_string(int(levelTime)) + " SECONDS", 0xFFCBD5E1);

        drawText(32, 42, "PRESS [ENTER / SPACE] TO TRY AGAIN", CRT_LIGHT_BRIGHT);
        drawText(38, 45, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);
    }

    void render() {
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), 0xFF080C14);

        if (currentState == STATE_TITLE) {
            renderTitleScreen();
        } else if (currentState == STATE_PLAYING) {
            render3DView();
        } else if (currentState == STATE_SUCCESS) {
            renderSuccessScreen();
        } else if (currentState == STATE_GAMEOVER) {
            renderGameOverScreen();
        }

        SDL_UpdateTexture(screenTexture, nullptr, pixelBuffer.data(), NATIVE_WIDTH * sizeof(uint32_t));
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
    WalkAsciiHorrorEngine engine;
    if (engine.init()) engine.run();
    engine.cleanup();
    return 0;
}