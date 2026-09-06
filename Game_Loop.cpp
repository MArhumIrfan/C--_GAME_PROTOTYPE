// The main run() loop tying update/render together with the fixed timestep.

#include "Game.h"
#include "Font.h"
#include "Sprites.h"

#include <cmath>
#include <algorithm>
#include <cstdlib>

void WalkAsciiElevationEngine::run() {
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

