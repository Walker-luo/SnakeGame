#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"


SnakeBody::SnakeBody()
{
}


SnakeBody::SnakeBody(int x, int y): mX(x), mY(y)
{
}

int SnakeBody::getX() const
{
    return mX;
}

int SnakeBody::getY() const
{
    return mY;
}

bool SnakeBody::operator == (const SnakeBody& snakeBody)
{
    return (this->getX() == snakeBody.getX() && this->getY() == snakeBody.getY());
}

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength): mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength)
{
    this->initializeSnake();
    this->setRandomSeed();
}

void Snake::setRandomSeed()
{
    // use current time as seed for random generator
    std::srand(std::time(nullptr));
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    for (int i = 0; i < this->mInitialSnakeLength; i ++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    this->mDirection = Direction::Up;
}

bool Snake::isPartOfSnake(int x, int y)
{
    for(SnakeBody cor: mSnake) {
        if(x == cor.getX() && y == cor.getY()) return true;
    }

    return false;
}

/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()
{
    if(mSnake[0].getX() >= mGameBoardWidth-1 || mSnake[0].getX() <= 0
       || mSnake[0].getY() >= mGameBoardHeight-1 || mSnake[0].getY() <= 0) return true;


    return false;
}

/*
 * The snake head is overlapping with its body
 */
bool Snake::hitSelf()
{
    if(mSnake.size() >= 5) {
        for(unsigned int i = 4; i < mSnake.size()-1; ++i) {
            if(mSnake[i] == mSnake[0])
                return true;
        }
    }

		return false;
}


bool Snake::touchFood()
{
    SnakeBody newHead = this->createNewHead();
    if (this->mFood == newHead) return true;
    else return false;
}


bool Snake::touchAward()
{
    SnakeBody newHead = this->createNewHead();
    if (this->mAward == newHead) return true;
    else return false;
}


void Snake::senseFood(SnakeBody food)
{
    this->mFood = food;
}

void Snake::Awardsense(bool mAwardExist)
{
    this->AwardExist = mAwardExist;
}

void Snake::senseAward(SnakeBody award)
{
    this->mAward = award;
}


std::vector<SnakeBody>& Snake::getSnake()
{
    return this->mSnake;
}

bool Snake::changeDirection(Direction newDirection)
{
    switch (this->mDirection)
    {
        case Direction::Up:
        {
            if(newDirection != Direction::Down) {
		if(newDirection == Direction::Up) return true;
                mDirection = newDirection;
		break;
            }
            else break;
        }

        case Direction::Down:
        {
            if(newDirection != Direction::Up) {
		if(newDirection == Direction::Down) return true;
                mDirection = newDirection;
		break;
            }
            else break;
        }

        case Direction::Left:
        {
            if(newDirection != Direction::Right) {
		if(newDirection == Direction::Left) return true;
                mDirection = newDirection;
		break;
            }
            else break;
        }

        case Direction::Right:
        {
            if(newDirection != Direction::Left) {
		if(newDirection == Direction::Right) return true;
                mDirection = newDirection;
                break;
            }
            else break;
        }
    }

    return false;
}

Direction Snake::getDirection()
{
    return mDirection;
}


SnakeBody Snake::createNewHead()
{
    int x = mSnake[0].getX();
    int y = mSnake[0].getY();
    switch (this->mDirection) {
    case Direction::Up:
        y -= 1;
        break;
    case Direction::Down:
        y += 1;
        break;
    case Direction::Left:
        x -= 1;
        break;
    case Direction::Right:
        x += 1;
        break;
    }


    SnakeBody newHead = SnakeBody(x,y);
    return newHead;
}

/*
 * If eat food, return true, otherwise return false
 */
int Snake::moveFoward()
{
    if(AwardExist) {
        if(this->touchAward()) {
            this->mSnake.insert(this->mSnake.begin(), mAward);
            this->mSnake.push_back(SnakeTail);
            return 2;
        }
    }
    if(this->touchFood()) {
        this->mSnake.insert(this->mSnake.begin(),mFood);
        return 1;
    }
    else {
        SnakeBody newHead = this->createNewHead();
        SnakeTail = mSnake.back();
        this->mSnake.pop_back();
        this->mSnake.insert(this->mSnake.begin(),newHead);
        return 0;
    }
}

bool Snake::checkCollision()
{
    if (this->hitWall() || this->hitSelf())
    {
        return true;
    }
    else
    {
        return false;
    }
}


int Snake::getLength()
{
    return this->mSnake.size();
}

