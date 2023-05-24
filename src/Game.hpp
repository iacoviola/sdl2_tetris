#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <vector>

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
        void spawnShape();
        void initPlayfield();
        void rotateShape(bool clockwise);
        shape transposeMatrix(shape &s);
        shape reverseColumns(shape &s);
        void addToPlayfield();

        void drawField(SDL_Renderer* renderer);
        void drawPiece(SDL_Renderer* renderer);

        const char PLAYFIELD_HEIGHT = 24;
        const char PLAYFIELD_WIDTH = 10;

        block** mPlayfield;
        
        shape mCurrent;

        bool mLeft = false, mRight = false, mDown = false;

        int mMaxWidth, mMaxHeight;

    friend class App;
};

#endif // GAME_HPP