#include "Game.h" // The only header you need to include in main.cpp

int main(int argc, char* argv[]) {
    // These lines are just to prevent compiler warnings about unused parameters.
    (void)argc;
    (void)argv;

    // 1. Create an instance of our main Game class.
    Game game;

    // 2. Initialize the game (creates window, renderer, loads assets, etc.).
    //    We check if it returns true, meaning everything was set up successfully.
    if (game.init()) {
        
        // 3. If initialization was successful, start the main game loop.
        //    The program will stay inside this 'run()' function until the user quits.
        game.run();
    }

    // 4. Once the game loop is finished, clean up all resources.
    //    (destroys window, closes SDL, etc.)
    game.cleanup();

    // 5. Exit the program successfully.
    return 0;
}