#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include "Game.hpp"

class App {
    public:
        App();
        ~App();
        void run();
    private:

        void init();

        const int SCREEN_WIDTH = 300;
        const int SCREEN_HEIGHT = 600;

        SDL_Window* mWindow = NULL;
        SDL_Renderer* mRenderer = NULL;

        Game game;

        bool mQuit = false;
};

#endif // APP_HPP