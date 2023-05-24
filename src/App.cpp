#include <stdexcept>

#include "App.hpp"

App::App() : game(SCREEN_WIDTH, SCREEN_HEIGHT){
    init();
}

App::~App(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    mWindow = NULL;
    mRenderer = NULL;

    SDL_Quit();
}

void App::run(){
    SDL_Event e;

    while(!mQuit){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                mQuit = true;
            } else if(e.type == SDL_KEYDOWN){
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE:
                        mQuit = true;
                        break;
                    case SDLK_r:
                        game.rotateShape(true);
                        break;
                    case SDLK_e:
                        game.rotateShape(false);
                        break;
                    case SDLK_LEFT:
                        game.mLeft = true;
                        break;
                    case SDLK_RIGHT:
                        game.mRight = true;
                        break;
                    case SDLK_DOWN:
                        game.mDown = true;
                        break;
                }
            }
        }

        game.updateTime(SDL_GetTicks());
        game.updateMovement();

        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(mRenderer);
        
        game.drawField(this->mRenderer);
        game.drawPiece(this->mRenderer);
        
        SDL_RenderPresent(mRenderer);
    }
}

void App::init(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error(SDL_GetError());

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        printf("Warning: Linear texture filtering not enabled!");

    mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if(mWindow == NULL)
        throw std::runtime_error(SDL_GetError());

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(mRenderer == NULL)
        throw std::runtime_error(SDL_GetError());

    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}