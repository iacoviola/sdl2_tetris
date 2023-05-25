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

    TTF_CloseFont(mFontSmall);
    TTF_CloseFont(mFontLarge);
    
    mFontSmall = NULL;
    mFontLarge = NULL;

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
                        if(!game.mGameOver && !game.mStartScreen)
                            game.rotateShape(true);
                        break;
                    case SDLK_e:
                        if(!game.mGameOver && !game.mStartScreen)
                            game.rotateShape(false);
                        break;
                    case SDLK_LEFT:
                        if(!game.mGameOver && !game.mStartScreen)
                            game.mLeft = true;
                        break;
                    case SDLK_RIGHT:
                        if(!game.mGameOver && !game.mStartScreen)
                            game.mRight = true;
                        break;
                    case SDLK_DOWN:
                        if(!game.mGameOver && !game.mStartScreen)
                            game.mDown = true;
                        break;
                    case SDLK_RETURN:
                        if(game.mGameOver){
                            game.resetGame();
                            mScoreTexture->setFontSize(15);
                        } else if(game.mStartScreen){
                            game.mStartScreen = false;
                            game.spawnShape();
                            game.mLastUpdate = SDL_GetTicks();
                        }
                        break;
                }
            }
        }

        if(!game.mGameOver && !game.mStartScreen){
            game.updateTime(SDL_GetTicks());
            game.updateMovement();
        }

        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(mRenderer);

        game.drawField(this->mRenderer);
        
        if(!game.mGameOver && !game.mStartScreen)
            game.drawPiece(this->mRenderer);

        int scoreX = 10;
        int scoreY = 10;

        if(game.mGameOver || game.mStartScreen){
            if(game.mGameOver){
                mGameOverTexture->loadFromRenderedText("Game Over", {0xFF, 0xFF, 0xFF, 0xFF});
                mScoreTexture->setFontSize(25);
            }
            else
                mGameOverTexture->loadFromRenderedText("Tetris", {0xFF, 0xFF, 0xFF, 0xFF});
            
            mGameOverTexture->render((SCREEN_WIDTH - mGameOverTexture->getWidth()) / 2, (SCREEN_HEIGHT - mGameOverTexture->getHeight()) / 2 - SCREEN_HEIGHT / 8);

            scoreX = (SCREEN_WIDTH - mScoreTexture->getWidth()) / 2;
            scoreY = (SCREEN_HEIGHT - mScoreTexture->getHeight()) / 2 - SCREEN_HEIGHT / 14;
        }

        std::stringstream scoreText;
        if(game.mStartScreen)
            scoreText << "Press Enter to start";
        else
            scoreText << "Score: " << game.mScore;

        if(mScoreTexture->loadFromRenderedText(scoreText.str(), {0xFF, 0xFF, 0xFF, 0xFF}))
            mScoreTexture->render(scoreX, scoreY);
        
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

    mFontSmall = TTF_OpenFont("../res/Roboto-Regular.ttf", 15);
    if(mFontSmall == NULL)
        throw std::runtime_error(TTF_GetError());

    mFontLarge = TTF_OpenFont("../res/Roboto-Regular.ttf", 45);
    if(mFontLarge == NULL)
        throw std::runtime_error(TTF_GetError());

    TTF_SetFontStyle(mFontLarge, TTF_STYLE_BOLD);

    mScoreTexture = new LTexture(mRenderer, mFontSmall);
    mGameOverTexture = new LTexture(mRenderer, mFontLarge);

    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}