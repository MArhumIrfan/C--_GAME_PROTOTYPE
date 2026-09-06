// Per-frame simulation: input handling, physics/collision, enemy AI movement and pathfinding.

#include "Game.h"
#include "Font.h"
#include "Sprites.h"

#include <cmath>
#include <algorithm>
#include <cstdlib>

std::vector<Point> WalkAsciiElevationEngine::findPath(Point start, Point end) {
        std::vector<Point> path;
        
        auto cmp = [](const AStarNode* a, const AStarNode* b) { return a->f() > b->f(); };
        std::priority_queue<AStarNode*, std::vector<AStarNode*>, decltype(cmp)> openSet(cmp);
        std::vector<AStarNode*> allNodes;

        AStarNode* startNode = new AStarNode{start.x, start.y, 0, std::abs(start.x - end.x) + std::abs(start.y - end.y), nullptr};
        openSet.push(startNode);
        allNodes.push_back(startNode);
        
        bool inOpenSet[MAP_H][MAP_W] = {false};
        inOpenSet[start.y][start.x] = true;

        bool closedSet[MAP_H][MAP_W] = {false};

        while (!openSet.empty()) {
            AStarNode* current = openSet.top();
            openSet.pop();

            if (current->x == end.x && current->y == end.y) {
                while (current != nullptr) {
                    path.push_back({current->x, current->y});
                    current = current->parent;
                }
                std::reverse(path.begin(), path.end());
                break;
            }

            closedSet[current->y][current->x] = true;

            const int dx[] = {0, 0, 1, -1};
            const int dy[] = {1, -1, 0, 0};

            for (int i = 0; i < 4; ++i) {
                int nx = current->x + dx[i];
                int ny = current->y + dy[i];

                if (nx < 0 || nx >= MAP_W || ny < 0 || ny >= MAP_H || closedSet[ny][nx]) {
                    continue;
                }
                
                if (worldMap[ny][nx].wallType != 0 && worldMap[ny][nx].wallType != 5 && worldMap[ny][nx].wallType != 4) {
                    continue;
                }
                
                if (!inOpenSet[ny][nx]) {
                    AStarNode* neighbor = new AStarNode{nx, ny, current->g + 1, std::abs(nx - end.x) + std::abs(ny - end.y), current};
                    openSet.push(neighbor);
                    allNodes.push_back(neighbor);
                    inOpenSet[ny][nx] = true;
                }
            }
        }
        
        for (auto node : allNodes) delete node;
        return path;
    }

bool WalkAsciiElevationEngine::hasLineOfSight(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float dist = std::hypot(dx, dy);
        dx /= dist;
        dy /= dist;

        float currentX = x1;
        float currentY = y1;

        for (float i = 0; i < dist; i += 0.2f) {
            currentX += dx * 0.2f;
            currentY += dy * 0.2f;
            int cx = std::clamp(int(currentX), 0, MAP_W - 1);
            int cy = std::clamp(int(currentY), 0, MAP_H - 1);
            
            if (worldMap[cy][cx].wallType == 1 || worldMap[cy][cx].wallType == 4 || worldMap[cy][cx].wallType == 6) {
                return false;
            }
        }
        return true;
    }

void WalkAsciiElevationEngine::moveEnemyToward(Enemy& e, float targetX, float targetY, float dtSec) {
        float dist = std::hypot(targetX - e.x, targetY - e.y);
        if (dist < 0.1f) return;

        float dx = (targetX - e.x) / dist;
        float dy = (targetY - e.y) / dist;

        float moveX = dx * e.speed * dtSec;
        float moveY = dy * e.speed * dtSec;

        int curTileX = std::clamp(int(e.x), 0, MAP_W - 1);
        int curTileY = std::clamp(int(e.y), 0, MAP_H - 1);
        int nextTileX = std::clamp(int(e.x + moveX), 0, MAP_W - 1);
        int nextTileY = std::clamp(int(e.y + moveY), 0, MAP_H - 1);

        if (worldMap[curTileY][nextTileX].wallType == 0 || worldMap[curTileY][nextTileX].wallType == 5) e.x += moveX;
        if (worldMap[nextTileY][curTileX].wallType == 0 || worldMap[nextTileY][curTileX].wallType == 5) e.y += moveY;
    }

bool WalkAsciiElevationEngine::isMapVisible(float mapX, float mapY) {
        if (currentLevel <= 10) return true;
        if (player.inLocker) return false;
        
        float dx = mapX - player.posX;
        float dy = mapY - player.posY;
        float dist = std::hypot(dx, dy);
        
        float visibilityRadius = 3.5f * std::max(0.35f, player.sanity / 100.0f);
        if (dist <= visibilityRadius) return true;
        
        if (player.lanternOn && dist <= 14.0f) {
            float angle = std::atan2(dy, dx);
            float pAngle = std::atan2(player.dirY, player.dirX);
            float diff = std::abs(std::atan2(std::sin(angle - pAngle), std::cos(angle - pAngle)));
            if (diff > 3.14159f) diff = 2.0f * 3.14159f - diff; 
            if (diff <= 0.5f) return true; 
        }
        return false;
    }

void WalkAsciiElevationEngine::handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) isRunning = false;

            if (currentState == STATE_PLAYING && event.type == SDL_MOUSEMOTION) {
                if (!player.inLocker) {
                    float rotAngle = event.motion.xrel * (0.0005f + player.mouseSensitivity * 0.004f);

                    float oldDirX = player.dirX;
                    player.dirX = player.dirX * cos(rotAngle) - player.dirY * sin(rotAngle);
                    player.dirY = oldDirX * sin(rotAngle) + player.dirY * cos(rotAngle);

                    float oldPlaneX = player.planeX;
                    player.planeX = player.planeX * cos(rotAngle) - player.planeY * sin(rotAngle);
                    player.planeY = oldPlaneX * sin(rotAngle) + player.planeY * cos(rotAngle);

                    player.pitch -= event.motion.yrel * 0.12f;
                    player.pitch = std::clamp(player.pitch, -22.0f, 22.0f);
                }
            }
            
            if (currentState == STATE_TITLE) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    int windowW, windowH;
                    SDL_GetWindowSize(window, &windowW, &windowH);

                    float yRatio = (float)y / windowH;
                    if (yRatio > 0.45f && yRatio < 0.9f) {
                        int selected = (int)((yRatio - 0.45f) / 0.1f);
                        if (selected >= 0 && selected < 6) {
                            menuCursor = selected;
                            if (event.button.button == SDL_BUTTON_LEFT) {
                                SDL_Event keyEvent;
                                keyEvent.type = SDL_KEYDOWN;
                                keyEvent.key.keysym.sym = SDLK_RETURN;
                                SDL_PushEvent(&keyEvent);
                            }
                        }
                    }
                }
            }

            if (event.type == SDL_KEYDOWN) {
                if (currentState == STATE_TITLE) {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) menuCursor = (menuCursor - 1 + 6) % 6;
                    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) menuCursor = (menuCursor + 1) % 6;
                    
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                        if (menuCursor == 0) startNewGame();
                        else if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                        else if (menuCursor == 4) {
                            currentResIndex = (currentResIndex + 1) % RESOLUTION_PRESETS.size();
                            updateWindowScale();
                        }
                        else if (menuCursor == 5) isRunning = false;
                    }
                    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
                        if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                        if (menuCursor == 2) audioState.masterVolume = std::max(0.0f, audioState.masterVolume - 0.05f);
                        if (menuCursor == 3) player.mouseSensitivity = std::max(0.0f, player.mouseSensitivity - 0.05f);
                        if (menuCursor == 4) {
                             currentResIndex = (currentResIndex - 1 + RESOLUTION_PRESETS.size()) % RESOLUTION_PRESETS.size();
                             updateWindowScale();
                        }
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
                        if (menuCursor == 1) currentDifficulty = (currentDifficulty == DIFF_NORMAL) ? DIFF_EASY : DIFF_NORMAL;
                        if (menuCursor == 2) audioState.masterVolume = std::min(1.0f, audioState.masterVolume + 0.05f);
                        if (menuCursor == 3) player.mouseSensitivity = std::min(1.0f, player.mouseSensitivity + 0.05f);
                        if (menuCursor == 4) {
                            currentResIndex = (currentResIndex + 1) % RESOLUTION_PRESETS.size();
                            updateWindowScale();
                        }
                    }
                }
                else if (currentState == STATE_PLAYING) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = STATE_PAUSED;
                        setCaptureMouse(false);
                        SDL_LockAudioDevice(audioDevice);
                        audioState.gameState = currentState;
                        audioState.inGame = false;
                        SDL_UnlockAudioDevice(audioDevice);
                    }
                    else if (event.key.keysym.sym == SDLK_q) {
                        currentState = STATE_TITLE;
                        setCaptureMouse(false);
                        SDL_LockAudioDevice(audioDevice);
                        audioState.gameState = currentState;
                        audioState.inGame = false;
                        SDL_UnlockAudioDevice(audioDevice);
                    }
                    else if (event.key.keysym.sym == SDLK_f) {
                        if (!player.inLocker) {
                            SDL_LockAudioDevice(audioDevice);
                            audioState.itemSoundType = 4;
                            audioState.itemSoundPhase = 0.0f;
                            audioState.itemSoundTimer = 0.2f;
                            SDL_UnlockAudioDevice(audioDevice);

                            if (player.lanternBroken) {
                                player.reigniteClicks++;
                                if (player.reigniteClicks >= 3) {
                                    player.lanternBroken = false;
                                    player.lanternOn = true;
                                    player.reigniteClicks = 0;
                                }
                            } else {
                                player.lanternOn = !player.lanternOn;
                            }
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_e) {
                        if (player.inLocker) {
                            player.inLocker = false;
                            SDL_LockAudioDevice(audioDevice);
                            audioState.itemSoundType = 8; 
                            audioState.itemSoundPhase = 0.0f; 
                            audioState.itemSoundTimer = 0.5f;
                            SDL_UnlockAudioDevice(audioDevice);
                            return; 
                        }

                        bool itemPicked = false;
                        for (auto it = itemsInWorld.begin(); it != itemsInWorld.end(); ++it) {
                            if (std::hypot(player.posX - it->x, player.posY - it->y) < 1.5f) {
                                if (it->type == ITEM_KEY) {
                                    player.hasKey = true;
                                    SDL_LockAudioDevice(audioDevice);
                                    audioState.itemSoundType = 9; 
                                    audioState.itemSoundPhase = 0.0f;
                                    audioState.itemSoundTimer = 0.5f;
                                    SDL_UnlockAudioDevice(audioDevice);
                                    itemsInWorld.erase(it);
                                    itemPicked = true;
                                    break;
                                } else {
                                    for (int i = 0; i < 3; ++i) {
                                        if (player.inventory[i] == ITEM_NONE) {
                                            player.inventory[i] = it->type;
                                            SDL_LockAudioDevice(audioDevice);
                                            audioState.itemSoundType = (it->type == ITEM_PEBBLE) ? 1 : ((it->type == ITEM_MEDS) ? 2 : 3);
                                            audioState.itemSoundPhase = 0.0f;
                                            audioState.itemSoundTimer = 0.5f;
                                            SDL_UnlockAudioDevice(audioDevice);
                                            itemsInWorld.erase(it);
                                            itemPicked = true;
                                            break;
                                        }
                                    }
                                }
                                if(itemPicked) break;
                            }
                        }
                        
                        if (!itemPicked) {
                            int checkX = int(player.posX + player.dirX * 1.0f);
                            int checkY = int(player.posY + player.dirY * 1.0f);
                            int targetWall = -1;

                            if (checkX >= 0 && checkX < MAP_W && checkY >= 0 && checkY < MAP_H) {
                                targetWall = worldMap[checkY][checkX].wallType;
                            }
                            
                            if (targetWall != 2 && targetWall != 4 && targetWall != 5 && targetWall != 6) {
                                checkX = int(player.posX + player.dirX * 1.5f);
                                checkY = int(player.posY + player.dirY * 1.5f);
                                if (checkX >= 0 && checkX < MAP_W && checkY >= 0 && checkY < MAP_H) {
                                    targetWall = worldMap[checkY][checkX].wallType;
                                }
                            }
                                
                            if (targetWall == 2) {
                                if (player.hasKey) {
                                    currentState = STATE_SUCCESS;
                                    setCaptureMouse(false);
                                } else {
                                    player.showLockedMessage = 2.0f;
                                }
                            } else if (targetWall == 4) {
                                worldMap[checkY][checkX].wallType = 5; 
                                SDL_LockAudioDevice(audioDevice);
                                audioState.itemSoundType = 5; 
                                audioState.itemSoundPhase = 0.0f; 
                                audioState.itemSoundTimer = 0.5f;
                                SDL_UnlockAudioDevice(audioDevice);
                            } else if (targetWall == 5) {
                                if (int(player.posX) != checkX || int(player.posY) != checkY) {
                                    worldMap[checkY][checkX].wallType = 4; 
                                    SDL_LockAudioDevice(audioDevice);
                                    audioState.itemSoundType = 6; 
                                    audioState.itemSoundPhase = 0.0f; 
                                    audioState.itemSoundTimer = 0.5f;
                                    SDL_UnlockAudioDevice(audioDevice);
                                }
                            } else if (targetWall == 6) {
                                player.inLocker = true;
                                player.lanternOn = false; 
                                SDL_LockAudioDevice(audioDevice);
                                audioState.itemSoundType = 8; 
                                audioState.itemSoundPhase = 0.0f; 
                                audioState.itemSoundTimer = 0.5f;
                                SDL_UnlockAudioDevice(audioDevice);
                                
                                if (stalker.active && stalker.mode > 0 && stalker.isChasing) {
                                    float d = std::hypot(player.posX - stalker.x, player.posY - stalker.y);
                                    if (d < 6.0f && hasLineOfSight(player.posX, player.posY, stalker.x, stalker.y)) {
                                        stalker.mode = 2; 
                                        stalker.enragedTimer = 10.0f;
                                    } else {
                                        stalker.isChasing = false;
                                        stalker.investigateX = checkX + 0.5f;
                                        stalker.investigateY = checkY + 0.5f;
                                        stalker.investigateTimer = 5.0f;
                                    }
                                }
                            }
                        }
                    }
                    else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_3) {
                        if (!player.inLocker) {
                            int slot = event.key.keysym.sym - SDLK_1;
                            bool throwing = (SDL_GetModState() & KMOD_SHIFT);
                            
                            if (player.inventory[slot] != ITEM_NONE) {
                                if (throwing) {
                                    Projectile proj;
                                    proj.x = player.posX;
                                    proj.y = player.posY;
                                    proj.z = player.eyeHeight;
                                    proj.vx = player.dirX * 6.0f;
                                    proj.vy = player.dirY * 6.0f;
                                    proj.vz = 2.5f + (player.pitch / 22.0f) * 2.0f;
                                    proj.type = player.inventory[slot];
                                    proj.active = true;
                                    activeProjectiles.push_back(proj);
                                    
                                    SDL_LockAudioDevice(audioDevice);
                                    audioState.itemSoundType = (proj.type == ITEM_PEBBLE) ? 1 : ((proj.type == ITEM_MEDS) ? 2 : 3);
                                    audioState.itemSoundPhase = 0.0f;
                                    audioState.itemSoundTimer = 0.5f;
                                    SDL_UnlockAudioDevice(audioDevice);

                                    player.inventory[slot] = ITEM_NONE;
                                } else {
                                    ItemType type = player.inventory[slot];
                                    player.inventory[slot] = ITEM_NONE;
                                    
                                    if (type == ITEM_PEBBLE) {
                                        Projectile proj;
                                        proj.x = player.posX; proj.y = player.posY; proj.z = player.eyeHeight;
                                        proj.vx = player.dirX * 6.0f; proj.vy = player.dirY * 6.0f; proj.vz = 2.5f + (player.pitch / 22.0f) * 2.0f;
                                        proj.type = type; proj.active = true;
                                        activeProjectiles.push_back(proj);
                                        
                                        SDL_LockAudioDevice(audioDevice);
                                        audioState.itemSoundType = 1; audioState.itemSoundPhase = 0.0f; audioState.itemSoundTimer = 0.5f;
                                        SDL_UnlockAudioDevice(audioDevice);
                                    } else if (type == ITEM_BREAD) {
                                        player.health = std::min(100.0f, player.health + 40.0f);
                                        if (stalker.active && stalker.mode == 2) stalker.enragedTimer = 10.0f; 
                                    } else if (type == ITEM_MEDS) {
                                        player.sanity = std::min(100.0f, player.sanity + 50.0f);
                                        player.health -= 15.0f;       
                                        player.takingDamage = true;
                                        if (rand() % 100 < 20) player.toxicTimer = 0.8f;     
                                    }
                                }
                            }
                        }
                    }
                    else if (event.key.keysym.sym >= SDLK_F2 && event.key.keysym.sym <= SDLK_F10) {
                        currentLevel = event.key.keysym.sym - SDLK_F1 + 1;
                        generateProceduralMultiLevelMaze();
                    }
                    else if (event.key.keysym.sym == SDLK_PAGEUP) {
                        currentLevel++;
                        generateProceduralMultiLevelMaze();
                    }
                    else if (event.key.keysym.sym == SDLK_PAGEDOWN) {
                        if (currentLevel > 1) {
                            currentLevel--;
                            generateProceduralMultiLevelMaze();
                        }
                    }
                }
                else if (currentState == STATE_PAUSED) {
                    if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_r) {
                        currentState = STATE_PLAYING;
                        setCaptureMouse(true);
                        SDL_LockAudioDevice(audioDevice);
                        audioState.gameState = currentState;
                        audioState.inGame = true;
                        SDL_UnlockAudioDevice(audioDevice);
                    }
                    else if (event.key.keysym.sym == SDLK_q) currentState = STATE_TITLE;
                }
                else if (currentState == STATE_SUCCESS || currentState == STATE_GAMEOVER) {
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                        if (currentState == STATE_SUCCESS) nextLevel(); else startNewGame();
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = STATE_TITLE;
                        setCaptureMouse(false);
                        SDL_LockAudioDevice(audioDevice);
                        audioState.gameState = currentState;
                        SDL_UnlockAudioDevice(audioDevice);
                    }
                }
            }
        }

        if (currentState == STATE_PLAYING && !player.inLocker) {
            const uint8_t* state = SDL_GetKeyboardState(NULL);
            player.forward = 0;
            player.strafe = 0;

            if (state[SDL_SCANCODE_W]) player.forward += 1;
            if (state[SDL_SCANCODE_S]) player.forward -= 1;
            if (state[SDL_SCANCODE_A]) player.strafe -= 1; 
            if (state[SDL_SCANCODE_D]) player.strafe += 1; 

            if ((state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) && player.stamina > 0.0f && (player.forward != 0 || player.strafe != 0)) {
                player.isSprinting = true;
            } else {
                player.isSprinting = false;
            }

            player.isCrouching = state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL];

            if (player.isCrouching) {
                player.moveSpeed = player.baseMoveSpeed * 0.45f;
                player.targetEyeHeight = 0.45f;
            } else if (player.isSprinting) {
                player.moveSpeed = player.baseMoveSpeed * 1.75f;
                player.targetEyeHeight = 0.8f;
            } else {
                player.moveSpeed = player.baseMoveSpeed;
                player.targetEyeHeight = 0.8f;
            }
        } else if (currentState == STATE_PLAYING && player.inLocker) {
            player.forward = 0;
            player.strafe = 0;
            player.isSprinting = false;
            player.isCrouching = false;
        }
    }

void WalkAsciiElevationEngine::update(double dt) {
        if (currentState == STATE_JUMPSCARE) {
            jumpscareTimer -= static_cast<float>(dt);
            if (jumpscareTimer <= 0.0f) {
                currentState = STATE_GAMEOVER;
                SDL_LockAudioDevice(audioDevice);
                audioState.gameState = currentState;
                audioState.isJumpscare = false;
                SDL_UnlockAudioDevice(audioDevice);
            }
            return;
        }

        if (currentState != STATE_PLAYING) return;

        float dtSec = static_cast<float>(dt);
        levelTime += dtSec;
        player.takingDamage = false;

        if (player.showLockedMessage > 0.0f) {
            player.showLockedMessage -= dtSec;
        }

        if (player.damageShake > 0.0f) {
            player.damageShake = std::max(0.0f, player.damageShake - dtSec * 2.0f);
        }

        if (player.toxicTimer > 0.0f) player.toxicTimer -= dtSec;

        player.eyeHeight += (player.targetEyeHeight - player.eyeHeight) * 0.2f;

        if (!player.inLocker) {
            if (player.isSprinting) {
                player.stamina -= 25.0f * dtSec;
            } else {
                player.stamina = std::min(100.0f, player.stamina + 15.0f * dtSec);
            }

            if (player.forward != 0 || player.strafe != 0) {
                float forwardStep = player.forward * player.moveSpeed * dtSec;
                float strafeStep  = player.strafe  * (player.moveSpeed * 0.85f) * dtSec;

                float moveX = player.dirX * forwardStep - player.dirY * strafeStep;
                float moveY = player.dirY * forwardStep + player.dirX * strafeStep;

                float bufX = (moveX > 0) ? 0.32f : -0.32f;
                float bufY = (moveY > 0) ? 0.32f : -0.32f;

                float prevX = player.posX;
                float prevY = player.posY;

                int nextTileX = std::clamp(int(player.posX + moveX + bufX), 0, MAP_W - 1);
                int nextTileY = std::clamp(int(player.posY), 0, MAP_H - 1);

                int typeX = worldMap[nextTileY][nextTileX].wallType;
                bool canMoveX = (typeX == 0 || typeX == 5 || (typeX == 3 && player.isCrouching));

                nextTileX = std::clamp(int(player.posX), 0, MAP_W - 1);
                nextTileY = std::clamp(int(player.posY + moveY + bufY), 0, MAP_H - 1);
                
                int typeY = worldMap[nextTileY][nextTileX].wallType;
                bool canMoveY = (typeY == 0 || typeY == 5 || (typeY == 3 && player.isCrouching));
                
                if (canMoveX) player.posX += moveX;
                if (canMoveY) player.posY += moveY;

                player.stepAccumulator += std::hypot(player.posX - prevX, player.posY - prevY);
                if (player.stepAccumulator >= 1.0f) {
                    totalSteps++;
                    player.stepAccumulator = 0.0f;
                }
            }
        }
        
        if (player.isSprinting && player.lanternOn && !player.inLocker) {
            player.lanternOn = false;
            player.lanternBroken = true;
            player.reigniteClicks = 0;
            SDL_LockAudioDevice(audioDevice);
            audioState.itemSoundType = 4;
            audioState.itemSoundPhase = 0.0f;
            audioState.itemSoundTimer = 0.2f;
            SDL_UnlockAudioDevice(audioDevice);
        }

        bool inCrawlspace = worldMap[int(player.posY)][int(player.posX)].wallType == 3;
        if (inCrawlspace) {
            player.isCrouching = true;
            player.moveSpeed = player.baseMoveSpeed * 0.45f;
            player.targetEyeHeight = 0.45f;
            player.health = std::min(100.0f, player.health + 2.0f * dtSec);
        }

        for (auto& proj : activeProjectiles) {
            if (!proj.active) continue;

            int curX = std::clamp(int(proj.x), 0, MAP_W - 1);
            int curY = std::clamp(int(proj.y), 0, MAP_H - 1);
            int nextX = std::clamp(int(proj.x + proj.vx * dtSec), 0, MAP_W - 1);
            int nextY = std::clamp(int(proj.y + proj.vy * dtSec), 0, MAP_H - 1);
            
            if (worldMap[curY][nextX].wallType == 1 || worldMap[curY][nextX].wallType == 4 || worldMap[curY][nextX].wallType == 6) proj.vx *= -0.5f; 
            if (worldMap[nextY][curX].wallType == 1 || worldMap[nextY][curX].wallType == 4 || worldMap[nextY][curX].wallType == 6) proj.vy *= -0.5f; 

            proj.x += proj.vx * dtSec;
            proj.y += proj.vy * dtSec;
            proj.vz -= 15.0f * dtSec; 
            proj.z += proj.vz * dtSec;

            if (stalker.active && stalker.mode > 0) {
                float distToProj = std::hypot(proj.x - stalker.x, proj.y - stalker.y);
                if (distToProj < 0.8f && proj.z < 2.0f) {
                    if (stalker.mode == 1) stalker.mode = 2; 
                    stalker.enragedTimer = 5.0f; 
                    proj.active = false;
                }
            }
            
            if (proj.z <= 0.0f && proj.active) {
                proj.active = false;
                
                SDL_LockAudioDevice(audioDevice);
                audioState.itemSoundType = (proj.type == ITEM_PEBBLE) ? 1 : ((proj.type == ITEM_MEDS) ? 2 : 3);
                audioState.itemSoundPhase = 0.0f;
                audioState.itemSoundTimer = 0.5f;
                SDL_UnlockAudioDevice(audioDevice);

                if (proj.type == ITEM_PEBBLE) {
                    itemsInWorld.push_back({proj.x, proj.y, ITEM_PEBBLE}); 
                    if (stalker.active && stalker.mode > 0 && stalker.enragedTimer <= 0.0f) {
                        stalker.investigateX = proj.x;
                        stalker.investigateY = proj.y;
                        stalker.investigateTimer = 5.0f; 
                    }
                } else if (proj.type == ITEM_BREAD) {
                    itemsInWorld.push_back({proj.x, proj.y, ITEM_BREAD}); 
                } 
            }
        }
        
        activeProjectiles.erase(std::remove_if(activeProjectiles.begin(), activeProjectiles.end(), [](const Projectile& p){ return !p.active; }), activeProjectiles.end());

        float closestDist = 20.0f;

        if (mistEnemy.active) {
            float distToMonster = std::hypot(player.posX - mistEnemy.x, player.posY - mistEnemy.y);
            closestDist = std::min(closestDist, distToMonster);
            
            if (distToMonster < 8.0f && hasLineOfSight(player.posX, player.posY, mistEnemy.x, mistEnemy.y) && !player.inLocker) {
                player.health -= (6.0f / std::max(1.0f, distToMonster)) * dtSec;
                player.takingDamage = true;
            }
            mistEnemy.pathRecalculateTimer -= dtSec;
            if(mistEnemy.pathRecalculateTimer <= 0.0f) {
                int targetX = rand() % MAP_W;
                int targetY = rand() % MAP_H;
                if(worldMap[targetY][targetX].wallType == 0) {
                    mistEnemy.currentPath = findPath({(int)mistEnemy.x, (int)mistEnemy.y}, {targetX, targetY});
                }
                mistEnemy.pathRecalculateTimer = 5.0f + (rand() % 5);
            }
            
            if (!mistEnemy.currentPath.empty()) {
                Point nextWaypoint = mistEnemy.currentPath.front();
                moveEnemyToward(mistEnemy, nextWaypoint.x + 0.5f, nextWaypoint.y + 0.5f, dtSec);
                 if (std::hypot(mistEnemy.x - (nextWaypoint.x + 0.5f), mistEnemy.y - (nextWaypoint.y + 0.5f)) < 0.5f) {
                    mistEnemy.currentPath.erase(mistEnemy.currentPath.begin());
                }
            }

            mistEnemy.animTimer += dtSec;
            if (mistEnemy.animTimer > 0.35f) { mistEnemy.currentFrame = 1 - mistEnemy.currentFrame; mistEnemy.animTimer = 0.0f; }
        }

        if (stalker.active && stalker.mode > 0 && !inCrawlspace) {
            float distToMonster = std::hypot(player.posX - stalker.x, player.posY - stalker.y);
            closestDist = std::min(closestDist, distToMonster);
            
            if (player.isSprinting) {
                stalker.awareness = std::min(5.0f, stalker.awareness + (float)dtSec * 0.1f);
            }

            if (stalker.mode == 1) { 
                stalker.isChasing = false;
                if (distToMonster < 5.0f && hasLineOfSight(player.posX, player.posY, stalker.x, stalker.y) && !player.inLocker) {
                    stalker.isChasing = true;
                    
                    if (!stalker.spottedPlayer) {
                        stalker.spottedPlayer = true;
                        SDL_LockAudioDevice(audioDevice);
                        audioState.monsterVocalType = 2; 
                        audioState.monsterVocalPhase = 0.0f;
                        audioState.monsterVocalTimer = 1.0f;
                        SDL_UnlockAudioDevice(audioDevice);
                    }

                    if (distToMonster < 2.0f) {
                        stalker.speed = 4.0f;
                        moveEnemyToward(stalker, player.posX, player.posY, dtSec);
                        if (distToMonster < 1.0f) stalker.active = false;
                    }
                } else {
                    stalker.speed = 1.0f; 
                    if (!hasLineOfSight(player.posX, player.posY, stalker.x, stalker.y) || distToMonster >= 15.0f) {
                        stalker.spottedPlayer = false;
                    }
                }
            }
            else if (stalker.mode == 2) { 
                stalker.speed = 1.8f + (stalker.awareness * 0.2f);
                if (stalker.enragedTimer > 0.0f) stalker.speed += 1.5f;

                if (!stalker.wasHunting) {
                    stalker.wasHunting = true;
                    SDL_LockAudioDevice(audioDevice);
                    audioState.monsterVocalType = 1; 
                    audioState.monsterVocalPhase = 0.0f;
                    audioState.monsterVocalTimer = 2.0f;
                    SDL_UnlockAudioDevice(audioDevice);
                }

                if (rand() % 1000 < 2) {
                    SDL_LockAudioDevice(audioDevice);
                    if (audioState.monsterVocalTimer <= 0.0f) {
                        audioState.monsterVocalType = 1;
                        audioState.monsterVocalPhase = 0.0f;
                        audioState.monsterVocalTimer = 2.0f;
                    }
                    SDL_UnlockAudioDevice(audioDevice);
                }

                if (stalker.investigateTimer > 0.0f && distToMonster > 3.5f) {
                    stalker.isChasing = false; 
                    stalker.investigateTimer -= dtSec;
                    
                    float distToTarget = std::hypot(stalker.investigateX - stalker.x, stalker.investigateY - stalker.y);
                    if (distToTarget > 0.5f) {
                        stalker.animTimer += dtSec;
                        if (stalker.animTimer > 0.25f) { stalker.currentFrame = 1 - stalker.currentFrame; stalker.animTimer = 0.0f; }

                        float dx = (stalker.investigateX - stalker.x) / distToTarget;
                        float dy = (stalker.investigateY - stalker.y) / distToTarget;
                        moveEnemyToward(stalker, stalker.x + dx, stalker.y + dy, dtSec);
                    }
                }
                else {
                    stalker.pathRecalculateTimer -= dtSec;
                    if (stalker.pathRecalculateTimer <= 0.0f) {
                        if (player.inLocker) {
                            if(stalker.isChasing && distToMonster < 6.0f) {
                                stalker.currentPath = findPath({(int)stalker.x, (int)stalker.y}, {(int)player.posX, (int)player.posY});
                            } else {
                                stalker.isChasing = false;
                                int tX = rand() % MAP_W;
                                int tY = rand() % MAP_H;
                                if(worldMap[tY][tX].wallType == 0 || worldMap[tY][tX].wallType == 5) {
                                    stalker.currentPath = findPath({(int)stalker.x, (int)stalker.y}, {tX, tY});
                                }
                            }
                        } else {
                            stalker.currentPath = findPath({(int)stalker.x, (int)stalker.y}, {(int)player.posX, (int)player.posY});
                        }
                        stalker.pathRecalculateTimer = std::max(0.2f, 1.0f - (stalker.awareness * 0.15f));
                    }
                    
                    if (!stalker.currentPath.empty()) {
                        stalker.isChasing = true;
                        Point nextWaypoint = stalker.currentPath.front();
                        
                        if (worldMap[nextWaypoint.y][nextWaypoint.x].wallType == 4) {
                            stalker.doorAttackTimer += dtSec;
                            if (stalker.doorAttackTimer >= 2.0f) {
                                worldMap[nextWaypoint.y][nextWaypoint.x].wallType = 0; 
                                stalker.doorAttackTimer = 0.0f;
                                SDL_LockAudioDevice(audioDevice);
                                audioState.itemSoundType = 7;
                                audioState.itemSoundPhase = 0.0f;
                                audioState.itemSoundTimer = 0.5f;
                                SDL_UnlockAudioDevice(audioDevice);
                            }
                        } else {
                            moveEnemyToward(stalker, nextWaypoint.x + 0.5f, nextWaypoint.y + 0.5f, dtSec);
                            if (std::hypot(stalker.x - (nextWaypoint.x + 0.5f), stalker.y - (nextWaypoint.y + 0.5f)) < 0.5f) {
                                stalker.currentPath.erase(stalker.currentPath.begin());
                            }
                        }
                    } else {
                        stalker.isChasing = false;
                    }
                }
            }

            if (stalker.mode != 2) {
                stalker.wasHunting = false;
            }

            if (!player.inLocker) {
                player.sanity -= (6.0f / std::max(1.0f, distToMonster)) * dtSec;
                if (distToMonster < 0.75f) {
                    deathReason = "CAUGHT BY THE ENTITY";
                    currentState = STATE_JUMPSCARE;
                    jumpscareTimer = 2.5f; 
                    setCaptureMouse(false);
                    SDL_LockAudioDevice(audioDevice);
                    audioState.gameState = currentState;
                    audioState.inGame = false; 
                    audioState.isJumpscare = true;
                    SDL_UnlockAudioDevice(audioDevice);
                    return;
                }
            }
        } else if (stalker.active && inCrawlspace) {
             stalker.isChasing = false;
             stalker.currentPath.clear();
        }

        player.sanity = std::max(0.0f, player.sanity);
        player.health = std::max(0.0f, player.health);

        if (player.sanity <= 0.0f) {
            deathReason = "LOST TO THE TERROR (SANITY DEPLETED)";
            currentState = STATE_GAMEOVER;
            setCaptureMouse(false);
        } else if (player.health <= 0.0f) {
            if (mistEnemy.active && stalker.active) deathReason = "OVERCOME BY THE HORRORS";
            else if (mistEnemy.active) deathReason = "CONSUMED BY THE MIST";
            else deathReason = "HEALTH DEPLETED";

            currentState = STATE_GAMEOVER;
            setCaptureMouse(false);
        } 

        if (currentState != STATE_PLAYING) {
            SDL_LockAudioDevice(audioDevice);
            audioState.gameState = currentState;
            audioState.inGame = false;
            SDL_UnlockAudioDevice(audioDevice);
            return;
        }

        int themeId = (currentLevel <= 10) ? 0 : (currentLevel - 11) % 4;
        int floorSurface = 0;
        if (themeId == 1 || worldMap[int(player.posY)][int(player.posX)].floorH > 0.01f) floorSurface = 1; 
        else if (themeId == 2) floorSurface = 2; 

        SDL_LockAudioDevice(audioDevice);
        float activeCorr = (player.toxicTimer > 0.0f) ? 0.9f : corruptionLevel;
        audioState.gameState = currentState;
        audioState.currentLevel = currentLevel;
        audioState.sanity = player.sanity;
        audioState.closestEnemyDist = closestDist;
        audioState.isChasing = (stalker.active && stalker.isChasing);
        audioState.corruption = activeCorr;
        audioState.isMoving = (player.forward != 0 || player.strafe != 0) && !player.inLocker;
        audioState.isSprinting = player.isSprinting;
        audioState.isCrouching = player.isCrouching;
        audioState.floorSurface = floorSurface;
        SDL_UnlockAudioDevice(audioDevice);
    }

