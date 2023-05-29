#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Game.hpp"
#include "LTexture.hpp"

class App {
    public:
        App();
        ~App();
        void run();
    private:

        void init();
        void handleEvents();
        void update();
        void render();
        void darkenBackground();
        void renderBackground();

        int SCREEN_WIDTH = 300;
        int SCREEN_HEIGHT = 600;

        int screenWbeforeFS;
        int screenHbeforeFS;
        
        bool isBackgroundLoaded = false;

        SDL_Window* mWindow = NULL;
        SDL_Renderer* mRenderer = NULL;

        Game game;

        bool mQuit = false;
        LTexture* mSmallTexture;
        LTexture* mTitleTexture;
        LTexture* mBackgroundTexture;
        TTF_Font* mFontSmall, *mFontLarge;
};

#endif // APP_HPP