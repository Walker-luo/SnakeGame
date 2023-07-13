#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <time.h>
#include <memory>

#include "snake.h"

class Time
{
    int second;
    int minute;
    clock_t start;
    clock_t call;

public:
    Time():second(0),minute(0){start = clock();}
    void zero();
    std::string gettime()const;
};

class Game
{
public:
    Game();
    ~Game();

    void helpMenu();
    int renderStartMenu();
    void createInformationBoard();
    void renderInformationBoard() const;
    void renderSpeed() const;

    void createGameBoard();
    void renderGameBoard() const;

    void createInstructionBoard();
    void renderInstructionBoard_1() const;
    void renderInstructionBoard_2() const;


    bool Pause();
    void loadLeadBoard();
    void updateLeadBoard();
    bool readLeaderBoard();
    bool updateLeaderBoard();
    bool writeLeaderBoard();
    void renderLeaderBoard() const;

    void renderBoards_1() const;
    void renderBoards_2() const;

    void initializeGame();
    bool chooseGameMode() const;
    void OneRunGame();
    void TwoRunGame();
    void renderLife_1() const;
    void renderLife_2() const;
    void renderPoints_1() const;
    void renderPoints_2() const;
    void renderDifficulty_1() const;
    void renderDifficulty_2() const;

	//add the random obstacle
    void addObstacle();
    void renderObstacle();
    bool checkHitObstacle();
    bool isPartOfObstacles(int x, int y);


    //create the fruit of life
    void createLifeFruit();

    void createRandomFood();
    void createRandomAward();
    //create the random food and award;
    //create award -> return true else false
    bool createRandom();
    void renderRandom() const;

    void renderSnake() const;
    void OneControlSnake();
    void TwoControlSnake();
    bool hitEachOther() const;

    void startGame();
    bool renderRestartMenu_1() const;
    bool renderRestartMenu_2() const;
    void adjustDelay();



private:
    // We need to have two windows
    // One is for game introduction
    // One is for game mWindows
    Time time;
    int mScreenWidth;
    int mScreenHeight;
    int mGameBoardWidth;
    int mGameBoardHeight;
    const int mInformationHeight = 6;
    const int mInstructionWidth = 18;
    std::vector<WINDOW *> mWindows;
    // Snake information
    const int mInitialSnakeLength = 2;
    const char mSnakeSymbol_1 = '@';
    const char mSnakeSymbol_2 = 'O';
    std::unique_ptr<Snake> mPtrSnake_1;
    std::unique_ptr<Snake> mPtrSnake_2;
    bool GameMode;
    //Obstacle information
    std::vector<SnakeBody> mObstacles;
    const char mObstacleSymbol = '+';
    int numObstacles;
    // Food information
    SnakeBody mFood;
    const char mFoodSymbol = '#';
    // Award information
    SnakeBody mAward;
    bool mAwardExist;
    const char mAwardSymbol = '$';
    //Life information
    int mLife_1;
    int mLife_2;
    bool mLifeFruitExist;
    SnakeBody mLifeFruit;
    const char mLifeFruitSymbol = '&';

    int mPoints_1 = 0;
    int mPoints_2 = 0;
    int mDifficulty = 0;
    int mBaseDelay = 100;
    int mDelay;
    bool speed;
    int speedDelay;
    const std::string mRecordBoardFilePath = "record.dat";
    std::vector<int> mLeaderBoard;
    const int mNumLeaders = 4;
    bool isEnd = 0;
};

#endif
