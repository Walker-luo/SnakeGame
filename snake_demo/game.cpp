#include <string>
#include <iostream>
#include <cmath>

// For terminal delay
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm>

#include "game.h"

Game::Game()
{
    // Separate the screen to three windows
    this->mWindows.resize(3);
    initscr();
    // If there wasn't any key pressed don't wait for keypress
    nodelay(stdscr, true);
    // Turn on keypad control
    keypad(stdscr, true);
    // No echo for the key pressed
    noecho();
    // No cursor show
    curs_set(0);
    //start the color
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    // Get screen and board parameters
    getmaxyx(stdscr, this->mScreenHeight, this->mScreenWidth);
    this->mGameBoardWidth = this->mScreenWidth - this->mInstructionWidth;
    this->mGameBoardHeight = this->mScreenHeight - this->mInformationHeight;

    this->createInformationBoard();
    this->createGameBoard();
    this->createInstructionBoard();

    // Initialize the leader board to be all zeros
    this->mLeaderBoard.assign(this->mNumLeaders, 0);
}

Game::~Game()
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        delwin(this->mWindows[i]);
    }
    endwin();
}

void Game::createInformationBoard()
{
    int startY = 0;
    int startX = 0;
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, startY, startX);
}

void Game::renderInformationBoard() const
{
    mvwprintw(this->mWindows[0], 1, 1, "Welcome to The Snake Game!");
    mvwprintw(this->mWindows[0], 2, 1, "This is a mock version.");
    mvwprintw(this->mWindows[0], 3, 1, "Implemented using C++ and libncurses library.");
    mvwprintw(this->mWindows[0], 4, 1, "Whether speed or not: ");

    wrefresh(this->mWindows[0]);
}

void Game::renderSpeed() const
{
	if(this->speed) mvwaddch(this->mWindows[0], 4, 23, 'Y' | COLOR_PAIR(1) | A_UNDERLINE);
	else  mvwaddch(this->mWindows[0], 4, 23, 'N' | COLOR_PAIR(3) | A_UNDERLINE);
	wrefresh(this->mWindows[0]);
 
}

void Game::createGameBoard()
{
    int startY = this->mInformationHeight;
    int startX = 0;
    this->mWindows[1] = newwin(this->mScreenHeight - this->mInformationHeight, this->mScreenWidth - this->mInstructionWidth, startY, startX);
}

void Game::renderGameBoard() const
{
    wrefresh(this->mWindows[1]);
}

void Game::createInstructionBoard()
{
    int startY = this->mInformationHeight;
    int startX = this->mScreenWidth - this->mInstructionWidth;
    this->mWindows[2] = newwin(this->mScreenHeight - this->mInformationHeight, this->mInstructionWidth, startY, startX);
}

void Game::renderInstructionBoard() const
{
    mvwprintw(this->mWindows[2], 1, 1, "Manual");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Life");
    mvwprintw(this->mWindows[2], 11, 1, "Points");
    mvwprintw(this->mWindows[2], 14, 1, "Difficulty");

    wrefresh(this->mWindows[2]);
}


void Game::renderLeaderBoard() const
{
    // If there is not too much space, skip rendering the leader board
    if (this->mScreenHeight - this->mInformationHeight - 18 - 2 < 3 * 2)
    {
        return;
    }
    mvwprintw(this->mWindows[2], 18, 1, "Leader Board");
    std::string pointString;
    std::string rank;
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 18 - 2); i ++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 18 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 18 + (i + 1), 5, pointString.c_str());
    }
    wrefresh(this->mWindows[2]);
}

bool Game::renderRestartMenu() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    int index = 0;
    int offset = 4;
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(menu, 2, 1, pointString.c_str());
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }

}

void Game::renderLife() const
{
	std::string Lifesting = std::to_string(this->mLife);
	mvwprintw(this->mWindows[2], 9, 1, Lifesting.c_str());
	wrefresh(this->mWindows[2]);
}

void Game::renderPoints() const
{
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(this->mWindows[2], 12, 1, pointString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::renderDifficulty() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2], 15, 1, difficultyString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::initializeGame()
{
    // allocate memory for a new snake
        this->mFood = SnakeBody(0,0); // avoid error happens when addObstacle
		this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));
		this->mPoints = 0;
		this->mLife = 2;
		this->numObstacles = 3;
		this->mObstacles.clear();
        this->addObstacle();
		this->createRandomFood();
        this->mPtrSnake->senseFood(mFood);
        this->mDelay = mBaseDelay;

	this->speed = false;
	this->speedDelay =mDelay/2;
}


void Game::createRandomFood()
{
    int x, y;
    while(true) {
        x = (rand() % this->mGameBoardWidth) - 1;
        y = (rand() % this->mGameBoardHeight) - 1;
        if(x < 1 || y < 1 || this->mPtrSnake->isPartOfSnake(x,y) || this->isPartOfObstacles(x,y)) continue;
        this->mFood = SnakeBody(x,y);
        break;
    }
}


void Game::createRandomAward()
{
    int x, y;
    while(true) {
        x = (rand() % this->mGameBoardWidth) - 1;
        y = (rand() % this->mGameBoardHeight) - 1;
        if(x < 1 || y < 1 || this->mPtrSnake->isPartOfSnake(x,y) || this->isPartOfObstacles(x,y)) continue;
        this->mAward = SnakeBody(x,y);
        if(mAward == mFood) continue;
        break;
    }

}


void Game::createLifeFruit()
{
	if(rand() % 2 == 0) this->mLifeFruitExist = true;
	else { 
		this->mLifeFruitExist = false;
		return;
	}
    	int x, y;
    	while(true) {
        x = (rand() % this->mGameBoardWidth) - 1;
        y = (rand() % this->mGameBoardHeight) - 1;
        if(x < 1 || y < 1 || this->mPtrSnake->isPartOfSnake(x,y) || this->isPartOfObstacles(x,y)) continue;
        this->mLifeFruit = SnakeBody(x,y);
        if(mLifeFruit == mFood) continue;
	if(this->mAwardExist && mLifeFruit == this->mAward) continue;
        break;
    }


}


bool Game::createRandom()
{
    this->createRandomFood();
    if(rand() % 2 == 0) {
        this->createRandomAward();
        return true;
    }
    else {
        this->mAwardExist = false;
        return false;
    }
}

void Game::renderRandom() const
{
    mvwaddch(this->mWindows[1], this->mFood.getY(), this->mFood.getX(), this->mFoodSymbol);
    if(mAwardExist)
        mvwaddch(this->mWindows[1], this->mAward.getY(), this->mAward.getX(), this->mAwardSymbol | COLOR_PAIR(2));
    if(this->mLifeFruitExist)
	   mvwaddch(this->mWindows[1], this->mLifeFruit.getY(), this->mLifeFruit.getX(), this->mLifeFruitSymbol | COLOR_PAIR(3) | A_BLINK | A_BOLD); 
    wrefresh(this->mWindows[1]);
}


void Game::addObstacle()
{
    int x, y;
    SnakeBody obstacle;
    while(true) {
        x = (rand() % this->mGameBoardWidth) - 1;
        y = (rand() % this->mGameBoardHeight) - 1;
	if(x<1 || y<1 || this->mPtrSnake->isPartOfSnake(x,y)) continue;
	obstacle = SnakeBody(x,y);
	if(obstacle == this->mFood) continue;
	if(this->mAwardExist && obstacle == this->mAward) continue;
	for(int i = 0; i < mObstacles.size(); ++i) {
        if(mObstacles[i] == obstacle) continue;
	}
    this->mObstacles.push_back(obstacle);
	if(this->mObstacles.size() == this->numObstacles) break;
    }
}

bool Game::isPartOfObstacles(int x, int y)
{
    for(SnakeBody cor: this->mObstacles){
        if(x == cor.getX() && y == cor.getY()) return true;
    }
    return false;
}

void Game::renderObstacle()
{
	for(int i = 0; i < this->numObstacles; ++i)
		mvwaddch(this->mWindows[1], this->mObstacles[i].getY(), this->mObstacles[i].getX(), this->mObstacleSymbol
           | COLOR_PAIR(1) | A_BOLD | A_STANDOUT);
	wrefresh(this->mWindows[1]);
}

bool Game::checkHitObstacle()
{
	SnakeBody head = this->mPtrSnake->getHead();
	for(int i = 0; i < this->numObstacles; ++i){
		if(this->mObstacles[i] == head) {
			if(this->speed && this->mPoints) {
				--this->mPoints;
				this->mPtrSnake->deletTail();
				return false;
			}
			return true;
		}
	}

	return false;
}

void Game::renderSnake() const
{
    int snakeLength = this->mPtrSnake->getLength();
    std::vector<SnakeBody>& snake = this->mPtrSnake->getSnake();
    for (int i = 0; i < snakeLength; i ++)
    {
        mvwaddch(this->mWindows[1], snake[i].getY(), snake[i].getX(), this->mSnakeSymbol);
    }
    wrefresh(this->mWindows[1]);
}

void Game::controlSnake()
{
    int key;
    key = getch();
    switch(key)
    {
        case 'W':
        case 'w':
        case KEY_UP:
        {
            this->speed = this->mPtrSnake->changeDirection(Direction::Up);
            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
            this->speed = this->mPtrSnake->changeDirection(Direction::Down);
            break;
        }
        case 'A':
        case 'a':
        case KEY_LEFT:
        {
            this->speed = this->mPtrSnake->changeDirection(Direction::Left);
            break;
        }
        case 'D':
        case 'd':
        case KEY_RIGHT:
        {
            this->speed = this->mPtrSnake->changeDirection(Direction::Right);
            break;
        }
        default:
        {
            break;
        }
    }
}

void Game::renderBoards() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard();
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        box(this->mWindows[i], 0, 0);
	wborder(this->mWindows[i], '|', '|', '-', '-', '+', '+', '+', '+');
        wrefresh(this->mWindows[i]);
    }
    this->renderLeaderBoard();
}

//change the difficulity
void Game::adjustDelay()
{
    this->mDifficulty = this->mPoints / 5;
    if (mPoints % 5 == 0)
    {
        this->mDelay = this->mBaseDelay * pow(0.75, this->mDifficulty);
	this->speedDelay = this->mDelay/2;
	this->numObstacles += 2;
	this->addObstacle();
    }
}

void Game::runGame()
{
    bool moveSuccess;
    int key;
    while (true){
        this->controlSnake();
        werase(this->mWindows[1]);
        box(this->mWindows[1], 0, 0);
        wborder(this->mWindows[1], '|', '|', '-', '-', '+', '+', '+', '+');

        int eaten = this->mPtrSnake->moveFoward();

        this->renderSnake();
	this->renderSpeed();
        if(eaten) {
	    if(this->mPtrSnake->WheAddLife()) ++this->mLife;
            this->mPoints += eaten;
            this->mAwardExist = this->createRandom();
	    this->createLifeFruit();
            this->mPtrSnake->Awardsense(mAwardExist);
            this->mPtrSnake->senseFood(this->mFood);
	    this->mPtrSnake->LifeFruitsense(this->mLifeFruitExist);

            if(this->mAwardExist) {
                this->mPtrSnake->senseAward(this->mAward);
            }
	    if(this->mLifeFruitExist)
		    this->mPtrSnake->senseLifeFruit(this->mLifeFruit);

            this->adjustDelay();
        }


        if(this->mPtrSnake->checkCollision()) --this->mLife;
        if(this->checkHitObstacle()) --this->mLife;
	if(!this->mLife) break;

        this->renderObstacle();
        this->renderRandom();

	this->renderLife();
        this->renderPoints();
        this->renderDifficulty();

	if(speed) std::this_thread::sleep_for(std::chrono::milliseconds(this->speedDelay));
	else std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));

        refresh();
    }
}

void Game::startGame()
{
    refresh();
    bool choice;
    while (true)
    {
        this->readLeaderBoard();
        this->renderBoards();
        this->initializeGame();
        this->runGame();
        this->updateLeaderBoard();
        this->writeLeaderBoard();
        choice = this->renderRestartMenu();
        if (choice == false)
        {
            break;
        }
    }
}

// https://en.cppreference.com/w/cpp/io/basic_fstream
bool Game::readLeaderBoard()
{
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.in);
    if (!fhand.is_open())
    {
        return false;
    }
    int temp;
    int i = 0;
    while ((!fhand.eof()) && (i < mNumLeaders))
    {
        fhand.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        this->mLeaderBoard[i] = temp;
        i ++;
    }
    fhand.close();
    return true;
}

bool Game::updateLeaderBoard()
{
    bool updated = false;
    int newScore = this->mPoints;
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        if (this->mLeaderBoard[i] >= this->mPoints)
        {
            continue;
        }
        int oldScore = this->mLeaderBoard[i];
        this->mLeaderBoard[i] = newScore;
        newScore = oldScore;
        updated = true;
    }
    return updated;
}

bool Game::writeLeaderBoard()
{
    // trunc: clear the data file
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.trunc | fhand.out);
    if (!fhand.is_open())
    {
        return false;
    }
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        fhand.write(reinterpret_cast<char*>(&this->mLeaderBoard[i]), sizeof(this->mLeaderBoard[i]));;
    }
    fhand.close();
    return true;
}







