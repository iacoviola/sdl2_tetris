#include <stdexcept>
#include <sstream>

#include "App.hpp"

App::App() : game(mWindowWidth, mWindowHeight){
    init();
}

App::~App(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    mWindow = NULL;
    mRenderer = NULL;

    mSmallTexture->free();
    mTitleTexture->free();
    mBackgroundTexture->free();

    mSmallTexture = NULL;
    mTitleTexture = NULL;
    mBackgroundTexture = NULL;

    TTF_CloseFont(mFontSmall);
    TTF_CloseFont(mFontLarge);
    
    mFontSmall = NULL;
    mFontLarge = NULL;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void App::run(){

    Uint32 a = SDL_GetTicks();
    Uint32 b = SDL_GetTicks();
    double delta = 0;

    while(!mQuit){

        a = SDL_GetTicks();
        delta = a - b;
        
        handleEvents();

        if(delta > 1000/60){
            if(!game.mGameOver && !game.mStartScreen){
                update();
            }

            render();
            b = a;
        }
    }
}

void App::init(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        throw std::runtime_error(SDL_GetError());
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
        puts("Warning: Linear texture filtering not enabled!");
    }

    mWindow = SDL_CreateWindow("SDL2 Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWindowWidth, mWindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(mWindow == NULL){
        throw std::runtime_error(SDL_GetError());
    }

    SDL_SetWindowMinimumSize(mWindow, mWindowWidth, mWindowHeight);

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if(mRenderer == NULL){
        throw std::runtime_error(SDL_GetError());
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG)){
        throw std::runtime_error(IMG_GetError());
    }

    SDL_Surface* loadedSurface = IMG_Load("../../res/icon/icon.png");
    if(loadedSurface == NULL){
        throw std::runtime_error(IMG_GetError());
    }
    
    SDL_SetWindowIcon(mWindow, loadedSurface);

    SDL_FreeSurface(loadedSurface);

    if(TTF_Init() == -1){
        throw std::runtime_error(TTF_GetError());
    }

    mFontSmall = TTF_OpenFont("../../res/fonts/Roboto-Regular.ttf", 15);
    if(mFontSmall == NULL){
        throw std::runtime_error(TTF_GetError());
    }

    mFontLarge = TTF_OpenFont("../../res/fonts/Roboto-Regular.ttf", 45);
    if(mFontLarge == NULL){
        throw std::runtime_error(TTF_GetError());
    }

    TTF_SetFontStyle(mFontLarge, TTF_STYLE_BOLD);

    mSmallTexture = new LTexture(mRenderer, mFontSmall);
    mTitleTexture = new LTexture(mRenderer, mFontLarge);

    mBackgroundTexture = new LTexture(mRenderer);
    if(mBackgroundTexture->loadFromFile(path + backgrounds[mBackgroundIndex])){
        isBackgroundLoaded = true;
    }

    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void App::handleEvents(){

    SDL_Event e;

    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT){
            mQuit = true;
        } else if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
            SDL_SetWindowSize(mWindow, e.window.data1, e.window.data2);

            SDL_GetWindowSize(mWindow, &mWindowWidth, &mWindowHeight);

            game.mMaxWidth = mWindowWidth;
            game.mMaxHeight = mWindowHeight;

            int prevBlockSize = game.mBlockSize;
            int prevStartingX = game.mStartingX;
            int prevStartingY = game.mStartingY;
            
            if(mWindowWidth > mWindowHeight / 2){
                game.mBlockSize = mWindowHeight / (game.PLAYFIELD_HEIGHT - game.PLAYFIELD_OFFSET);
            } else {
                game.mBlockSize = mWindowWidth / game.PLAYFIELD_WIDTH;
            }

            game.mStartingX = (mWindowWidth - game.mBlockSize * game.PLAYFIELD_WIDTH) / 2;
            game.mEndingX = game.mStartingX + game.mBlockSize * game.PLAYFIELD_WIDTH;

            game.mStartingY = (mWindowHeight - game.mBlockSize * (game.PLAYFIELD_HEIGHT - game.PLAYFIELD_OFFSET)) / 2;
            game.mEndingY = game.mStartingY + game.mBlockSize * (game.PLAYFIELD_HEIGHT - game.PLAYFIELD_OFFSET);
            
            game.reloadShape(prevBlockSize, prevStartingX, prevStartingY);

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
                    if(game.mStartScreen){
                        game.mStartScreen = false;
                        game.spawnShape();
                    } else {
                        game.resetGame();
                        mSmallTexture->setFontSize(15);
                    }
                    break;
                case SDLK_f:
                    if(SDL_GetWindowFlags(mWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP){
                        SDL_SetWindowFullscreen(mWindow, 0);
                        SDL_SetWindowSize(mWindow, screenWbeforeFS, screenHbeforeFS);
                    } else {
                        screenWbeforeFS = mWindowWidth;
                        screenHbeforeFS = mWindowHeight;
                        SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }
                    break;
                case SDLK_n:
                    if(isBackgroundLoaded){
                        mBackgroundIndex++;
                        if(mBackgroundIndex > 2){
                            mBackgroundIndex = 0;
                        }
                        if(mBackgroundTexture->loadFromFile(path + backgrounds[mBackgroundIndex])){
                            isBackgroundLoaded = true;
                        } else {
                            isBackgroundLoaded = false;
                        }
                    }
                    break;
                case SDLK_m:
                    if(isBackgroundLoaded){
                        mBackgroundIndex--;
                        if(mBackgroundIndex < 0){
                            mBackgroundIndex = 2;
                        }
                        if(mBackgroundTexture->loadFromFile(path + backgrounds[mBackgroundIndex])){
                            isBackgroundLoaded = true;
                        } else {
                            isBackgroundLoaded = false;
                        }
                    }
                    break;
                case SDLK_SPACE:
                    if(!game.mGameOver && !game.mStartScreen)
                        game.dropPiece();
                    break;
            }
        }
    }
}

void App::update(){
    game.updateGravity();
    game.updateMovement();
}

void App::render(){
    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(mRenderer);

    if(isBackgroundLoaded){
        renderBackground();
    }

    game.drawField(this->mRenderer);
    
    if(!game.mStartScreen){
        game.drawPiece(this->mRenderer);
    }

    if(!game.mGameOver && !game.mStartScreen){
        game.drawGhostPiece(this->mRenderer);
    }

    int scoreX = 10;
    int scoreY = 10;

    if(game.mGameOver){
        darkenBackground();
    }

    if(game.mGameOver || game.mStartScreen){
        if(game.mGameOver){
            mTitleTexture->loadFromRenderedText("Game Over", {0xFF, 0xFF, 0xFF, 0xFF});
            mSmallTexture->setFontSize(25);
        } else {
            mTitleTexture->loadFromRenderedText("Tetris", {0xFF, 0xFF, 0xFF, 0xFF});
        }
        
        mTitleTexture->render((mWindowWidth - mTitleTexture->getWidth()) / 2, (mWindowHeight - mTitleTexture->getHeight()) / 2 - mWindowHeight / 8);

        scoreX = (mWindowWidth - mSmallTexture->getWidth()) / 2;
        scoreY = (mWindowHeight - mSmallTexture->getHeight()) / 2 - mWindowHeight / 14;
    }

    std::stringstream scoreText;
    if(game.mStartScreen){
        scoreText << "Press Enter to start";
    } else {
        scoreText << "Score: " << game.mScore;
    }

    if(mSmallTexture->loadFromRenderedText(scoreText.str(), {0xFF, 0xFF, 0xFF, 0xFF})){
        mSmallTexture->render(scoreX, scoreY);
    }
    
    SDL_RenderPresent(mRenderer);
}

void App::darkenBackground(){
    SDL_Surface* back = SDL_CreateRGBSurface(0, mWindowWidth, mWindowHeight, 32, 0, 0, 0, 0);
    SDL_FillRect(back, NULL, SDL_MapRGB(back->format, 0, 0, 0));

    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(mRenderer, back);

    SDL_SetTextureBlendMode(backTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(backTexture, 140);

    SDL_RenderCopy(mRenderer, backTexture, NULL, NULL);

    SDL_FreeSurface(back);
    SDL_DestroyTexture(backTexture);
}

void App::renderBackground(){
    SDL_Rect stretch = {0, 0, 0, mWindowHeight};

    if(mWindowHeight > mWindowWidth){
        int visibleImage = (mWindowWidth * 100) / (mWindowHeight * 1.78);
        int bWidth = mBackgroundTexture->getWidth();
        int cropWidth = (bWidth - bWidth * (visibleImage / 100.0)) / 2.5;
        SDL_Rect crop = {cropWidth, 0, bWidth - cropWidth, mBackgroundTexture->getHeight()};
        stretch.w = (bWidth - cropWidth) / mBackgroundTexture->getHeight() * mWindowHeight;
        mBackgroundTexture->render(0, 0, &crop, &stretch);
    } else {
        stretch.w = mWindowWidth;
        mBackgroundTexture->render(0, 0, NULL, &stretch);
    }
}