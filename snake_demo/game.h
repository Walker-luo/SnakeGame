#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <memory>

#include "snake.h"


class Game
{
public:
    Game();
    ~Game();

		void createInformationBoard();
    void renderInformationBoard() const;

    void createGameBoard();
    void renderGameBoard() const;

		void createInstructionBoard();
    void renderInstructionBoard() const;

		void loadLeadBoard();
    void updateLeadBoard();
    bool readLeaderBoard();
    bool updateLeaderBoard();
    bool writeLeaderBoard();
    void renderLeaderBoard() const;

		void renderBoards() const;

		void initializeGame();
    void runGame();
    void renderPoints() const;
    void renderDifficulty() const;

	//add the random obstacle
    void addObstacle();
    void renderObstacle();
    bool checkHitObstacle();
    bool isPartOfObstacles(int x, int y);



    void createRandomFood();
    void createRandomAward();
    //create the random food and award;
    //create award -> return true else false
    bool createRandom();
    void renderRandom() const;

    void renderSnake() const;
    void controlSnake();

		void startGame();
    bool renderRestartMenu() const;
    void adjustDelay();



private:
    // We need to have two windows
    // One is for game introduction
    // One is for game mWindows
    int mScreenWidth;
    int mScreenHeight;
    int mGameBoardWidth;
    int mGameBoardHeight;
    const int mInformationHeight = 6;
    const int mInstructionWidth = 18;
    std::vector<WINDOW *> mWindows;
    // Snake information
    const int mInitialSnakeLength = 2;
    const char mSnakeSymbol = '@';
    std::unique_ptr<Snake> mPtrSnake;
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
    int mPoints = 0;
    int mDifficulty = 0;
    int mBaseDelay = 100;
    int mDelay;
    bool speed;
    int speedDelay;
    const std::string mRecordBoardFilePath = "record.dat";
    std::vector<int> mLeaderBoard;
    const int mNumLeaders = 3;
};

#endif
