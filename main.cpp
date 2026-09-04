#include "Game.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Game game;

    if (game.init()) {
        game.run();
    }

    game.cleanup();

    return 0;
}