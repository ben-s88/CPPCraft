#define SDL_MAIN_HANDLED
#include <iostream>
#include <Game.h>
#include <SDL.h>
#include <string>
#include <chrono>
#include <vector>
typedef std::chrono::steady_clock Clock;

int main()
{
    Game game;
    int lastUpdateTime = SDL_GetTicks();
    auto frameStartTime = Clock::now();
    double frameTime;
    int fps{};
    auto processStartTime = Clock::now();
    std::string title;

    if (!game.init(true))
    {
        return 1;
    }

    while (!game.quit)
    {
        frameStartTime = Clock::now();

        game.gameLoop();

        if (SDL_GetTicks() - lastUpdateTime >= 500)
        {
            frameTime = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - frameStartTime).count() / 1000000.f;
            fps = (frameTime > 0) ? (int)(1000 / frameTime) : 0;
            title = "CPP Craft - FPS: " + std::to_string(fps) + " FT: " + std::to_string(frameTime);
            game.setWindowTitle(title.c_str());
            lastUpdateTime = SDL_GetTicks();
        }
        
    }

    game.cleanUp();

    return 0;
}