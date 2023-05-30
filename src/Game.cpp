#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>

#include "Game.hpp"

static const int TETRIS_PIECES_TOTAL = 7;
static const int TETRIS_PIECE_SIZE = 4;

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

    mStartingX = 0;
    mEndingX = mMaxWidth;
    mStartingY = 0;
    mEndingY = mMaxHeight;

    mPlayfield = new block*[PLAYFIELD_HEIGHT];
    for(int i = 0; i < PLAYFIELD_HEIGHT; i++){
        mPlayfield[i] = new block[PLAYFIELD_WIDTH];
    }

    mShapeBag = new int[TETRIS_PIECES_TOTAL];
    for(int i = 0; i < TETRIS_PIECES_TOTAL; i++){
        mShapeBag[i] = i;
    }


    initPlayfield();
}

Game::~Game(){
    for(int i = 0; i < PLAYFIELD_HEIGHT; i++){
        delete[] mPlayfield[i];
    }
    delete[] mPlayfield;
}

void Game::checkCollisions(int hasRotated){
    for(int i = 0; i < TETRIS_PIECE_SIZE; i++){
        if(mCurrent.colliders[i].x < mStartingX){
            mCurrent.x += mBlockSize;
            updateColliders(mCurrent);
            i = -1;
        } else if(mCurrent.colliders[i].x + mCurrent.colliders[i].w > mEndingX){
            mCurrent.x -= mBlockSize;
            updateColliders(mCurrent);
            i = -1;
        } else if(mCurrent.colliders[i].y + mCurrent.colliders[i].h > mEndingY){
            mCurrent.y -= mBlockSize;
            addToPlayfield();
            return;
        } else {
            if(mPlayfield[PLAYFIELD_OFFSET + abs(mCurrent.colliders[i].y - mStartingY) / mBlockSize][abs(mCurrent.colliders[i].x - mStartingX) / mBlockSize].active){
                if(mLeft)
                    mCurrent.x += mBlockSize;
                if(mRight)
                    mCurrent.x -= mBlockSize;
                if(mDown || mDrop){
                    mCurrent.y -= mBlockSize;
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

bool Game::checkGhostCollision(shape &s, int hasRotated){
    for(int i = 0; i < TETRIS_PIECE_SIZE; i++){
        if(s.colliders[i].y + s.colliders[i].h > mEndingY){
            s.y -= mBlockSize;
            return true;
        }
        else {
            if(mPlayfield[PLAYFIELD_OFFSET + abs(s.colliders[i].y - mStartingY) / mBlockSize][abs(s.colliders[i].x - mStartingX) / mBlockSize].active){
                s.y -= mBlockSize;
                return true;
            }
        }
    }
    return false;
}

void Game::updateColliders(shape &s){
    int k = 0;
    for(int i = 0; i < s.size; i++){
        for(int j = 0; j < s.size; j++){
            if(s.data[i][j] == 1){
                s.colliders[k++] = {s.x + j * mBlockSize, s.y + i * mBlockSize, mBlockSize, mBlockSize};
                if(k == TETRIS_PIECE_SIZE) return;
            }
        }
    }
}

void Game::updateMovement(){
    if(mLeft && !mDrop){
        mCurrent.x -= mBlockSize;
        updateColliders(mCurrent);
        checkCollisions();
        mLeft = false;
    }
    if(mRight && !mDrop){
        mCurrent.x += mBlockSize;
        updateColliders(mCurrent);
        checkCollisions();
        mRight = false;
    }
    if(mDown || mDrop){
        mCurrent.y += mBlockSize;
        updateColliders(mCurrent);
        checkCollisions();
        mDown = false;
        mDrop = false;
    }
}

void Game::updateGravity(){
    if(mDropCounter >= 1){
        mDropCounter = 0;
        mDrop = true;
    } else {
        mDropCounter += mGravity;
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
    updateColliders(mCurrent);
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
                mPlayfield[PLAYFIELD_OFFSET + (mCurrent.y - mStartingY) / mBlockSize + i][(mCurrent.x - mStartingX) / mBlockSize + j].active = true;
                mPlayfield[PLAYFIELD_OFFSET + (mCurrent.y - mStartingY) / mBlockSize + i][(mCurrent.x - mStartingX) / mBlockSize + j].color = mCurrent.color;
            }
        }
    }
    checkRows();
    checkGameOver();
    if(++mShapesPlaced % 10 == 0)
        mGravity *= 1.1f;
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
    mShapeIndex = 7;
    mLastUpdate = 0;
    mGameOver = false;
    mScore = 0;
    mShapesPlaced = 0;
    mGravity = 1.0f / 64.0f;
    mDropCounter = 0.0f;
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
            SDL_Rect rect = {j * mBlockSize + mStartingX, (i - PLAYFIELD_OFFSET) * mBlockSize + mStartingY, mBlockSize, mBlockSize};
            if(mPlayfield[i][j].active){
                SDL_SetRenderDrawColor(gRenderer, mPlayfield[i][j].color.r, mPlayfield[i][j].color.g, mPlayfield[i][j].color.b, mPlayfield[i][j].color.a);
                SDL_RenderFillRect(gRenderer, &rect);
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            } else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x5F);
                SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
            }
            
            SDL_RenderDrawRect(gRenderer, &rect);
        }
    }
}

void Game::drawPiece(SDL_Renderer* gRenderer){
    for(int i = 0; i < mCurrent.size; i++){
        for(int j = 0; j < mCurrent.size; j++){
            if(mCurrent.data[i][j] == 1 && mCurrent.y + i * mBlockSize >= mStartingY){
                SDL_Rect rect = {mCurrent.x + j * mBlockSize, mCurrent.y + i * mBlockSize, mBlockSize, mBlockSize};
                SDL_SetRenderDrawColor(gRenderer, mCurrent.color.r, mCurrent.color.g, mCurrent.color.b, mCurrent.color.a);
                SDL_RenderFillRect(gRenderer, &rect);
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderDrawRect(gRenderer, &rect);
            }
        }
    }
}

void Game::dropPiece(){
    mCurrent = mGhost;
    addToPlayfield();
}

void Game::spawnShape(){
    mCurrent = gTetrisPieces[getNextShape()];

    int x = mStartingX + 3 * mBlockSize; // (mEndingX - mStartingX - mCurrent.size * BLOCK_SIZE) / 2 + mStartingX;
    mCurrent.x = x; // - x % BLOCK_SIZE;
    mCurrent.y = -2 * mBlockSize + mStartingY;
    updateColliders(mCurrent);
}

void Game::reloadShape(int previousSize, int previousX, int previousY){
    mCurrent.x = ((mCurrent.x - previousX) / previousSize) * mBlockSize + mStartingX;
    mCurrent.y = ((mCurrent.y - previousY) / previousSize) * mBlockSize + mStartingY;
    updateColliders(mCurrent);
}

void Game::drawGhostPiece(SDL_Renderer* renderer){
    mGhost = mCurrent;
    bool collided = false;

    while(!collided){
        mGhost.y += mBlockSize;
        updateColliders(mGhost);
        collided = checkGhostCollision(mGhost);
    }
    for(int i = 0; i < mGhost.size; i++){
        for(int j = 0; j < mGhost.size; j++){
            if(mGhost.data[i][j] == 1){
                SDL_Rect rect = {mGhost.x + j * mBlockSize, mGhost.y + i * mBlockSize, mBlockSize, mBlockSize};
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }
}