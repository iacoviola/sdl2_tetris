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

        void checkCollisions(char hasRotated = 0);
        void updateColliders();
        void updateMovement();
        void updateTime(Uint32 now);
        void spawnShape();
        void initPlayfield();
        void rotateShape(bool clockwise);
        shape transposeMatrix(shape &s);
        shape reverseColumns(shape &s);
        void addToPlayfield();

        void generatePermutation();
        int getNextShape();

        void drawField(SDL_Renderer* renderer);
        void drawPiece(SDL_Renderer* renderer);

        const char PLAYFIELD_HEIGHT = 24;
        const char PLAYFIELD_WIDTH = 10;
        const char PLAYFIELD_OFFSET = 4;

        block** mPlayfield;
        
        shape mCurrent;

        Uint32 mLastUpdate = 0;

        bool mLeft = false, mRight = false, mDown = false;
        bool mFall = false;

        int mShapeIndex = 7;

        int mMaxWidth, mMaxHeight;

        int* mShapeBag;

    friend class App;
};

#endif // GAME_HPP