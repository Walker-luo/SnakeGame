#ifndef SNAKE_H
#define SNAKE_H

#include <vector>

enum class Direction
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
};

class SnakeBody
{
public:
    SnakeBody();
    SnakeBody(int x, int y);
    int getX() const;
    int getY() const;
    bool operator == (const SnakeBody& snakeBody);
private:
    int mX;
    int mY;
};

// Snake class should have no depency on the GUI library
class Snake
{
public:
    //Snake();
    Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength);
    // Set random seed
    void setRandomSeed();
    // Initialize snake
    void initializeSnake();
    // Checking API for generating random food
    bool isPartOfSnake(int x, int y);
    //let the snake where the food is -> to decide whether eat it or not;
    void senseFood(SnakeBody food);
    bool touchFood();
    // Check if the snake is dead;
    bool hitWall();
    bool hitSelf();
    //hit wall or hit is self;
    bool checkCollision();

    bool changeDirection(Direction newDirection);
    std::vector<SnakeBody>& getSnake();
    int getLength();
    SnakeBody createNewHead();
    bool moveFoward();

private:
    const int mGameBoardWidth;
    const int mGameBoardHeight;
    // Snake information
    const int mInitialSnakeLength;
    Direction mDirection;
    SnakeBody mFood;
    std::vector<SnakeBody> mSnake;
};

#endif