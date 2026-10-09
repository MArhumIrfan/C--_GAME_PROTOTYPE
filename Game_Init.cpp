#include "Game.h"
#include "Font.h"
#include "Sprites.h"

#include <cmath>
#include <algorithm>
#include <cstdlib>

void WalkAsciiElevationEngine::initializeSprites() {
    const std::vector<std::string> rawStalker0 = {
        "                                                                   ",
        "                                                         .:-::....         .::.",
        "                              ..::----===========+++====+++++++:",
        "                            :-==++*##***+====. :=+*##%%%%######*#*+",
        "                       .::-==+#@@@@@@@@@#+-     :%@@@@@@@@@@%%#####* .",
        "                       -==+*%@@@@@@@@@@@@%-      =@@@@@@@@@@@@@@@%%**=.",
        "                       -+*#@@@@@@@@@@@@@@#       -@@@@@@@@@@@@@@%#*++=:.",
        "                        :=#@@@@@@@@@@@@#.       :#@@@@@@@@@@@@%*+-:.::-.",
        "                           =#@@@@@@@@%+.        .+-:+%@@@@@*-.     ..:-",
        "                              :======:           ==.  .              ..:",
        "                                                 :+:                 .:-",
        "                                                 .:.               ..--* .",
        "                                        ::.   :=*#+#+              .:-+*",
        "                                       :=*#=:--+%@@%+             .:-+**=",
        "                                             :-#@%%#*=:           .:=+###",
        "                                                 :+*+:      ......:--+*%%%-",
        "                                        .-+-.   :**:   .:...::-::::=+#%%%#",
        "                                      -+%%==*#%%%@@%#*=---::--=----=*#%%%@",
        "                                   .=+@- :=+ :*:-###@@%***=:=======+*%@@%@",
        "                                  -*#-          :-=%@@@###*-++++=++*#%@@%*",
        "                         .       :%%  .:*@%%@%==%-=+ @@@*#%*++++++*#%@@@%",
        "                        .        #@@@@@@@@@@@@@@@@@@@@@#=%%%+++++*%@@@@%.",
        "                         ..     :@@@@@@@@@@@@@@@@@@@@@@%-#%%#+**#%@@@@@#",
        "                         ..     +@@@@@@@@@@@@@@@@@@@@@@@-#%%%#*#%@@@@@@",
        "                   .      .     *@@@@@@@@@@@@@@@@@@@@@@@:%@@%%#%%@@@@@",
        "                     -    .     +@@@@@@%@@@@@@@@@@@@@@@@:@%@%%%%@%@%#",
        "                          .     .@@@@%#@@@@@@@@@@@@@@@@+-@%%%%%@@%=",
        "                                 @@@@@@@@@@@@@@@@@@@@@@ +@%%#=#@@",
        "                                  @@@@@+*@@@@@@@@@@@@%: #%%%+=%@",
        "                                  . =+*@@@@@@@@@@@% =  .%%#%=+%",
        "                                         -*%*+.   +=*  *@%%%--",
        "                                      : -+==+++++*%   *@%%@*",
        "                                 .=:      .=*+-:    -%@%%",
        "                                   :**=:        -+%@@+",
        "                                        .-+**##="
    };

    auto padSprite = [](std::vector<std::string>& sprite, const std::vector<std::string>& rawSprite) {
        size_t maxLength = 0;
        for (const auto& s : rawSprite) {
            if (s.length() > maxLength) {
                maxLength = s.length();
            }
        }
        sprite.clear();
        for (const auto& s : rawSprite) {
            sprite.push_back(s);
            sprite.back().append(maxLength - s.length(), ' ');
        }
    };

    padSprite(spriteStalker0, rawStalker0);
    padSprite(spriteStalker1, rawStalker0);

    // Using R"( )" ensures backslashes don't break the ASCII formatting
    const std::vector<std::string> rawStatue = {
        R"(               .---.       )",
        R"(              / ,-- \    )",
        R"(      .--.   ( (. .) )   .--.    )",
        R"(   ,'    \  (.-`-'(_)  /    `. )",
        R"(  /       `-/ \ `.  \-'       \)",
        R"(  : (_,' .  / (.\_ ) \  . `._) : )",
        R"( |   `-'(_,\ \     / /._)`-'   | )",
        R"( |       .  `.\,O,'.'  .   :   |)",
        R"( |   . : !   /\_  /\   ! . !   |)",
        R"( | ! |-'-|  : ""T"" :  |-'-| | |)",
        R"( | |-'   `-'|   H   |`-'   `-| |)",
        R"( `-'        |   H .:|        `-')",
        R"(            | . H !||  )",
        R"(            | . H !||  )",
        R"(            | . H !||  )",
        R"(            | . H !||  )",
        R"(            | . H |||  )",
        R"(            | . H |||  )",
        R"(            /_,'V.L|.\ )"
    };

    const std::vector<std::string> rawStatueJumpscare = {
        R"(  -. -. `.  / .-' _.'  _)",
        R"(.--`. `. `| / __.-- _' `)",
        R"('.-.  \  \ |  /   _.' `_)",
        R"(.-. \  `  || |  .' _.-' `.)",
        R"(.' _ \ '  -    -'  - ` _.-.)",
        R"( .' `. \%\%\%\%\%   | \%\%\%\%\% _.-.`-)",
        R"(.' .-. ><(@)> ) ( <(@)>< .-.`.)",
        R"( (("`(   -   | |   -   )'")))",
        R"(/ \#)\    (.(_).)    /(#//\)",
        R"(' / ) ((  /   | |   \  )) (`.`.)",
        R"(.'  (.) \ .md88o88bm. / (.) \))",
        R"(  / /| / \ `Y88888Y' / \ | \ \)",
        R"(.' / O  / `.   -   .' \  O \ \)",
        R"(/ /(O)/ /| `.___.' | \(O) \)",
        R"( / / / / |  |   |  |\  \  \ \)",
        R"(/ / // /|  |   |  |  \  \ \ )",
        R"(_.--/--/'( ) ) ( ) ) )`\-\-\-._)",
        R"(( ( ( ) ( ) ) ( ) ) ( ) ) ) ( ) ))"
    };

    padSprite(spriteStatue, rawStatue);
    padSprite(spriteStatueJumpscare, rawStatueJumpscare);
}
void WalkAsciiElevationEngine::updateWindowScale() {
    if (window) {
        int targetW = RESOLUTION_PRESETS[currentResIndex].width;
        int targetH = RESOLUTION_PRESETS[currentResIndex].height;
        SDL_SetWindowSize(window, targetW, targetH);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

void WalkAsciiElevationEngine::setCaptureMouse(bool capture) {
    SDL_SetRelativeMouseMode(capture ? SDL_TRUE : SDL_FALSE);
    SDL_SetWindowGrab(window, capture ? SDL_TRUE : SDL_FALSE);
}

void WalkAsciiElevationEngine::generateProceduralMultiLevelMaze() {
    srand(static_cast<unsigned int>(time(nullptr)) + currentLevel * 1337);
    itemsInWorld.clear();
    activeProjectiles.clear(); 

    for (int r = 0; r < MAP_H; ++r) {
        for (int c = 0; c < MAP_W; ++c) {
            worldMap[r][c].wallType = 1;
        }
    }

    std::stack<Point> stack;
    startPos = { 1, 1 };
    worldMap[startPos.y][startPos.x].wallType = 0;
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
                if (worldMap[ny][nx].wallType == 1) dirs.push_back(i);
            }
        }

        if (!dirs.empty()) {
            int d = dirs[rand() % dirs.size()];
            worldMap[curr.y + dy[d] / 2][curr.x + dx[d] / 2].wallType = 0;
            worldMap[curr.y + dy[d]][curr.x + dx[d]].wallType = 0;
            stack.push({ curr.x + dx[d], curr.y + dy[d] });
        } else {
            stack.pop();
        }
    }

    for (int y = 2; y < MAP_H - 2; ++y) {
        for (int x = 2; x < MAP_W - 2; ++x) {
            if (worldMap[y][x].wallType == 0) {
                bool ns = (worldMap[y-1][x].wallType == 1 && worldMap[y+1][x].wallType == 1 && worldMap[y][x-1].wallType == 0 && worldMap[y][x+1].wallType == 0);
                bool ew = (worldMap[y][x-1].wallType == 1 && worldMap[y][x+1].wallType == 1 && worldMap[y-1][x].wallType == 0 && worldMap[y+1][x].wallType == 0);
                if ((ns || ew) && (rand() % 100 < 30)) {
                    worldMap[y][x].wallType = 4; 
                }
            }
        }
    }

    for (int y = 2; y < MAP_H - 2; ++y) {
        for (int x = 2; x < MAP_W - 2; ++x) {
            if (worldMap[y][x].wallType == 1) {
                int adjFloors = 0;
                if(worldMap[y-1][x].wallType == 0) adjFloors++;
                if(worldMap[y+1][x].wallType == 0) adjFloors++;
                if(worldMap[y][x-1].wallType == 0) adjFloors++;
                if(worldMap[y][x+1].wallType == 0) adjFloors++;
                if (adjFloors == 1 && (rand() % 100 < 25)) {
                    worldMap[y][x].wallType = 6; 
                }
            }
        }
    }

    std::vector<Point> potentialVents;
    std::vector<Point> emptyFloorSpaces;
    for (int y = 1; y < MAP_H - 1; ++y) {
        for (int x = 1; x < MAP_W - 1; ++x) {
            if (worldMap[y][x].wallType == 1) {
                bool horiz = (worldMap[y][x-1].wallType == 0 && worldMap[y][x+1].wallType == 0);
                bool vert  = (worldMap[y-1][x].wallType == 0 && worldMap[y+1][x].wallType == 0);
                if (horiz || vert) potentialVents.push_back({x, y});
            }
            if (worldMap[y][x].wallType == 0) emptyFloorSpaces.push_back({x, y});
        }
    }

    int numVents = 1 + (rand() % 2); 
    while (numVents > 0 && !potentialVents.empty()) {
        int idx = rand() % potentialVents.size();
        Point v = potentialVents[idx];
        worldMap[v.y][v.x].wallType = 3; 
        potentialVents[idx] = potentialVents.back();
        potentialVents.pop_back();
        numVents--;
    }

    if (currentLevel >= 5 && !emptyFloorSpaces.empty()) {
        int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
        itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, ITEM_KEY});
        emptyFloorSpaces[idx] = emptyFloorSpaces.back(); emptyFloorSpaces.pop_back();
    }

    int numBread = 2 + (rand() % 3);
    int numMeds = 1 + (rand() % 2);
    int numPebbles = 3 + (rand() % 3);

    for (int i = 0; i < numBread && !emptyFloorSpaces.empty(); ++i) {
        int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
        itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, ITEM_BREAD});
        emptyFloorSpaces[idx] = emptyFloorSpaces.back(); emptyFloorSpaces.pop_back();
    }
    for (int i = 0; i < numMeds && !emptyFloorSpaces.empty(); ++i) {
        int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
        itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, ITEM_MEDS});
        emptyFloorSpaces[idx] = emptyFloorSpaces.back(); emptyFloorSpaces.pop_back();
    }
    for (int i = 0; i < numPebbles && !emptyFloorSpaces.empty(); ++i) {
        int idx = rand() % emptyFloorSpaces.size(); Point p = emptyFloorSpaces[idx];
        itemsInWorld.push_back({p.x + 0.5f, p.y + 0.5f, ITEM_PEBBLE});
        emptyFloorSpaces[idx] = emptyFloorSpaces.back(); emptyFloorSpaces.pop_back();
    }

    endPos = { MAP_W - 2, MAP_H - 2 };
    worldMap[endPos.y][endPos.x].wallType = 2;

    player.resetForNewLevel(startPos.x + 0.5f, startPos.y + 0.5f);
    player.hasKey = (currentLevel < 5);

    stalker.resetForNewLevel(MAP_W / 2 + 0.5f, MAP_H / 2 + 0.5f);
    mistEnemy.resetForNewLevel(MAP_W / 2 + 0.5f, MAP_H / 2 + 0.5f);
    statue.resetForNewLevel(MAP_W / 2 + 0.5f, MAP_H / 2 + 0.5f);
    
    if (currentLevel <= 10) {
        corruptionLevel = 0.0f;
        stalker.active = false;
        mistEnemy.active = false;
        statue.active = false;
    } else if (currentLevel >= 11 && currentLevel <= 15) {
        corruptionLevel = std::min(1.0f, (currentLevel - 10) * 0.1f);
        mistEnemy.active = true;
        mistEnemy.mode = 1; 
        stalker.active = false;
        statue.active = true;
        statue.speed = 4.5f; 
    } else if (currentLevel >= 16 && currentLevel <= 20) {
        corruptionLevel = std::min(1.0f, 0.5f + (currentLevel - 15) * 0.1f);
        stalker.active = true;
        stalker.mode = 1; 
        mistEnemy.active = false;
        statue.active = false;
    } else { 
        corruptionLevel = 1.0f;
        stalker.active = true;
        stalker.mode = 2; 
        mistEnemy.active = (rand() % 100 < 40);
        statue.active = false;
    }
}

void WalkAsciiElevationEngine::startNewGame() {
    currentLevel = 1;
    totalSteps = 0;
    levelTime = 0.0f;
    corruptionLevel = 0.0f; 
    player.sanity = 100.0f;
    player.health = 100.0f;
    player.stamina = 100.0f;
    
    for (int i = 0; i < 3; ++i) player.inventory[i] = ITEM_NONE;

    generateProceduralMultiLevelMaze();
    currentState = STATE_PLAYING;
    setCaptureMouse(true);

    SDL_LockAudioDevice(audioDevice);
    audioState.gameState = currentState;
    audioState.currentLevel = currentLevel;
    audioState.inGame = true;
    audioState.isJumpscare = false;
    audioState.sanity = 100.0f;
    audioState.closestEnemyDist = 20.0f;
    audioState.corruption = corruptionLevel;
    SDL_UnlockAudioDevice(audioDevice);
}

void WalkAsciiElevationEngine::nextLevel() {
    currentLevel++;
    player.sanity = std::min(100.0f, player.sanity + 30.0f);
    player.health = std::min(100.0f, player.health + 30.0f);
    generateProceduralMultiLevelMaze();
    currentState = STATE_PLAYING;
    setCaptureMouse(true);

    SDL_LockAudioDevice(audioDevice);
    audioState.gameState = currentState; 
    audioState.currentLevel = currentLevel;
    audioState.inGame = true;
    audioState.isJumpscare = false;
    audioState.corruption = corruptionLevel;
    SDL_UnlockAudioDevice(audioDevice);
}

bool WalkAsciiElevationEngine::init() {
    srand(static_cast<unsigned int>(time(nullptr)));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) return false;
    
    initializeSprites();

    window = SDL_CreateWindow(
        "Walk ASCII 3D Horror",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        RESOLUTION_PRESETS[currentResIndex].width,
        RESOLUTION_PRESETS[currentResIndex].height,
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

void WalkAsciiElevationEngine::cleanup() {
    setCaptureMouse(false);
    if (audioDevice != 0) SDL_CloseAudioDevice(audioDevice);
    if (screenTexture) SDL_DestroyTexture(screenTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}