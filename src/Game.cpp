#include <cstdlib>

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
    spawnShape();

    mPlayfield = new block*[PLAYFIELD_HEIGHT];
    for(int i = 0; i < PLAYFIELD_HEIGHT; i++){
        mPlayfield[i] = new block[PLAYFIELD_WIDTH];
    }
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
        else if(mCurrent.colliders[i].y < 0){
            mCurrent.y += BLOCK_SIZE;
            updateColliders();
            i = -1;
        }
        else if(mCurrent.colliders[i].y + mCurrent.colliders[i].h > mMaxHeight){
            mCurrent.y -= BLOCK_SIZE;
            addToPlayfield();
            return;
        }
        for(int i = 0; i < TETRIS_PIECE_SIZE; i++){
            if(mPlayfield[mCurrent.colliders[i].y / BLOCK_SIZE][mCurrent.colliders[i].x / BLOCK_SIZE].active){
                if(mLeft)
                    mCurrent.x += BLOCK_SIZE;
                if(mRight)
                    mCurrent.x -= BLOCK_SIZE;
                if(mDown){
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
    if(mLeft){
        mCurrent.x -= 30;
        updateColliders();
        checkCollisions();
        mLeft = false;
    }
    if(mRight){
        mCurrent.x += 30;
        updateColliders();
        checkCollisions();
        mRight = false;
    }
    /*if(up){
        current.y -= 30;
        updateColliders();
        checkCollision();
        up = false;
    }*/
    if(mDown){
        mCurrent.y += 30;
        updateColliders();
        checkCollisions();
        mDown = false;
    }
}

void Game::initPlayfield(){
    for(int i = 0; i < 24; i++){
        for(int j = 0; j < 10; j++){
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
                mPlayfield[mCurrent.y / BLOCK_SIZE + i][mCurrent.x / BLOCK_SIZE + j].active = true;
                mPlayfield[mCurrent.y / BLOCK_SIZE + i][mCurrent.x / BLOCK_SIZE + j].color = mCurrent.color;
            }
        }
    }
    spawnShape();
}

void Game::drawField(SDL_Renderer* gRenderer){
    for(int i = 0; i < PLAYFIELD_HEIGHT; i++){
        for(int j = 0; j < PLAYFIELD_WIDTH; j++){
            SDL_Rect rect = {j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            SDL_SetRenderDrawColor(gRenderer, mPlayfield[i][j].color.r, mPlayfield[i][j].color.g, mPlayfield[i][j].color.b, mPlayfield[i][j].color.a);
            SDL_RenderFillRect(gRenderer, &rect);
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderDrawRect(gRenderer, &rect);
        }
    }
}

void Game::drawPiece(SDL_Renderer* gRenderer){
    for(int i = 0; i < mCurrent.size; i++){
        for(int j = 0; j < mCurrent.size; j++){
            if(mCurrent.data[i][j] == 1){
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
    mCurrent = gTetrisPieces[rand() % TETRIS_PIECES_TOTAL];
    mCurrent.x = 3 * BLOCK_SIZE;
    mCurrent.y = 0;
}