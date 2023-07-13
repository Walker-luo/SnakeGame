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
    Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength, int num = 1);
    // Set random seed
    void setRandomSeed();
    // Initialize snake
    void initializeSnake();
    // Checking API for generating random food
    bool isPartOfSnake(int x, int y);
    //let the snake know where the food is -> to decide whether eat it or not;
    void senseFood(SnakeBody food);
    bool touchFood();
    // let the snake know where the award is -> to decide whether eat it or not;
    void Awardsense(bool mAwardExist);
    void senseAward(SnakeBody award);
    bool touchAward();
    //LifeFruit information
    void senseLifeFruit(SnakeBody LifeFruit);
    void LifeFruitsense(bool LifeFruitExist);
    bool touchLifeFruit();
    bool WheAddLife();
    // Check if the snake is dead;
    bool hitWall();
    bool hitSelf();
    //hit wall or hit is self;
    bool checkCollision();

    bool changeDirection(Direction newDirection);
    Direction getDirection();
    std::vector<SnakeBody>& getSnake();
    int getLength();
    SnakeBody getHead();
    SnakeBody createNewHead();
    int moveFoward();

    void deletTail();

private:
    int numOfSnake;
    const int mGameBoardWidth;
    const int mGameBoardHeight;
    // Snake information
    const int mInitialSnakeLength;
    Direction mDirection;
    SnakeBody SnakeTail;
    SnakeBody mFood;
    SnakeBody mAward;
    bool AwardExist;
    //Life information
    SnakeBody mLifeFruit;
    bool mLifeFruitExist;
    bool AddLife;

    std::vector<SnakeBody> mSnake;
};

#endif
