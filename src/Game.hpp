#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <random>

/*
*   Struct representing a shape
*   @param color the color of the shape
*   @param data the shape of the tetromino
*   @param x the x position of the shape
*   @param y the y position of the shape
*   @param size the size of the shape (4x4) or (3x3)
*   @param colliders the colliders of the shape
*/
typedef struct shape {
    SDL_Color color;
    char data[4][4];
    int x, y;
    int size;
    std::vector<SDL_Rect> colliders{4};
} shape;

/*
*   Struct representing a block of the playfield
*   @param color the color of the block
*   @param active if the block is active or not
*/
typedef struct block {
    SDL_Color color;
    bool active;
} block;


/*
*   Class handling the game logic
*/
class Game {
    public:
        /*
        *   Constructor for the Game class
        *   @param max_width The maximum width of the window
        *   @param max_height The maximum height of the window
        */
        Game(int max_width, int max_height);

        /*
        *   Destructor for the Game class
        */
        ~Game();
    private:

        /*
        *   Checks for collisions between the current shape and the playfield
        *   @param hasRotated if the call to this function is due to a rotation of the shape
        *          0 if not, 1 if the shape has rotated clockwise, -1 if the shape has rotated counter-clockwise
        */
        void checkCollisions(int hasRotated = 0);

        /*
        *   Checks for collisions between the current shape's ghost and the playfield
        *   @return true if there is a collision, false otherwise
        */
        bool checkGhostCollision();

        /*
        *   Updates the colliders of the selected shape, can either be the current shape or the ghost shape
        *   @param s the shape to update the colliders of
        */
        void updateColliders(shape &s);

        /*
        *   Updates the position on the field of the current shape
        */
        void updateMovement();

        /*
        *   Increases the drop counter by the gravity value
        *   proceeds to drop the piece if the drop counter is greater than or equal to 1
        */
        void updateGravity();

        /*
        *   Spawns a new shape two blocks above the playfield
        */
        void spawnShape();

        /*
        *   Reloads the shape after a window resize event calculating the new position
        */
        void reloadShape(int previousSize, int previousX, int previousY);

        /*
        *   Initializes the playfield
        */
        void initPlayfield();

        /*
        *   Rotates the current shape clockwise or counter-clockwise
        */
        void rotateShape(bool clockwise);

        /*
        *   Transposes the shape
        *   @param s the shape to transpose
        *   @return the transposed shape
        * 
        *   @example
        *   1 1 1 0    1 0 0 0
        *   0 0 1 0 -> 1 0 0 0
        *   0 0 0 0    1 1 0 0
        */
        shape transposeMatrix(shape &s);

        /*
        *   Reverses the columns of the shape
        *   @param s the shape to reverse the columns of
        *   @return the shape with the columns reversed
        * 
        *   @example 
        *   1 1 1 0    0 1 1 1
        *   0 0 1 0 -> 0 1 0 0
        *   0 0 0 0    0 0 0 0
        */
        shape reverseColumns(shape &s);

        /*
        *   Adds the current shape to the playfield
        */
        void addToPlayfield();

        /*
        *   Checks for full rows and clears them by calling clearRow()
        */
        void checkRows();

        /*
        *   Clears a row and shifts the rows above it down by calling shiftDown()
        *   @param row the row to clear
        */
        void clearRow(int row);

        /*
        *   Shifts the rows above the given row down
        *   @param row the row to start shifting down from
        */
        void shiftDown(int row);

        /*
        *   Checks if the game is over, checking if the first row above the playfield has any active blocks
        */
        void checkGameOver();

        /*
        *   Resets the game to its initial state for a new game
        */
        void resetGame();

        /*
        *   Drops the current piece to the bottom of the playfield instantly
        */
        void dropPiece();

        /*
        *   Generates a new permutation of the shapes
        */
        void generatePermutation();

        /*
        *   Gets the next shape in the permutation
        *   @return the index of the next shape in the permutation
        */
        int getNextShape();


        /*
        *   Draws the playfield
        *   @param renderer the renderer to draw the playfield to
        */
        void drawField(SDL_Renderer* renderer);

        /*
        *   Draws the current piece
        *   @param renderer the renderer to draw the current piece to
        */
        void drawPiece(SDL_Renderer* renderer);

        /*
        *   Draws the ghost piece
        *   @param renderer the renderer to draw the ghost piece to
        */
        void drawGhostPiece(SDL_Renderer* renderer);

        // The height of the playfield
        const int PLAYFIELD_HEIGHT = 24;
        // The width of the playfield
        const int PLAYFIELD_WIDTH = 10;
        // The number of not visible rows above the playfield
        const int PLAYFIELD_OFFSET = 4;

        // The size of a block in pixels
        int mBlockSize = 30;

        // The starting X position of the playfield (if the window is resized, 0 otherwise)
        int mStartingX;
        // The ending X position of the playfield (if the window is resized, mMaxWidth otherwise)
        int mEndingX;
        // The starting Y position of the playfield (if the window is resized, 0 otherwise)
        int mStartingY;
        // The ending Y position of the playfield (if the window is resized, mMaxHeight otherwise)
        int mEndingY;

        // The playfield
        block** mPlayfield;
        
        // The current dropping shape
        shape mCurrent;
        // The current shape's ghost
        shape mGhost;

        // The direction the current shape is moving in
        bool mLeft = false, mRight = false, mDown = false;
        // If the current shape is dropping
        bool mDrop = false;
        // If the game is over
        bool mGameOver = false;
        // If the game is in the start screen
        bool mStartScreen = true;
        // If the current shape has been placed
        bool mPlaced = false;

        // The index of the current shape in the permutation
        int mShapeIndex = 7;

        // The maximum width and height of the window
        int mMaxWidth, mMaxHeight;

        // The permutation of the shapes
        int* mShapeBag;

        // The score of the game, incremented by 100 for each row cleared
        int mScore = 0;
        // The number of shapes placed
        int mShapesPlaced = 0;

        // The initial gravity value
        float mGravity = 1.0f / 64.0f; //0.015625f;
        // The counter for the gravity, incremented by the gravity value each frame
        float mDropCounter = 0.0f;
    friend class App;
};

#endif // GAME_HPP