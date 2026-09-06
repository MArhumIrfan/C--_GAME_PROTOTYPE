#include "Game.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    WalkAsciiElevationEngine engine;
    if (engine.init()) engine.run();
    engine.cleanup();
    return 0;
}
