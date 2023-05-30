#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Game.hpp"
#include "LTexture.hpp"

/*
*   The path to the background images
*/
static std::string path = "../../res/backgrounds/";

/*
*   The background images' filenames
*/
static std::string backgrounds[3] = {
    "background_beach.jpg",
    //"background_farm.jpg",
    //"background_forest.jpg",
    "background_mountain.jpg",
    "background_desert.jpg"
};

/*
*   Class handling the application logic
*/
class App {
    public:
        /*
        *   Constructor for the App class
        */
        App();

        /*
        *   Destructor for the App class
        *   frees the memory allocated for all of the application's components
        */
        ~App();

        /*
        *   Runs the application
        */
        void run();
    private:

        /*
        *   Initializes the needed components of the application
        */
        void init();
        /*
        *   Handles the user's input
        */
        void handleEvents();
        /*
        *   Updates the game logic each frame (locked at 60 FPS)
        */
        void update();
        /*
        *   Renders the game each frame (locked at 60 FPS)
        */
        void render();
        /*
        *   Darkens the background on game over
        */
        void darkenBackground();
        /*
        *   Renders the background image
        */
        void renderBackground();

        // The current width of the window
        int mWindowWidth = 300;
        // The current height of the window
        int mWindowHeight = 600;

        // The width of the window before fullscreen
        int screenWbeforeFS;
        // The height of the window before fullscreen
        int screenHbeforeFS;
        
        // If the background image has been loaded
        bool isBackgroundLoaded = false;

        // The SDL window
        SDL_Window* mWindow = NULL;
        // The SDL renderer
        SDL_Renderer* mRenderer = NULL;

        // The current background image index
        int mBackgroundIndex = 0;

        // The game object handling the game logic
        Game game;

        // If the application is running
        bool mQuit = false;
        // The score and infos texture
        LTexture* mSmallTexture;
        // The title and game over texture
        LTexture* mTitleTexture;
        // The background image texture
        LTexture* mBackgroundTexture;
        // The fonts used in the application
        TTF_Font* mFontSmall, *mFontLarge;
        // Shape drop sound
        Mix_Chunk* mDropSound;
        // Background music
        Mix_Music* mMusic;
};

#endif // APP_HPP