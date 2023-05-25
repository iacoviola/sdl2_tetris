#include <stdexcept>
#include <sstream>

#include "App.hpp"

App::App() : game(SCREEN_WIDTH, SCREEN_HEIGHT){
    init();
}

App::~App(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    mWindow = NULL;
    mRenderer = NULL;

    TTF_CloseFont(mFont);
    mFont = NULL;

    TTF_Quit();
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

        std::stringstream scoreText;
        scoreText << "Score: " << game.mScore;

        if(mScoreTexture->loadFromRenderedText(scoreText.str(), {0xFF, 0xFF, 0xFF, 0xFF}))
            mScoreTexture->render(10, 10);
        
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

    if (TTF_Init() == -1)
        throw std::runtime_error(TTF_GetError());

    mFont = TTF_OpenFont("../res/Roboto-Regular.ttf", 15);
    if(mFont == NULL)
        throw std::runtime_error(TTF_GetError());

    mScoreTexture = new LTexture(mRenderer, mFont);

    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}