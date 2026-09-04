#include "Game.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <stack>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- FONT DATA ---
const uint8_t FONT_8X8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00},
    {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, {0x00,0x66,0xAC,0xD8,0x36,0x6A,0x00,0x00}, {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00},
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},
    {0x7C,0xC6,0xCE,0xD6,0xE6,0xC6,0x7C,0x00}, {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, {0x7C,0xC6,0x06,0x1C,0x30,0x66,0xFE,0x00}, {0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00},
    {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00}, {0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00}, {0x7C,0xC6,0xC0,0xFC,0xC6,0xC6,0x7C,0x00}, {0xFE,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},
    {0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00}, {0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00}, {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30},
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00}, {0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00},
    {0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00}, {0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, {0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00}, {0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00},
    {0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00}, {0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00}, {0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00}, {0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00},
    {0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, {0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00}, {0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00},
    {0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00}, {0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00}, {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00}, {0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00},
    {0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00}, {0x7C,0xC6,0xC6,0xC6,0xC6,0xCE,0x7C,0x06}, {0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE6,0x00}, {0x7C,0xC6,0x60,0x38,0x0C,0xC6,0x7C,0x00},
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, {0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, {0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x00}, {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00},
    {0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00}, {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}, {0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00}, {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},
    {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}, {0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x78,0x0C,0x7C,0xCC,0x76,0x00}, {0xE0,0x60,0x60,0x7C,0x66,0x66,0xDC,0x00}, {0x00,0x00,0x78,0xCC,0xC0,0xCC,0x78,0x00},
    {0x1C,0x0C,0x0C,0x7C,0xCC,0xCC,0x76,0x00}, {0x00,0x00,0x78,0xCC,0xFC,0xC0,0x78,0x00}, {0x38,0x6C,0x60,0xF0,0x60,0x60,0xF0,0x00}, {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0xF8},
    {0xE0,0x60,0x6C,0x76,0x66,0x66,0xE6,0x00}, {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00}, {0x06,0x00,0x06,0x06,0x06,0x66,0x66,0x3C}, {0xE0,0x60,0x66,0x6C,0x78,0x6C,0xE6,0x00},
    {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, {0x00,0x00,0xEC,0xFE,0xD6,0xD6,0xD6,0x00}, {0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x00}, {0x00,0x00,0x78,0xCC,0xCC,0xCC,0x78,0x00},
    {0x00,0x00,0xDC,0x66,0x66,0x7C,0x60,0xF0}, {0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0x1E}, {0x00,0x00,0xDC,0x76,0x66,0x60,0xF0,0x00}, {0x00,0x00,0x7C,0xC0,0x78,0x0C,0xF8,0x00},
    {0x10,0x30,0x7C,0x30,0x30,0x34,0x18,0x00}, {0x00,0x00,0xCC,0xCC,0xCC,0xCC,0x76,0x00}, {0x00,0x00,0xCC,0xCC,0xCC,0x78,0x30,0x00}, {0x00,0x00,0xC6,0xD6,0xD6,0xFE,0x6C,0x00},
    {0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00}, {0x00,0x00,0xC6,0xC6,0xC6,0x7E,0x06,0xFC}, {0x00,0x00,0xFC,0x98,0x30,0x64,0xFC,0x00}, {0x1C,0x30,0x30,0xE0,0x30,0x30,0x1C,0x00},
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, {0xE0,0x30,0x30,0x1C,0x30,0x30,0xE0,0x00}, {0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

const std::vector<std::string> BREAD_SPRITE_DATA = { /* ... data ... */ };
const std::vector<std::string> MEDS_SPRITE_DATA = { /* ... data ... */ };
const std::vector<std::string> PEBBLE_SPRITE_DATA = { /* ... data ... */ };
const std::vector<std::string> OIL_SPRITE_DATA = { /* ... data ... */ };

Game::Game() : spriteBread(BREAD_SPRITE_DATA), spriteMeds(MEDS_SPRITE_DATA), spritePebble(PEBBLE_SPRITE_DATA), spriteOil(OIL_SPRITE_DATA) {}

bool Game::init() {
    srand(static_cast<unsigned int>(time(nullptr)));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) { return false; }
    window = SDL_CreateWindow("Walk ASCII 3D Horror", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESOLUTION_PRESETS[currentResIndex].width, RESOLUTION_PRESETS[currentResIndex].height, SDL_WINDOW_SHOWN);
    if (!window) { return false; }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) { return false; }
    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, NATIVE_WIDTH, NATIVE_HEIGHT);
    if (!screenTexture) { return false; }
    pixelBuffer.resize(NATIVE_WIDTH * NATIVE_HEIGHT, 0xFF000000);
    SDL_AudioSpec wantedSpec;
    SDL_zero(wantedSpec);
    wantedSpec.freq = AUDIO_SAMPLE_RATE;
    wantedSpec.format = AUDIO_S16SYS;
    wantedSpec.channels = 1;
    wantedSpec.samples = AUDIO_BUFFER_SIZE;
    wantedSpec.callback = AudioSystem::audioCallback;
    wantedSpec.userdata = &audioSystem;
    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &wantedSpec, nullptr, 0);
    if (audioDevice != 0) { SDL_PauseAudioDevice(audioDevice, 0); }
    initializeSprites();
    isRunning = true;
    return true;
}

void Game::run() {
    uint32_t previousTime = SDL_GetTicks();
    double lag = 0.0;
    while (isRunning) {
        uint32_t currentTime = SDL_GetTicks();
        double elapsed = static_cast<double>(currentTime - previousTime);
        previousTime = currentTime;
        lag += elapsed;
        handleEvents();
        while (lag >= FIXED_TIMESTEP) {
            update(FIXED_TIMESTEP / 1000.0);
            lag -= FIXED_TIMESTEP;
        }
        render();
        SDL_Delay(1);
    }
}

void Game::cleanup() {
    setCaptureMouse(false);
    if (audioDevice != 0) SDL_CloseAudioDevice(audioDevice);
    if (screenTexture) SDL_DestroyTexture(screenTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;

        if (currentState == STATE_PLAYING && event.type == SDL_MOUSEMOTION) {
            float rotAngle = event.motion.xrel * (0.0005f + player.mouseSensitivity * 0.004f);
            float oldDirX = player.dirX;
            player.dirX = player.dirX * std::cos(rotAngle) - player.dirY * std::sin(rotAngle);
            player.dirY = oldDirX * std::sin(rotAngle) + player.dirY * std::cos(rotAngle);
            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * std::cos(rotAngle) - player.planeY * std::sin(rotAngle);
            player.planeY = oldPlaneX * std::sin(rotAngle) + player.planeY * std::cos(rotAngle);
            player.pitch -= event.motion.yrel * 0.12f;
            player.pitch = std::clamp(player.pitch, -22.0f, 22.0f);
        }
        
        if (event.type == SDL_KEYDOWN) {
            if (currentState == STATE_TITLE) {
                if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) menuCursor = (menuCursor - 1 + 6) % 6;
                if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) menuCursor = (menuCursor + 1) % 6;
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                    if (menuCursor == 0) startNewGame();
                    else if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                    else if (menuCursor == 4) { currentResIndex = (currentResIndex + 1) % RESOLUTION_PRESETS.size(); updateWindowScale(); }
                    else if (menuCursor == 5) isRunning = false;
                }
                if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
                    if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                    if (menuCursor == 2) audioSystem.masterVolume = std::max(0.0f, audioSystem.masterVolume - 0.05f);
                    if (menuCursor == 3) player.mouseSensitivity = std::max(0.0f, player.mouseSensitivity - 0.05f);
                    if (menuCursor == 4) { currentResIndex = (currentResIndex - 1 + RESOLUTION_PRESETS.size()) % RESOLUTION_PRESETS.size(); updateWindowScale(); }
                }
                if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
                    if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                    if (menuCursor == 2) audioSystem.masterVolume = std::min(1.0f, audioSystem.masterVolume + 0.05f);
                    if (menuCursor == 3) player.mouseSensitivity = std::min(1.0f, player.mouseSensitivity + 0.05f);
                    if (menuCursor == 4) { currentResIndex = (currentResIndex + 1) % RESOLUTION_PRESETS.size(); updateWindowScale(); }
                }
            } else if (currentState == STATE_PLAYING) {
                if (event.key.keysym.sym == SDLK_ESCAPE) { currentState = STATE_PAUSED; setCaptureMouse(false); }
                else if (event.key.keysym.sym == SDLK_F10) {
                    nextLevel();
                }
                else if (event.key.keysym.sym == SDLK_f) {
                    if (player.lanternFuel > 0) {
                        player.lanternOn = !player.lanternOn;
                    }
                } else if (event.key.keysym.sym == SDLK_e) {
                    bool pickedUp = false;
                    for (auto it = itemsInWorld.begin(); it != itemsInWorld.end(); ++it) {
                        if (std::hypot(player.posX - it->x, player.posY - it->y) < 1.5f) {
                            for (int i = 0; i < 3; ++i) {
                                if (player.inventory[i] == ITEM_NONE) {
                                    player.inventory[i] = it->type;
                                    audioSystem.triggerItemSound(it->type);
                                    itemsInWorld.erase(it);
                                    pickedUp = true;
                                    goto item_loop_end;
                                }
                            }
                        }
                    }
                    item_loop_end:;

                    if (!pickedUp) {
                        int checkX = static_cast<int>(player.posX + player.dirX);
                        int checkY = static_cast<int>(player.posY + player.dirY);
                        if (checkX >= 0 && checkX < MAP_W && checkY >= 0 && checkY < MAP_H) {
                            if (worldMap[checkY][checkX].wallType == 5) { worldMap[checkY][checkX].wallType = 6; } // Open door
                            else if (worldMap[checkY][checkX].wallType == 6) { worldMap[checkY][checkX].wallType = 5; } // Close door
                            else if(worldMap[checkY][checkX].wallType == 4) {
                                currentMessage = "IT CANNOT SEE YOU IF YOU HIDE IN THE VENTS...";
                                currentState = STATE_MESSAGE;
                            }
                        }
                    }

                } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_3) {
                    int slot = event.key.keysym.sym - SDLK_1;
                    if (player.inventory[slot] != ITEM_NONE) {
                        ItemType type = player.inventory[slot];
                        player.inventory[slot] = ITEM_NONE;
                        audioSystem.triggerItemSound(type);

                        if (type == ITEM_PEBBLE) {
                            Projectile p; p.x = player.posX; p.y = player.posY; p.z = player.eyeHeight;
                            p.vx = player.dirX * 8.0f; p.vy = player.dirY * 8.0f; p.vz = 2.0f;
                            p.type = type; p.active = true;
                            activeProjectiles.push_back(p);
                        } else if (type == ITEM_BREAD) {
                            player.health = std::min(100.0f, player.health + 40.0f);
                        } else if (type == ITEM_MEDS) {
                            player.sanity = std::min(100.0f, player.sanity + 50.0f);
                        } else if (type == ITEM_OIL) {
                            player.lanternFuel = std::min(LANTERN_FUEL_MAX, player.lanternFuel + OIL_TIN_RESTORE_AMOUNT);
                        }
                    }
                }
            } else if (currentState == STATE_PAUSED) {
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_r) { currentState = STATE_PLAYING; setCaptureMouse(true); }
                else if (event.key.keysym.sym == SDLK_q) { currentState = STATE_TITLE; setCaptureMouse(false); }
            } else if (currentState == STATE_SUCCESS || currentState == STATE_GAMEOVER) {
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                    if (currentState == STATE_SUCCESS) nextLevel(); else startNewGame();
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) { currentState = STATE_TITLE; setCaptureMouse(false); }
            } else if (currentState == STATE_MESSAGE) {
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_e) {
                    currentState = STATE_PLAYING;
                }
            }
        }

        const uint8_t* state = SDL_GetKeyboardState(NULL);
        if (currentState == STATE_PLAYING) {
            player.forward = (state[SDL_SCANCODE_W] ? 1 : 0) - (state[SDL_SCANCODE_S] ? 1 : 0);
            player.strafe = (state[SDL_SCANCODE_D] ? 1 : 0) - (state[SDL_SCANCODE_A] ? 1 : 0);
            player.isSprinting = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
            player.isCrouching = state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL];
        }
    }
}

void Game::update(double dt) {
    if (currentState != STATE_PLAYING) {
        audioSystem.inGame = false;
        return;
    }
    audioSystem.inGame = true;
    
    player.update(dt, worldMap);
    stalker.update(dt, player, worldMap);
    mistEnemy.update(dt, player, worldMap);

    levelTime += dt;
    if(player.stepAccumulator >= 1.0f) {
        totalSteps++;
        player.stepAccumulator = 0.0f;
    }

    for (auto& proj : activeProjectiles) {
        if (!proj.active) continue;
        proj.x += proj.vx * dt; proj.y += proj.vy * dt;
        proj.z += proj.vz * dt; proj.vz -= 20.0f * dt;
        if (worldMap[int(proj.y)][int(proj.x)].wallType == 1) proj.active = false;
        if (proj.z <= 0.0f) {
            proj.active = false;
            if (proj.type == ITEM_PEBBLE) {
                stalker.setInvestigateTarget(proj.x, proj.y, worldMap);
            }
        }
    }
    activeProjectiles.erase(std::remove_if(activeProjectiles.begin(), activeProjectiles.end(), [](const Projectile& p){ return !p.active; }), activeProjectiles.end());

    if(player.sanity < 30.0f && !ghost.active && (rand() % 10000) < 5) {
        ghost.active = true;
        ghost.x = player.posX + player.dirX * (5.0f + (rand() % 5));
        ghost.y = player.posY + player.dirY * (5.0f + (rand() % 5));
        ghost.timer = 0.3f;
    }
    if(ghost.active) {
        ghost.timer -= dt;
        if(ghost.timer <= 0.0f) ghost.active = false;
    }

    audioSystem.isHiding = (worldMap[int(player.posY)][int(player.posX)].wallType == 3);
    audioSystem.sanity = player.sanity;
    audioSystem.corruption = corruptionLevel;
    audioSystem.isMoving = player.isMoving();
    audioSystem.isSprinting = player.isSprinting;
    audioSystem.isChasing = stalker.isChasing;
    float stalkerDist = stalker.active ? std::hypot(player.posX - stalker.x, player.posY - stalker.y) : 999.0f;
    float mistDist = mistEnemy.active ? std::hypot(player.posX - mistEnemy.x, player.posY - mistEnemy.y) : 999.0f;
    audioSystem.closestEnemyDist = std::min(stalkerDist, mistDist);

    if (player.health <= 0) {
        deathReason = "YOUR BODY FAILED";
        currentState = STATE_GAMEOVER;
    } else if (player.sanity <= 0) {
        deathReason = "YOUR MIND SHATTERED";
        currentState = STATE_GAMEOVER;
    } else if ((int)player.posX == endPos.x && (int)player.posY == endPos.y) {
        currentState = STATE_SUCCESS;
    }
    if (currentState != STATE_PLAYING) setCaptureMouse(false);
}

void Game::render() {
    std::fill(pixelBuffer.begin(), pixelBuffer.end(), 0xFF080C14);

    if(currentState == STATE_TITLE) renderTitleScreen();
    else if(currentState == STATE_PAUSED) { render3DView(); renderPauseScreen(); }
    else if(currentState == STATE_JUMPSCARE) renderJumpscareScreen();
    else if(currentState == STATE_SUCCESS) renderSuccessScreen();
    else if(currentState == STATE_GAMEOVER) renderGameOverScreen();
    else if(currentState == STATE_MESSAGE) { render3DView(); renderMessageScreen(); }
    else render3DView();

    SDL_UpdateTexture(screenTexture, nullptr, pixelBuffer.data(), NATIVE_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Game::render3DView() {
    int viewWidth = (currentDifficulty == DIFF_EASY) ? 100 : TOTAL_COLS;
    int horizon = int(ROWS / 2.0f + player.pitch);
    float vignette = 1.0f - (100.0f - player.sanity) / 150.0f;
    
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
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX; mapX += stepX; side = 0;
            } else {
                sideDistY += deltaDistY; mapY += stepY; side = 1;
            }
            if (mapX >= 0 && mapX < MAP_W && mapY >= 0 && mapY < MAP_H) {
                if (worldMap[mapY][mapX].wallType > 0 && worldMap[mapY][mapX].wallType != 6) { // Treat open doors as empty
                    hit = worldMap[mapY][mapX].wallType;
                }
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
        if (activeCorr > 0.5f && (rand() % 100) < int(activeCorr * 20)) {
            vOffset = (rand() % 5) - 2; 
        }

        // Floor
        for (int r = horizon + 1; r < ROWS; ++r) {
            float p = r - horizon;
            float straightDist = (ROWS * player.eyeHeight) / p;
            float currentFloorX = player.posX + rayDirX * straightDist;
            float currentFloorY = player.posY + rayDirY * straightDist;
            int fTileX = std::clamp(int(currentFloorX), 0, MAP_W - 1);
            int fTileY = std::clamp(int(currentFloorY), 0, MAP_H - 1);
            float vis = calculateVisibility(col, r, straightDist, viewWidth);
            if (vis <= 0.02f) continue;
            uint32_t floorColor = getWallColor(straightDist, 0);
            char floorGlyph = (straightDist < 8.0f && ((fTileX + fTileY) % 2 == 0) && (col % 2 == 0)) ? '.' : ' ';
            if (player.sanity < 50.0f && floorGlyph != ' ' && (rand() % 100) < 5) floorGlyph = ".,;`'"[rand() % 5];
            if (floorGlyph != ' ') {
                float v = std::clamp(1.0f - (r - ROWS/2.0f) / (ROWS/2.0f), 0.0f, 1.0f);
                uint32_t finalFloorColor = applyShadow(floorColor, vis, vignette * v);
                drawGlyphFine(col, r + vOffset, floorGlyph, finalFloorColor);
            }
        }
        
        // Walls
        char wallGlyph = ' ';
        uint32_t wallColor = 0;
        if (hit == 2) { // End Goal
            wallColor = (side == 0) ? RED_GOAL_BRIGHT : RED_GOAL_DARK;
            wallGlyph = (perpWallDist <= 2.5f) ? '#' : '%';
        } else if (hit == 3) { // Crawlspace
            wallColor = getWallColor(perpWallDist + 2.0f, side); 
            wallGlyph = '#';
        } else if (hit == 4) { // Message Wall
            wallColor = (side == 0) ? TIER_MID_BRIGHT : 0xFF1E40AF;
            wallGlyph = (perpWallDist < 1.5f) ? '?' : '+';
        } else if (hit == 5) { // Closed Door
            wallColor = getWallColor(perpWallDist, side);
            wallGlyph = (perpWallDist < 4.0f) ? '|' : ':';
        }
        else { // Normal Wall
            wallColor = getWallColor(perpWallDist, side);
            if (side == 1) { wallGlyph = (perpWallDist <= 2.5f) ? ':' : (perpWallDist <= 5.8f ? ';' : '.'); }
            else {
                if (perpWallDist <= 1.25f)      wallGlyph = '@';
                else if (perpWallDist <= 2.50f) wallGlyph = '#';
                else if (perpWallDist <= 4.00f) wallGlyph = '%';
                else if (perpWallDist <= 5.80f) wallGlyph = '*';
                else if (perpWallDist <= 7.50f) wallGlyph = '+';
                else wallGlyph = '.';
            }
        }
        
        if (player.sanity < 30.0f && (rand() % 100 < 5)) wallGlyph = "!@#$%^&*"[rand()%8];

        int lineHeight = int(ROWS / perpWallDist);
        int drawStart = -lineHeight / 2 + horizon;
        int drawEnd = lineHeight / 2 + horizon;
        for (int r = std::max(0, drawStart); r < std::min(ROWS, drawEnd); ++r) {
            float vis = calculateVisibility(col, r, perpWallDist, viewWidth);
            if (vis <= 0.02f) continue;
            uint32_t finalColor = applyShadow(wallColor, vis, vignette);
            drawGlyphFine(col, r + vOffset, wallGlyph, finalColor);
        }
    }
    
    renderItems(zBuffer);
    if (stalker.active) renderEnemySprite(zBuffer, stalker, spriteStalker0, spriteStalker1, 1.0f);
    if (mistEnemy.active) renderEnemySprite(zBuffer, mistEnemy, spriteMist0, spriteMist0, 0.8f);
    if (ghost.active) { /* Render ghost sprite logic here */ }
    
    if (currentDifficulty == DIFF_EASY) {
        renderSidebarMinimap();
    }

    renderUI();
}

void Game::renderItems(const std::vector<float>& zBuffer) { /* ... */ }
void Game::renderEnemySprite(const std::vector<float>& zBuffer, const Enemy& e, const std::vector<std::string>& f0, const std::vector<std::string>& f1, float heightMultiplier) { /* ... */ }
void Game::renderSidebarMinimap() {
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
            else if (worldMap[r][c].wallType == 4) { mapCh = '?'; mapCol = TIER_MID_BRIGHT; }
            else if (worldMap[r][c].wallType == 5) { mapCh = '-'; mapCol = THEME1_BRIGHT; }
            else if (worldMap[r][c].wallType == 6) { mapCh = ' '; }

            drawGlyphFine(miniStartX + c, miniStartY + r, mapCh, mapCol);
        }
    }

    for (const auto& it : itemsInWorld) {
        if (it.x >= 0 && it.x < MAP_W && it.y >= 0 && it.y < MAP_H && isMapVisible(it.x, it.y)) {
            char ch = ' ';
            uint32_t color = 0;
            if (it.type == ITEM_BREAD) { ch = 'B'; color = 0xFFF59E0B; }
            else if (it.type == ITEM_MEDS) { ch = '+'; color = 0xFF06B6D4; }
            else if (it.type == ITEM_PEBBLE) { ch = 'o'; color = 0xFF94A3B8; }
            else if (it.type == ITEM_OIL) { ch = 'L'; color = 0xFFEAB308; }
            drawGlyphFine(miniStartX + int(it.x), miniStartY + int(it.y), ch, color);
        }
    }

    int pMapX = int(player.posX);
    int pMapY = int(player.posY);
    drawGlyphFine(miniStartX + pMapX, miniStartY + pMapY, 'O', 0xFF38BDF8);

    if (stalker.active && isMapVisible(stalker.x, stalker.y)) {
        drawGlyphFine(miniStartX + int(stalker.x), miniStartY + int(stalker.y), '!', RED_GOAL_BRIGHT);
    }
    if (mistEnemy.active && isMapVisible(mistEnemy.x, mistEnemy.y)) {
        drawGlyphFine(miniStartX + int(mistEnemy.x), miniStartY + int(mistEnemy.y), '~', 0xFF8B5CF6);
    }

    drawTextFine(miniStartX, 32, "EASY MODE", TIER_LOW_BRIGHT);
}

void Game::renderUI() {
    drawRectFilled(1, 1, 52, 15, 0xEE050505);

    std::string themeName = getCurrentThemeName();
    drawTextFine(2, 2, "AREA: " + themeName + " | LVL: " + std::to_string(currentLevel), TIER_HIGH_BRIGHT);

    uint32_t hpCol = (player.health < 30.0f) ? RED_GOAL_BRIGHT : ((player.health < 60.0f) ? 0xFFF59E0B : TIER_HIGH_BRIGHT);
    drawTextFine(2, 4, "HEALTH: " + std::to_string(int(player.health)) + "%", hpCol);

    uint32_t sanCol = (player.sanity < 30.0f) ? RED_GOAL_BRIGHT : ((player.sanity < 60.0f) ? 0xFFF59E0B : TIER_HIGH_BRIGHT);
    drawTextFine(2, 6, "SANITY: " + std::to_string(int(player.sanity)) + "%", sanCol);

    auto getItemName = [](ItemType type) -> std::string {
        if (type == ITEM_BREAD) return "BREAD";
        if (type == ITEM_MEDS) return "MEDS ";
        if (type == ITEM_PEBBLE) return "PEBBL";
        if (type == ITEM_OIL) return "OIL  ";
        return "---- ";
    };
    
    drawTextFine(2, 8, "INV: [1]" + getItemName(player.inventory[0]) + "[2]" + getItemName(player.inventory[1]) + "[3]" + getItemName(player.inventory[2]), 0xFF94A3B8);
    
    std::string fuelBar = "[";
    int barLength = 10;
    int filled = static_cast<int>((player.lanternFuel / LANTERN_FUEL_MAX) * barLength);
    for (int i = 0; i < barLength; ++i) { fuelBar += (i < filled) ? "=" : "-"; }
    fuelBar += "]";
    std::string lanStr = player.lanternBroken ? "BROKEN" : (player.lanternOn ? "ON" : "OFF");
    drawTextFine(2, 10, "[F] Lantern: " + lanStr, (player.lanternOn ? TIER_HIGH_BRIGHT : 0xFF94A3B8));
    drawTextFine(20, 10, "Fuel: " + fuelBar, 0xFFEAB308);

    drawTextFine(2, 12, "[E] Interact | [1-3] Use Item", TIER_MID_BRIGHT);
}

void Game::renderTitleScreen() {
    drawTextStandard(34, 12, "==============================", TIER_HIGH_BRIGHT);
    drawTextStandard(34, 14, "     WALK ASCII 3D HORROR     ", TIER_HIGH_BRIGHT);
    drawTextStandard(34, 16, "==============================", TIER_HIGH_BRIGHT);

    std::string diffStr = (currentDifficulty == DIFF_NORMAL) ? "NORMAL (NO MINIMAP)" : "EASY (WITH MINIMAP)";
    std::string resStr = RESOLUTION_PRESETS[currentResIndex].label;

    auto renderSlider = [&](int y, const std::string& label, float value, bool selected) {
        std::string bar = "[";
        int barLength = 20;
        int filled = static_cast<int>(value * barLength);
        for (int i = 0; i < barLength; ++i) bar += (i < filled) ? "=" : "-";
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
        if (i == 2) renderSlider(24 + i * 4, options[i] + " ", audioSystem.masterVolume, i == menuCursor);
        else if (i == 3) renderSlider(24 + i * 4, options[i] + " ", player.mouseSensitivity, i == menuCursor);
        else drawTextStandard(32, 24 + i * 4, prefix + options[i], col);
    }
    drawTextStandard(26, 50, "UP/DOWN: SELECT | LEFT/RIGHT: CHANGE | ENTER: START", 0xFF334155);
}

void Game::renderPauseScreen() {
    drawRectFilled(30, 18, 40, 24, 0xEE050505);
    drawTextFine(38, 22, "========================", TIER_MID_BRIGHT);
    drawTextFine(38, 24, "      GAME PAUSED       ", TIER_MID_BRIGHT);
    drawTextFine(38, 26, "========================", TIER_MID_BRIGHT);
    drawTextFine(35, 32, "[R / ESC] RESUME GAME", TIER_HIGH_BRIGHT);
    drawTextFine(35, 36, "[Q] QUIT TO TITLE", RED_GOAL_BRIGHT);
}

void Game::renderSuccessScreen() {
    drawTextStandard(36, 12, "****************************", TIER_HIGH_BRIGHT);
    drawTextStandard(36, 14, "      LEVEL COMPLETED!      ", TIER_HIGH_BRIGHT);
    drawTextStandard(36, 16, "****************************", TIER_HIGH_BRIGHT);
    drawTextStandard(34, 22, "COMPLETED LEVEL:  " + std::to_string(currentLevel), 0xFFFFFFFF);
    drawTextStandard(34, 25, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFFFFFFF);
    drawTextStandard(34, 28, "TIME TAKEN:       " + std::to_string(int(levelTime)) + " SECONDS", 0xFFFFFFFF);
    drawTextStandard(28, 44, "PRESS [ENTER] TO PROCEED TO THE NEXT LEVEL", TIER_LOW_BRIGHT);
    drawTextStandard(38, 47, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);
}

void Game::renderGameOverScreen() {
    drawTextStandard(36, 10, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);
    drawTextStandard(36, 12, "         YOU DIED           ", RED_GOAL_BRIGHT);
    drawTextStandard(36, 14, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX", RED_GOAL_BRIGHT);
    drawTextStandard(28, 20, deathReason, RED_GOAL_BRIGHT);
    drawTextStandard(34, 26, "DIED ON LEVEL:    " + std::to_string(currentLevel), 0xFFCBD5E1);
    drawTextStandard(34, 29, "TOTAL STEPS:      " + std::to_string(totalSteps), 0xFFCBD5E1);
    drawTextStandard(34, 32, "SURVIVED FOR:    " + std::to_string(int(levelTime)) + " SECONDS", 0xFFCBD5E1);
    drawTextStandard(32, 42, "PRESS [ENTER] TO TRY AGAIN", TIER_HIGH_BRIGHT);
    drawTextStandard(38, 45, "PRESS [ESC] FOR MAIN MENU", 0xFF64748B);
}

void Game::renderMessageScreen() {
    drawRectFilled(20, 30, TOTAL_COLS - 40, 10, 0xEE050505);
    drawTextFine(22, 32, currentMessage, 0xFFFFFFFF);
    drawTextFine(22, 36, "[PRESS E / ENTER TO CLOSE]", TIER_LOW_BRIGHT);
}

void Game::renderJumpscareScreen() {
    const auto& currentSprite = spriteStalker0;
    int rowCount = currentSprite.size(); int colCount = currentSprite[0].size();
    int centerXOffset = (TOTAL_COLS - colCount) / 2; int centerYOffset = (ROWS - rowCount) / 2;
    for (int y = 0; y < rowCount; ++y) {
        for (int x = 0; x < colCount; ++x) {
            int screenX = centerXOffset + x; int screenY = centerYOffset + y;
            if ((rand() % 100) < 5) screenX += (rand() % 5) - 2;
            if (screenX >= 0 && screenX < TOTAL_COLS && screenY >= 0 && screenY < ROWS) {
                char glyph = currentSprite[y][x];
                if (glyph != ' ' && glyph != '.') {
                    uint32_t flashCol = ((rand() % 2) == 0) ? RED_GOAL_BRIGHT : 0xFFFFFFFF;
                    drawGlyphFine(screenX, screenY, glyph, flashCol);
                }
            }
        }
    }
}

void Game::generateProceduralMultiLevelMaze() {
    srand(static_cast<unsigned int>(time(nullptr)) + currentLevel * 1337);
    itemsInWorld.clear(); activeProjectiles.clear();

    for (int r = 0; r < MAP_H; ++r) for (int c = 0; c < MAP_W; ++c) worldMap[r][c].wallType = 1;

    std::stack<Point> stack; startPos = { 1, 1 }; worldMap[startPos.y][startPos.x].wallType = 0; stack.push(startPos);
    const int dx[4] = {0, 0, 2, -2}, dy[4] = {-2, 2, 0, 0};

    while (!stack.empty()) {
        Point curr = stack.top();
        std::vector<int> dirs;
        for (int i = 0; i < 4; ++i) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx > 0 && nx < MAP_W - 1 && ny > 0 && ny < MAP_H - 1 && worldMap[ny][nx].wallType == 1) dirs.push_back(i);
        }
        if (!dirs.empty()) {
            int d = dirs[rand() % dirs.size()];
            worldMap[curr.y + dy[d] / 2][curr.x + dx[d] / 2].wallType = 0;
            worldMap[curr.y + dy[d]][curr.x + dx[d]].wallType = 0;
            stack.push({ curr.x + dx[d], curr.y + dy[d] });
        } else stack.pop();
    }

    std::vector<Point> potentialVents, emptyFloorSpaces;
    for (int y = 1; y < MAP_H - 1; ++y) for (int x = 1; x < MAP_W - 1; ++x) {
        if (worldMap[y][x].wallType == 1 && ((worldMap[y][x-1].wallType==0 && worldMap[y][x+1].wallType==0) || (worldMap[y-1][x].wallType==0 && worldMap[y+1][x].wallType==0)))
            potentialVents.push_back({x, y});
        if (worldMap[y][x].wallType == 0) emptyFloorSpaces.push_back({x, y});
    }

    int numVents = 1 + (rand() % 3);
    while (numVents-- > 0 && !potentialVents.empty()) {
        int idx = rand() % potentialVents.size(); Point v = potentialVents[idx]; worldMap[v.y][v.x].wallType = 3;
        potentialVents.erase(potentialVents.begin() + idx);
    }

    if (rand() % 100 < 5 && !emptyFloorSpaces.empty()) {
        int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
        const int ndx[] = {0,0,1,-1}, ndy[] = {1,-1,0,0};
        for(int i = 0; i < 4; ++i) {
            int wx = p.x + ndx[i], wy = p.y + ndy[i];
            if(worldMap[wy][wx].wallType == 1) { worldMap[wy][wx].wallType = 4; break; }
        }
    }

    int numBread = 2+(rand()%3), numMeds = 1+(rand()%2), numPebbles = 3+(rand()%3), numOil = 2+(rand()%2);
    auto spawnItem = [&](ItemType type) {
        if (!emptyFloorSpaces.empty()) {
            int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
            itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, type});
            emptyFloorSpaces.erase(emptyFloorSpaces.begin() + idx);
        }
    };
    for(int i=0; i<numBread; ++i) spawnItem(ITEM_BREAD);
    for(int i=0; i<numMeds; ++i) spawnItem(ITEM_MEDS);
    for(int i=0; i<numPebbles; ++i) spawnItem(ITEM_PEBBLE);
    for(int i=0; i<numOil; ++i) spawnItem(ITEM_OIL);

    endPos = { MAP_W - 2, MAP_H - 2 }; worldMap[endPos.y][endPos.x].wallType = 2;
    player.reset();
    player.posX = startPos.x + 0.5f; player.posY = startPos.y + 0.5f;

    stalker.reset(false, AI_STATE_IDLE);
    mistEnemy.reset(false, AI_STATE_IDLE);
    mistEnemy.speed = 0.5f;

    if (currentLevel <= 10) { corruptionLevel = 0.0f; } 
    else if (currentLevel >= 11 && currentLevel <= 15) {
        corruptionLevel = std::min(1.0f, (currentLevel - 10) * 0.1f);
        mistEnemy.reset(true, AI_STATE_IDLE);
    } else if (currentLevel >= 16 && currentLevel <= 20) {
        corruptionLevel = std::min(1.0f, 0.5f + (currentLevel - 15) * 0.1f);
        stalker.reset(true, AI_STATE_WATCHING);
    } else { // 21+
        corruptionLevel = 1.0f;
        stalker.reset(true, AI_STATE_HUNTING);
        mistEnemy.reset((rand() % 100 < 40), AI_STATE_IDLE);
    }
}

void Game::updateWindowScale() {
    if (window) {
        int targetW = RESOLUTION_PRESETS[currentResIndex].width;
        int targetH = RESOLUTION_PRESETS[currentResIndex].height;
        SDL_SetWindowSize(window, targetW, targetH);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

void Game::setCaptureMouse(bool capture) {
    SDL_SetRelativeMouseMode(capture ? SDL_TRUE : SDL_FALSE);
    SDL_SetWindowGrab(window, capture ? SDL_TRUE : SDL_FALSE);
}

uint32_t Game::applyShadow(uint32_t hexColor, float brightness, float vignette) {
    brightness *= vignette;
    brightness = std::clamp(brightness, 0.0f, 1.0f);
    uint32_t a = (hexColor >> 24) & 0xFF;
    uint32_t r = static_cast<uint32_t>(((hexColor >> 16) & 0xFF) * brightness);
    uint32_t g = static_cast<uint32_t>(((hexColor >> 8) & 0xFF) * brightness);
    uint32_t b = static_cast<uint32_t>((hexColor & 0xFF) * brightness);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

float Game::calculateVisibility(int col, int row, float dist, int viewWidth) {
    if (currentLevel <= 10) return 1.0f;
    if (!player.lanternOn) {
        if (dist > 1.2f) return 0.0f;
        return std::clamp(1.0f - (dist / 1.2f), 0.0f, 1.0f);
    }
    float lightCenterY = (ROWS / 2.0f) - (player.pitch * 0.5f); 
    float cameraX = 2.0f * col / float(viewWidth) - 1.0f;
    float cameraY = 2.0f * (row - lightCenterY) / float(ROWS);
    float aspect = (float)viewWidth / (float)ROWS;
    float spotRadius = std::hypot(cameraX * aspect, cameraY);
    if (spotRadius > 0.9f || dist > 14.0f) return 0.0f;
    float edgeFade = std::clamp(1.0f - ((spotRadius - 0.6f) * 3.3f), 0.0f, 1.0f);
    float distFade = 1.0f / (1.0f + 0.15f * dist + 0.3f * dist * dist);
    distFade = std::clamp(distFade * 1.5f, 0.0f, 1.0f); 
    float flicker = 1.0f - ((rand() % 100) / 100.0f) * 0.05f;
    return edgeFade * distFade * flicker;
}

uint32_t Game::getWallColor(float dist, int side) {
    uint32_t cBright, cMid, cDark;
    if (currentLevel <= 10) { cBright = THEME_INTRO_BRIGHT; cMid = THEME_INTRO_MID; cDark = THEME_INTRO_DARK; }
    else {
        int theme = (currentLevel - 11) % 4;
        if (theme == 0)      { cBright = THEME0_BRIGHT; cMid = THEME0_MID; cDark = THEME0_DARK; }
        else if (theme == 1) { cBright = THEME1_BRIGHT; cMid = THEME1_MID; cDark = THEME1_DARK; }
        else if (theme == 2) { cBright = THEME2_BRIGHT; cMid = THEME2_MID; cDark = THEME2_DARK; }
        else                 { cBright = THEME3_BRIGHT; cMid = THEME3_MID; cDark = THEME3_DARK; }
    }
    if (side == 0) return (dist < 3.0f) ? cBright : (dist < 6.5f ? cMid : cDark);
    else return (dist < 3.0f) ? cMid : cDark;
}

bool Game::isMapVisible(float mapX, float mapY) {
    if (currentLevel <= 10) return true;
    float dx = mapX - player.posX, dy = mapY - player.posY;
    float dist = std::hypot(dx, dy);
    if (dist <= 3.5f) return true;
    if (player.lanternOn && dist <= 14.0f) {
        float angle = std::atan2(dy, dx), pAngle = std::atan2(player.dirY, player.dirX);
        float diff = std::abs(std::atan2(std::sin(angle - pAngle), std::cos(angle - pAngle)));
        if (diff > M_PI) diff = 2.0f * M_PI - diff; 
        if (diff <= 0.5f) return true; 
    }
    return false;
}

bool Game::hasLineOfSight(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1, dy = y2 - y1;
    float dist = std::hypot(dx, dy);
    dx /= dist; dy /= dist;
    float currentX = x1, currentY = y1;
    for (float i = 0; i < dist; i += 0.2f) {
        currentX += dx * 0.2f; currentY += dy * 0.2f;
        if (worldMap[(int)currentY][(int)currentX].wallType == 1) return false;
    }
    return true;
}

void Game::initializeSprites() {
    const std::vector<std::string> rawStalker0 = {
        "                                                                   ", "                                                         .:-::....         .::.",
        "                              ..::----===========+++====+++++++:", "                            :-==++*##***+====. :=+*##%%%%######*#*+",
        "                       .::-==+#@@@@@@@@@#+-     :%@@@@@@@@@@%%#####* .", "                       -==+*%@@@@@@@@@@@@%-      =@@@@@@@@@@@@@@@%%**=.",
        "                       -+*#@@@@@@@@@@@@@@#       -@@@@@@@@@@@@@@%#*++=:.", "                        :=#@@@@@@@@@@@@#.       :#@@@@@@@@@@@@%*+-:.::-.",
        "                           =#@@@@@@@@%+.        .+-:+%@@@@@*-.     ..:-", "                              :======:           ==.  .              ..:",
        "                                                 :+:                 .:-", "                                                 .:.               ..--* .",
        "                                        ::.   :=*#+#+              .:-+*", "                                       :=*#=:--+%@@%+             .:-+**=",
        "                                             :-#@%%#*=:           .:=+###", "                                                 :+*+:      ......:--+*%%%-",
        "                                        .-+-.   :**:   .:...::-::::=+#%%%#", "                                      -+%%==*#%%%@@%#*=---::--=----=*#%%%@",
        "                                   .=+@- :=+ :*:-###@@%***=:=======+*%@@%@", "                                  -*#-          :-=%@@@###*-++++=++*#%@@%*",
        "                         .       :%%  .:*@%%@%==%-=+ @@@*#%*++++++*#%@@@%", "                        .        #@@@@@@@@@@@@@@@@@@@@@#=%%%+++++*%@@@@%.",
        "                         ..     :@@@@@@@@@@@@@@@@@@@@@@%-#%%#+**#%@@@@@#", "                         ..     +@@@@@@@@@@@@@@@@@@@@@@@-#%%%#*#%@@@@@@",
        "                   .      .     *@@@@@@@@@@@@@@@@@@@@@@@:%@@%%#%%@@@@@", "                     -    .     +@@@@@@%@@@@@@@@@@@@@@@@:@%@%%%%@%@%#",
        "                          .     .@@@@%#@@@@@@@@@@@@@@@@+-@%%%%%@@%=", "                                 @@@@@@@@@@@@@@@@@@@@@@ +@%%#=#@@",
        "                                  @@@@@+*@@@@@@@@@@@@%: #%%%+=%@", "                                  . =+*@@@@@@@@@@@% =  .%%#%=+%",
        "                                         -*%*+.   +=*  *@%%%--", "                                      : -+==+++++*%   *@%%@*",
        "                                 .=:      .=*+-:    -%@%%", "                                   :**=:        -+%@@+",
        "                                        .-+**##="
    };
    auto padSprite = [](std::vector<std::string>& sprite, const std::vector<std::string>& raw) {
        size_t maxL = 0;
        for (const auto& s : raw) if (s.length() > maxL) maxL = s.length();
        sprite.clear();
        for (const auto& s : raw) {
            sprite.push_back(s);
            sprite.back().append(maxL - s.length(), ' ');
        }
    };
    padSprite(spriteStalker0, rawStalker0);
    padSprite(spriteStalker1, rawStalker0);
    padSprite(spriteMist0, {
        "               __,aaPPPPPPPPaa,__               ", "           ,adP\"\"\"'          `\"\"Yb,_            ", "        ,adP'                     `\"Yb,         ",
        "      ,dP'     ,aadPP\"\"\"\"\"YYba,_     `\"Y,       ", "     ,P'    ,aP\"'            `\"\"Ya,     \"Y,     ", "    ,P'    aP'     _________     `\"Ya    `Yb,   ",
        "   ,P'    d\"    ,adP\"\"\"\"\"\"\"\"Yba,    `Y,    \"Y,  ", "  ,d'   ,d'   ,dP\"            `Yb,   `Y,    `Y, ", "  d'   ,d'   ,d'    ,dP\"\"Yb,    `Y,   `Y,    `b ",
        "  8    d'    d'   ,d\"      \"b,   `Y,   `8,    Y,", "  8    8     8    d'    _   `Y,   `8    `8    `b", "  8    8     8    8     8    `8    8     8     8",
        "  8    Y,    Y,   `b, ,aP     P    8    ,P     8", "  I,   `Y,   `Ya    \"\"\"\"     d'   ,P    d\"    ,P", "  `Y,   `8,    `Ya         ,8\"   ,P'   ,P'    d'",
        "   `Y,   `Ya,    `Ya,,__,,d\"'   ,P'   ,P\"    ,P ", "    `Y,    `Ya,     `\"\"\"\"'     ,P'   ,d\"    ,P' ", "     `Yb,    `\"Ya,_          ,d\"    ,P'    ,P'  ",
        "       `Yb,      \"\"YbaaaaaadP\"     ,P'    ,P'   ", "         `Yba,                   ,d'    ,dP'    ", "            `\"Yba,__       __,adP\"     dP\"      ",
        "                `\"\"\"\"\"\"\"\"\"\"\"\"\"'                 "
    });
}

void Game::startNewGame() {
    currentLevel = 1;
    totalSteps = 0;
    levelTime = 0.0f;
    
    player.reset();

    generateProceduralMultiLevelMaze();
    currentState = STATE_PLAYING;
    setCaptureMouse(true);

    audioSystem.inGame = true;
    audioSystem.isJumpscare = false;
    audioSystem.sanity = player.sanity;
    audioSystem.closestEnemyDist = 999.0f;
    audioSystem.corruption = 0.0f;
}

void Game::nextLevel() {
    currentLevel++;
    player.sanity = std::min(100.0f, player.sanity + 30.0f);
    player.health = std::min(100.0f, player.health + 30.0f);
    generateProceduralMultiLevelMaze();
    currentState = STATE_PLAYING;
    setCaptureMouse(true);

    audioSystem.inGame = true;
    audioSystem.isJumpscare = false;
    audioSystem.corruption = corruptionLevel;
}

// --- DRAWING HELPER FUNCTIONS ---

void Game::drawGlyphStandard(int col, int row, char c, uint32_t fgColor) {
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

void Game::drawTextStandard(int col, int row, const std::string& text, uint32_t color) {
    for (size_t i = 0; i < text.size(); ++i) {
        if (col + i < TOTAL_COLS) {
            drawGlyphStandard(col + i, row, text[i], color);
        }
    }
}

void Game::drawGlyphFine(int col, int row, char c, uint32_t fgColor) {
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

void Game::drawTextFine(int col, int row, const std::string& text, uint32_t color) {
    for (size_t i = 0; i < text.size(); ++i) {
        if (col + i < TOTAL_COLS) {
            drawGlyphFine(col + i, row, text[i], color);
        }
    }
}

void Game::drawRectFilled(int startCol, int startRow, int numCols, int numRows, uint32_t color) {
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

std::string Game::getCurrentThemeName() {
    if (currentLevel <= 10) {
        return "INTRO"; 
    }
    
    int theme = (currentLevel - 11) % 4;
    if (theme == 0) return "THE DUNGEONS";
    if (theme == 1) return "THE CATACOMBS";
    if (theme == 2) return "THE ABYSS";
    return "THE VOID";
}

