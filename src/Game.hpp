#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <random>

typedef struct shape {
    SDL_Color color;
    char data[4][4];
    int x, y;
    int size;
    std::vector<SDL_Rect> colliders{4};
} shape;

typedef struct block {
    SDL_Color color;
    bool active;
} block;

class Game {
    public:
        Game(int max_width, int max_height);
        ~Game();
    private:

        void checkCollisions(int hasRotated = 0);
        bool checkGhostCollision(shape &s, int hasRotated = 0);
        void updateColliders(shape &s);
        void updateMovement();
        void updateGravity();
        void spawnShape();
        void reloadShape(int previousSize, int previousX, int previousY);
        void initPlayfield();
        void rotateShape(bool clockwise);
        shape transposeMatrix(shape &s);
        shape reverseColumns(shape &s);
        void addToPlayfield();
        void checkRows();
        void clearRow(int row);
        void shiftDown(int row);
        void checkGameOver();
        void resetGame();

        void generatePermutation();
        int getNextShape();

        void drawField(SDL_Renderer* renderer);
        void drawPiece(SDL_Renderer* renderer);
        void drawGhostPiece(SDL_Renderer* renderer);

        const int PLAYFIELD_HEIGHT = 24;
        const int PLAYFIELD_WIDTH = 10;
        const int PLAYFIELD_OFFSET = 4;

        int mBlockSize = 30;

        int mStartingX;
        int mEndingX;
        int mStartingY;
        int mEndingY;

        block** mPlayfield;
        
        shape mCurrent;

        Uint32 mLastUpdate = 0;

        bool mLeft = false, mRight = false, mDown = false;
        bool mDrop = false;
        bool mGameOver = false;
        bool mStartScreen = true;

        int mShapeIndex = 7;

        int mMaxWidth, mMaxHeight;

        int* mShapeBag;

        int mScore = 0;
        int mShapesPlaced = 0;

        float mGravity = 1.0f / 64.0f; //0.015625f;
        float mDropCounter = 0.0f;

    friend class App;
};

#endif // GAME_HPP