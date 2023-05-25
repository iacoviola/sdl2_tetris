#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>

#include "Game.hpp"

static const char TETRIS_PIECES_TOTAL = 7;
static const char TETRIS_PIECE_SIZE = 4;
static const char BLOCK_SIZE = 30;

static const shape gTetrisPieces[TETRIS_PIECES_TOTAL] = {
    {
        {0x00, 0xFD, 0xFF, 0xFF},
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        0, 0, 4
    },
    {
        {0x04, 0x33, 0xFF, 0xFF},
        {
            {1, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        0, 0, 3
    },
    {
        {0xFF, 0xB3, 0x00, 0xFF},
        {
            {1, 1, 1, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        0, 0, 3
    },
    {
        {0x00, 0xF9, 0x00, 0xFF},
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        0, 0, 3
    },
    {
        {0xBB, 0x3A, 0xFF, 0xFF},
        {
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        0, 0, 3
    },
    {
        {0xFF, 0xFB, 0x00, 0xFF},
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        0, 0, 4
    },
    {
        {0xFF, 0x26, 0x00, 0xFF},
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        0, 0, 3
    },
};

Game::Game(int max_width, int max_height) : mMaxWidth(max_width), mMaxHeight(max_height){
    srand(time(NULL));

    mPlayfield = new block*[PLAYFIELD_HEIGHT];
    for(int i = 0; i < PLAYFIELD_HEIGHT; i++){
        mPlayfield[i] = new block[PLAYFIELD_WIDTH];
    }

    mShapeBag = new int[TETRIS_PIECES_TOTAL];
    for(int i = 0; i < TETRIS_PIECES_TOTAL; i++){
        mShapeBag[i] = i;
    }


    initPlayfield();

    spawnShape();
}

Game::~Game(){
    for(int i = 0; i < PLAYFIELD_HEIGHT; i++){
        delete[] mPlayfield[i];
    }
    delete[] mPlayfield;
}

void Game::checkCollisions(char hasRotated){
    for(int i = 0; i < TETRIS_PIECE_SIZE; i++){
        if(mCurrent.colliders[i].x < 0){
            mCurrent.x += BLOCK_SIZE;
            updateColliders();
            i = -1;
        }
        else if(mCurrent.colliders[i].x + mCurrent.colliders[i].w > mMaxWidth){
            mCurrent.x -= BLOCK_SIZE;
            updateColliders();
            i = -1;
        }
        else if(mCurrent.colliders[i].y + mCurrent.colliders[i].h > mMaxHeight){
            mCurrent.y -= BLOCK_SIZE;
            addToPlayfield();
            return;
        }
        else {
            if(mPlayfield[PLAYFIELD_OFFSET + abs(mCurrent.colliders[i].y) / BLOCK_SIZE][abs(mCurrent.colliders[i].x) / BLOCK_SIZE].active){
                if(mLeft)
                    mCurrent.x += BLOCK_SIZE;
                if(mRight)
                    mCurrent.x -= BLOCK_SIZE;
                if(mDown || mFall){
                    mCurrent.y -= BLOCK_SIZE;
                    addToPlayfield();
                }
                if(hasRotated == 1){
                    rotateShape(false);
                } else if(hasRotated == -1){
                    rotateShape(true);
                }
                return;
            }
        }
    }
}

void Game::updateColliders(){
    int k = 0;
    for(int i = 0; i < mCurrent.size; i++){
        for(int j = 0; j < mCurrent.size; j++){
            if(mCurrent.data[i][j] == 1){
                mCurrent.colliders[k++] = {mCurrent.x + j * BLOCK_SIZE, mCurrent.y + i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                if(k == TETRIS_PIECE_SIZE) return;
            }
        }
    }
}

void Game::updateMovement(){
    if(mLeft && !mFall){
        mCurrent.x -= 30;
        updateColliders();
        checkCollisions();
        mLeft = false;
    }
    if(mRight && !mFall){
        mCurrent.x += 30;
        updateColliders();
        checkCollisions();
        mRight = false;
    }
    if(mDown || mFall){
        mCurrent.y += 30;
        updateColliders();
        checkCollisions();
        mDown = false;
        mFall = false;
    }
}

void Game::updateTime(Uint32 now){
    if(now - mLastUpdate > 1000){
        mFall = true;
        mLastUpdate = now;
    }
}

void Game::initPlayfield(){
    for(int i = 0; i < PLAYFIELD_HEIGHT; i++){
        for(int j = 0; j < PLAYFIELD_WIDTH; j++){
            mPlayfield[i][j].color = {0x00, 0x00, 0x00, 0xFF};
            mPlayfield[i][j].active = false;
        }
    }
}

void Game::rotateShape(bool clockwise){
    if(clockwise){
        mCurrent = transposeMatrix(mCurrent);
        mCurrent = reverseColumns(mCurrent);
    } else {
        mCurrent = reverseColumns(mCurrent);
        mCurrent = transposeMatrix(mCurrent);
    }
    updateColliders();
    checkCollisions(true);
}

shape Game::transposeMatrix(shape &s){
    shape t = s;
    for(int i = 0; i < s.size; i++){
        for(int j = 0; j < s.size; j++){
            t.data[i][j] = s.data[j][i];
        }
    }
    return t;
}

shape Game::reverseColumns(shape &s){
    shape t = s;
    for(int i = 0; i < s.size; i++){
        for(int j = 0; j < s.size; j++){
            char temp = t.data[i][j];
            t.data[i][j] = s.data[i][s.size - j - 1];
            s.data[i][s.size - j - 1] = temp;
        }
    }
    return t;
}

void Game::addToPlayfield(){
    for(int i = 0; i < mCurrent.size; i++){
        for(int j = 0; j < mCurrent.size; j++){
            if(mCurrent.data[i][j] == 1){
                mPlayfield[PLAYFIELD_OFFSET + mCurrent.y / BLOCK_SIZE + i][mCurrent.x / BLOCK_SIZE + j].active = true;
                mPlayfield[PLAYFIELD_OFFSET + mCurrent.y / BLOCK_SIZE + i][mCurrent.x / BLOCK_SIZE + j].color = mCurrent.color;
            }
        }
    }
    checkRows();
    checkGameOver();
    if(!mGameOver)
        spawnShape();
}

void Game::checkRows(){
    bool isFull = true;
    for(int i = PLAYFIELD_HEIGHT - 1; i > 0; i--){
        for(int j = 0; j < PLAYFIELD_WIDTH; j++){
            if(!mPlayfield[i][j].active) break;
            if(j == PLAYFIELD_WIDTH - 1){
                isFull = true;
                clearRow(i);
                shiftDown(i);
                mScore += 100;
                i++;
            }
        }
    }
    if(isFull){
        SDL_Delay(250);
        mLastUpdate = SDL_GetTicks();
    }
}

void Game::clearRow(int row){
    for(int i = 0; i < PLAYFIELD_WIDTH; i++){
        mPlayfield[row][i].active = false;
        mPlayfield[row][i].color = {0x00, 0x00, 0x00, 0xFF};
    }
}

void Game::shiftDown(int row){
    for(int i = row - 1; i > 0; i--){
        for(int j = 0; j < PLAYFIELD_WIDTH; j++){
            mPlayfield[i + 1][j].active = mPlayfield[i][j].active;
            mPlayfield[i + 1][j].color = mPlayfield[i][j].color;
        }
    }
    mLastUpdate = SDL_GetTicks();
}

void Game::checkGameOver(){
    for(int i = 0; i < PLAYFIELD_WIDTH; i++){
        if(mPlayfield[PLAYFIELD_OFFSET - 1][i].active){
            mGameOver = true;
            break;
        }
    }
}

void Game::resetGame(){
    SDL_Delay(1000);
    mShapeIndex = 7;
    mLastUpdate = 0;
    mGameOver = false;
    mScore = 0;
    initPlayfield();
    spawnShape();
}

void Game::generatePermutation(){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(mShapeBag, mShapeBag + (int)TETRIS_PIECES_TOTAL, gen);
}

int Game::getNextShape(){
    if(mShapeIndex == TETRIS_PIECES_TOTAL){
        generatePermutation();
        mShapeIndex = 0;
    }
    return mShapeBag[mShapeIndex++];
}

void Game::drawField(SDL_Renderer* gRenderer){
    for(int i = PLAYFIELD_OFFSET; i < PLAYFIELD_HEIGHT; i++){
        for(int j = 0; j < PLAYFIELD_WIDTH; j++){
            SDL_Rect rect = {j * BLOCK_SIZE, (i - PLAYFIELD_OFFSET) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            if(mPlayfield[i][j].active){
                SDL_SetRenderDrawColor(gRenderer, mPlayfield[i][j].color.r, mPlayfield[i][j].color.g, mPlayfield[i][j].color.b, mPlayfield[i][j].color.a);
                SDL_RenderFillRect(gRenderer, &rect);
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            } else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x0F);
                SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
            }
            
            SDL_RenderDrawRect(gRenderer, &rect);
        }
    }
}

void Game::drawPiece(SDL_Renderer* gRenderer){
    for(int i = 0; i < mCurrent.size; i++){
        for(int j = 0; j < mCurrent.size; j++){
            if(mCurrent.data[i][j] == 1 && mCurrent.y + i * BLOCK_SIZE >= 0){
                SDL_Rect rect = {mCurrent.x + j * BLOCK_SIZE, mCurrent.y + i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_SetRenderDrawColor(gRenderer, mCurrent.color.r, mCurrent.color.g, mCurrent.color.b, mCurrent.color.a);
                SDL_RenderFillRect(gRenderer, &rect);
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderDrawRect(gRenderer, &rect);
            }
        }
    }
}

void Game::spawnShape(){
    mCurrent = gTetrisPieces[getNextShape()];

    int x = (mMaxWidth - mCurrent.size * BLOCK_SIZE) / 2;
    mCurrent.x = x - x % BLOCK_SIZE;
    mCurrent.y = -60;
    updateColliders();
}