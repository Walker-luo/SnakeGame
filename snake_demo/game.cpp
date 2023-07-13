#include <string>
#include <iostream>
#include <cmath>

// For terminal delay
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm>

#include "game.h"


std::string  Time::gettime()const
{
    int mcall = clock();
    int msecond = (mcall-start)/1000;
    int mminute = msecond / 60;
    msecond %= 60;
    std::string s,m;
    s = std::to_string(msecond);
    m = std::to_string(mminute);
    if(s.length() == 1) s.insert(s.begin(),'0');
    if(m.length() == 1) m.insert(m.begin(),'0');
    m.append(":");
    m.append(s);
    return m;
}

void Time::zero()
{
    this->start = clock();
}

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
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
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


void Game::helpMenu()
{
    WINDOW *menu;
    int width = this->mGameBoardWidth * 0.7;
    int height = this->mGameBoardHeight * 0.7;
    int startX = this->mGameBoardWidth * 0.15;
    int startY = this->mGameBoardHeight * 0.15 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    mvwprintw(menu,1,1,"This is Help Menu");
    mvwprintw(menu,3,1,"Food:");
    mvwaddch(menu,3,7,this->mFoodSymbol);
    mvwprintw(menu,4,2,"When you eat one food ,you get one point");
    mvwprintw(menu,5,1,"Award:");
    mvwaddch(menu, 5,8, this->mAwardSymbol | COLOR_PAIR(2));
    mvwprintw(menu,6,2,"When you get one award ,you get two point");
    mvwprintw(menu,7,1,"Speed:");
    mvwprintw(menu,8,2,"Choose the same direction will speed up your snake");
    mvwprintw(menu,9,1,"Obstacle:");
    mvwaddch(menu,9,11,this->mObstacleSymbol
           | COLOR_PAIR(1) | A_BOLD | A_STANDOUT);
    mvwprintw(menu,10,2,"When you hit one obstacle ,your life decreases");
    mvwprintw(menu,11,1,"Lifefruit:");
    mvwaddch(menu,11,12,this->mLifeFruitSymbol | COLOR_PAIR(3) | A_BLINK | A_BOLD);
    mvwprintw(menu,12,2,"When you get one Lifefruit ,your life increases");
    mvwprintw(menu,14,1,"Click ESC to back to game");
    wrefresh(menu);
    int key;
    while(true){
        key = getch();
        if(key == 27||key == 10||key == ' ') return;
    }
    delwin(menu);
    wrefresh(mWindows[1]);
}

int Game::renderStartMenu()
{
    WINDOW *menu;
    int width = this->mGameBoardWidth * 0.7;
    int height = this->mGameBoardHeight * 0.7;
    int startX = this->mGameBoardWidth * 0.15;
    int startY = this->mGameBoardHeight * 0.15 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    int logoStartX = (width-43)/2;
    int logoStartY = height*0.1;
    std::vector<std::string> title = {  " #####   #     #     #     #   ###  #######",
                                         "#     #  ##    #    # #    #  #     #",
                                         "#        # #   #   #   #   # #      #",
                                         " #####   #  #  #  #     #  ##       #######",
                                         "      #  #   # #  #######  # #      #",
                                         "#     #  #    ##  #     #  #  ##    #",
                                         " #####   #     #  #     #  #    ##  #######   "};
    std::vector<std::string> menuItems = {"Start Game","Help","Quit"};
    int index = 0;
    int offset = height*0.7;
    int medium = width*0.5;

    mvwprintw(menu, 0 + offset, medium-8, "Select: ");
    for(int i = 0;i < 7;++i) mvwprintw(menu, 1 + logoStartY + i, logoStartX, title[i].c_str());
    mvwprintw(menu, 0 + offset, medium, menuItems[0].c_str());
    mvwprintw(menu, 1 + offset, medium, menuItems[1].c_str());
    mvwprintw(menu, 2 + offset, medium, menuItems[2].c_str());

    wrefresh(menu);

    int key;
    int choice = 1;
    int times = 1;
    while (true)
    {
        if(times%5 == 0)choice++;
        if(choice == 8) choice = 1;
        wattron(menu,COLOR_PAIR(choice));
        for(int i = 0;i < 7;++i) mvwprintw(menu, 1 + logoStartY + i, logoStartX, title[i].c_str());
        wattroff(menu,COLOR_PAIR(choice));
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, medium, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, medium, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, medium, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, medium, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 27:
                return 2;
        }

        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        times++;
    }
    delwin(menu);
    wrefresh(this->mWindows[1]);

    if(index == 1) this->helpMenu();
    return index;


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
    mvwprintw(this->mWindows[0], 4, 1, "Whether speed or not: ");
    if(!this->GameMode)
    {
        mvwprintw(this->mWindows[0], 2, 1, "This is MAUALE MODE.");
        mvwprintw(this->mWindows[0], 3, 1, "Game time : ");
    }
    else
    {
        mvwprintw(this->mWindows[0], 2, 1, "This is VS MODE");
        mvwprintw(this->mWindows[0], 3, 1, "One life equals three points and the higher wins.");
    }
    mvwprintw(this->mWindows[0], 4, 1, "Enjoy your game time!");


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

void Game::renderInstructionBoard_1() const
{
    mvwprintw(this->mWindows[2], 1, 1, "MANUAL");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Life");
    mvwprintw(this->mWindows[2], 11, 1, "Points");
    mvwprintw(this->mWindows[2], 14, 1, "Difficulty");

    wrefresh(this->mWindows[2]);
}

void Game::renderInstructionBoard_2() const
{
    mvwprintw(this->mWindows[2], 1, 1, "VS MODE");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Life1");
    mvwprintw(this->mWindows[2], 10, 1, "Point1");
    wattron(this->mWindows[2], COLOR_PAIR(6));
    mvwprintw(this->mWindows[2], 12, 1, "Life2");
    mvwprintw(this->mWindows[2], 14, 1, "Point2");
    wattroff(this->mWindows[2], COLOR_PAIR(6));
    mvwprintw(this->mWindows[2], 17, 1, "Difficulity");
    wrefresh(this->mWindows[2]);

}

bool Game::Pause()
{
    WINDOW * pauseMenu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    pauseMenu = newwin(height, width, startY, startX);
    box(pauseMenu, 0, 0);
    std::vector<std::string> menuItems = {"Continue","Help","Quit"};

    int index = 0;
    int offset = 4;
    mvwprintw(pauseMenu, 1, 1, "Game Paused");
    wattron(pauseMenu, A_STANDOUT);
    mvwprintw(pauseMenu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(pauseMenu, A_STANDOUT);
    mvwprintw(pauseMenu, 1 + offset, 1, menuItems[1].c_str());
    mvwprintw(pauseMenu, 2 + offset, 1, menuItems[2].c_str());


    wrefresh(pauseMenu);

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
                mvwprintw(pauseMenu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(pauseMenu, A_STANDOUT);
                mvwprintw(pauseMenu, index + offset, 1, menuItems[index].c_str());
                wattroff(pauseMenu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(pauseMenu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(pauseMenu, A_STANDOUT);
                mvwprintw(pauseMenu, index + offset, 1, menuItems[index].c_str());
                wattroff(pauseMenu, A_STANDOUT);
                break;
            }
            case 27:
                return true;
        }
        wrefresh(pauseMenu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    werase(pauseMenu);
    delwin(pauseMenu);

    if (index == 0)
    {
        return true;
    }
    if (index == 1)
    {
        helpMenu();
        return true;
    }
    if(index == 2)return false;

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
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 18- 2); i ++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 18 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 18 + (i + 1), 5, pointString.c_str());
    }
    wrefresh(this->mWindows[2]);
}

bool Game::renderRestartMenu_1() const
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
    int offset = height*0.5;
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->mPoints_1);
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

bool Game::renderRestartMenu_2() const
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
    int offset = height*0.5;

    mvwprintw(menu, 1, 1, "P1 Final Score:");
    std::string pointString1 = std::to_string(this->mPoints_1+this->mLife_1*3);
    mvwprintw(menu, 1, 17, pointString1.c_str());

    mvwprintw(menu, 2, 1, "P2 Final Score:");
    std::string pointString2 = std::to_string(this->mPoints_2+this->mLife_2*3);
    mvwprintw(menu, 2, 17, pointString2.c_str());

    if(this->mLife_1 != 0&&this->mLife_2 == 0)
    {
        mvwprintw(menu, 3, 1, "P1 won the game!");
    }
    else if(this->mLife_2 != 0&&this->mLife_1 == 0)
    {
        mvwprintw(menu, 3, 1, "P2 won the game!");
    }
    else
    {
        if((this->mPoints_1+this->mLife_1*3) < (this->mPoints_2+this->mLife_2*3)) mvwprintw(menu, 3, 1, "P2 won the game!");
        else if((this->mPoints_1+this->mLife_1*3) > (this->mPoints_2+this->mLife_2*3)) mvwprintw(menu, 3, 1, "P1 won the game!");
        else mvwprintw(menu, 3, 1, "A tie!");
    }

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

void Game::renderLife_1() const
{
	std::string Lifesting = std::to_string(this->mLife_1);
	mvwprintw(this->mWindows[2], 9, 1, Lifesting.c_str());
	wrefresh(this->mWindows[2]);
}

void Game::renderLife_2() const
{
	std::string Lifesting_1 = std::to_string(this->mLife_1);
	mvwprintw(this->mWindows[2], 9, 1, Lifesting_1.c_str());
	std::string Lifesting_2 = std::to_string(this->mLife_2);
	mvwprintw(this->mWindows[2], 13, 1, Lifesting_2.c_str());
	wrefresh(this->mWindows[2]);
}

void Game::renderPoints_2() const
{
	std::string pointString1 = std::to_string(this->mPoints_1);
	std::string pointString2 = std::to_string(this->mPoints_2);
	mvwprintw(this->mWindows[2], 11, 1, pointString1.c_str());
	mvwprintw(this->mWindows[2], 15, 1, pointString2.c_str());
	wrefresh(this->mWindows[2]);

}

void Game::renderPoints_1() const
{
	std::string pointString1 = std::to_string(this->mPoints_1);
	mvwprintw(this->mWindows[2], 12, 1, pointString1.c_str());
	wrefresh(this->mWindows[2]);

}

void Game::renderDifficulty_1() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2], 15, 1, difficultyString.c_str());
    wrefresh(this->mWindows[2]);
}


void Game::renderDifficulty_2() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2],18, 1, difficultyString.c_str());
    wrefresh(this->mWindows[2]);
}



bool Game::chooseGameMode() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.4;
    int startX = this->mGameBoardWidth * 0.3;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"double persons", "single person"};

    int index = 0;
    int offset = 3;
    mvwprintw(menu, 1, 1, "choose the game mode");
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

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
    wrefresh(mWindows[1]);

    if (index == 0) return true;
    else return false;

}

void Game::initializeGame()
{
    // allocate memory for a new snake
    this->mFood = SnakeBody(0,0); // avoid error happens when addObstacle
	this->mPtrSnake_1.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength, 0));
    if(this->GameMode) {
        this->mPtrSnake_2.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength, 3));
        this->mPoints_2 = 0;
        this->mLife_2 = 2;
    }
	this->mPoints_1 = 0;
	this->mLife_1 = 2;

	//double snakes do not have this function
	if(!this->GameMode) {
        this->numObstacles = 3;
        this->mObstacles.clear();
        this->addObstacle();
	}

	this->mAwardExist = this->createRandom();
	this->createLifeFruit();
	
        this->mPtrSnake_1->Awardsense(mAwardExist);
        this->mPtrSnake_1->senseFood(this->mFood);
        this->mPtrSnake_1->LifeFruitsense(this->mLifeFruitExist);

        if(this->GameMode) {
		this->mPtrSnake_2->senseFood(mFood);
        	this->mPtrSnake_2->Awardsense(mAwardExist);
        	this->mPtrSnake_2->LifeFruitsense(this->mLifeFruitExist);
	}

    if(this->mAwardExist) {
	    this->mPtrSnake_1->senseAward(this->mAward);
	    if(this->GameMode) this->mPtrSnake_2->senseAward(this->mAward);
    }
    
    if(this->mLifeFruitExist) {
           this->mPtrSnake_1->senseLifeFruit(this->mLifeFruit);
           if(this->GameMode) this->mPtrSnake_2->senseLifeFruit(this->mLifeFruit);
    }

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
        if(x < 1 || y < 1 || this->mPtrSnake_1->isPartOfSnake(x,y) || this->isPartOfObstacles(x,y)) continue;
        if(this->GameMode && (x < 1 || y < 1 || this->mPtrSnake_2->isPartOfSnake(x,y))) continue;
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
        if(x < 1 || y < 1 || this->mPtrSnake_1->isPartOfSnake(x,y) || this->isPartOfObstacles(x,y)) continue;
        if(this->GameMode && (x < 1 || y < 1 || this->mPtrSnake_2->isPartOfSnake(x,y))) continue;
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
        if(x < 1 || y < 1 || this->mPtrSnake_1->isPartOfSnake(x,y) || this->isPartOfObstacles(x,y)) continue;
        if(this->GameMode && (x < 1 || y < 1 || this->mPtrSnake_2->isPartOfSnake(x,y))) continue;
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


//only one Snake has this function
void Game::addObstacle()
{
    int x, y;
    SnakeBody obstacle;
    while(true) {
        x = (rand() % this->mGameBoardWidth) - 1;
        y = (rand() % this->mGameBoardHeight) - 1;
	if(x<1 || y<1 || this->mPtrSnake_1->isPartOfSnake(x,y)) continue;
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
	SnakeBody head = this->mPtrSnake_1->getHead();
	for(int i = 0; i < this->numObstacles; ++i){
		if(this->mObstacles[i] == head) {
			if(this->speed && this->mPoints_1) {
				--this->mPoints_1;
				this->mPtrSnake_1->deletTail();
				return false;
			}
			return true;
		}
	}

	return false;
}


void Game::renderSnake() const
{
    int snakeLength_1 = this->mPtrSnake_1->getLength();
    std::vector<SnakeBody>& snake_1 = this->mPtrSnake_1->getSnake();
    for (int i = 0; i < snakeLength_1; i ++)
    {
        mvwaddch(this->mWindows[1], snake_1[i].getY(), snake_1[i].getX(), this->mSnakeSymbol_1);
    }


    if(this->GameMode) {
        int snakeLength_2 = this->mPtrSnake_2->getLength();
        std::vector<SnakeBody>& snake_2 = this->mPtrSnake_2->getSnake();
        for(int i = 0; i < snakeLength_2; ++i) {
            mvwaddch(this->mWindows[1], snake_2[i].getY(), snake_2[i].getX(), this->mSnakeSymbol_2 | COLOR_PAIR(6));
        }
    }

    wrefresh(this->mWindows[1]);
}

void Game::OneControlSnake()
{
    int key;
    key = getch();
    switch(key)
    {
        case 'W':
        case 'w':
        case KEY_UP:
        {
            this->speed = this->mPtrSnake_1->changeDirection(Direction::Up);
            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
            this->speed = this->mPtrSnake_1->changeDirection(Direction::Down);
            break;
        }
        case 'A':
        case 'a':
        case KEY_LEFT:
        {
            this->speed = this->mPtrSnake_1->changeDirection(Direction::Left);
            break;
        }
        case 'D':
        case 'd':
        case KEY_RIGHT:
        {
            this->speed = this->mPtrSnake_1->changeDirection(Direction::Right);
            break;
        }
        case ' ':
        case 10:
        case 27:
        {
            if(!this->Pause()) this ->isEnd = 1;
            break;
        }
        default:
        {
            break;
        }
    }
}

void Game::TwoControlSnake()
{
    int key=getch();
    switch(key)
    {
        case 'W':
        case 'w':
        {
            this->mPtrSnake_1->changeDirection(Direction::Up);
            break;
        }

        case 'S':
        case 's':
        {
            this->mPtrSnake_1->changeDirection(Direction::Down);
            break;
        }

        case 'A':
        case 'a':
        {
            this->mPtrSnake_1->changeDirection(Direction::Left);
            break;
        }

        case 'D':
        case 'd':
        {
            this->mPtrSnake_1->changeDirection(Direction::Right);
            break;
        }

        case KEY_UP:
        {
            this->mPtrSnake_2->changeDirection(Direction::Up);
            break;
        }
        case KEY_DOWN:
        {
            this->mPtrSnake_2->changeDirection(Direction::Down);
            break;
        }
        case KEY_LEFT:
        {
            this->mPtrSnake_2->changeDirection(Direction::Left);
            break;
        }
        case KEY_RIGHT:
        {
            this->mPtrSnake_2->changeDirection(Direction::Right);
            break;
        }
        case ' ':
        case 10:
        case 27:
        {
            if(!this->Pause()) this ->isEnd = 1;
            break;
        }
        default:
        {
            break;
        }

    }
}


void Game::renderBoards_1() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard_1();
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        box(this->mWindows[i], 0, 0);
	wborder(this->mWindows[i], '|', '|', '-', '-', '+', '+', '+', '+');
        wrefresh(this->mWindows[i]);
    }
    this->renderLeaderBoard();
}

void Game::renderBoards_2() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard_2();
    for (int i = 0; i <this->mWindows.size(); i ++)
    {
        box(this->mWindows[i], 0, 0);
	wborder(this->mWindows[i], '|', '|', '-', '-', '+', '+', '+', '+');
        wrefresh(this->mWindows[i]);
    }
}

//change the difficulity(the bigger Points decide)
void Game::adjustDelay()
{
    int mPoints;
    if(this->GameMode) mPoints = (mPoints_1 > mPoints_2)?mPoints_1: mPoints_2;
    else mPoints = mPoints_1;

    this->mDifficulty = mPoints / 5;
    if (mPoints % 5 == 0)
    {
        this->mDelay = this->mBaseDelay * pow(0.75, this->mDifficulty);
        this->speedDelay = this->mDelay/2;
        if(!this->GameMode) {
            this->numObstacles += 2;
            this->addObstacle();
        }
    }
}

void Game::OneRunGame()
{
    bool moveSuccess;
    int key;
    this->time.zero();
    while (true){
        //render the current time
        std::string t = this->time.gettime();
        mvwprintw(this->mWindows[0],3,14,t.c_str());

        this->OneControlSnake();
        if(this->isEnd == 1) return;
        this->OneControlSnake();
        werase(this->mWindows[1]);
        box(this->mWindows[1], 0, 0);
        wborder(this->mWindows[1], '|', '|', '-', '-', '+', '+', '+', '+');

        int eaten = this->mPtrSnake_1->moveFoward();

        this->renderSnake();
        this->renderSpeed();
        if(eaten) {
            if(this->mPtrSnake_1->WheAddLife()) ++this->mLife_1;
            this->mPoints_1 += eaten;
            this->mAwardExist = this->createRandom();
            this->createLifeFruit();
            this->mPtrSnake_1->Awardsense(mAwardExist);
            this->mPtrSnake_1->senseFood(this->mFood);
            this->mPtrSnake_1->LifeFruitsense(this->mLifeFruitExist);

            if(this->mAwardExist)
                this->mPtrSnake_1->senseAward(this->mAward);

            if(this->mLifeFruitExist)
                this->mPtrSnake_1->senseLifeFruit(this->mLifeFruit);

            this->adjustDelay();
        }


        if(this->mPtrSnake_1->checkCollision()) --this->mLife_1;
        if(this->checkHitObstacle()) --this->mLife_1;
        if(!this->mLife_1) {
		werase(this->mWindows[1]);
	       	break;
	}

        this->renderObstacle();
        this->renderRandom();

        this->renderLife_1();
        this->renderPoints_1();
        this->renderDifficulty_1();

        if(speed) std::this_thread::sleep_for(std::chrono::milliseconds(this->speedDelay));
        else std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));

        refresh();
    }
}

bool Game::hitEachOther() const
{
    SnakeBody head_1, head_2;
    head_1 = this->mPtrSnake_1->getHead();
    head_2 = this->mPtrSnake_2->getHead();
    if(this->mPtrSnake_1->isPartOfSnake(head_2.getX(), head_2.getY())) return true;
    if(this->mPtrSnake_2->isPartOfSnake(head_1.getX(), head_1.getY())) return true;
    return false;
}

void Game::TwoRunGame()
{
    int key;
    while (true){
        //render the current time

        this->TwoControlSnake();
        if(this->isEnd == 1) return;
        werase(this->mWindows[1]);
        box(this->mWindows[1], 0, 0);
        wborder(this->mWindows[1], '|', '|', '-', '-', '+', '+', '+', '+');

        int eaten_1 = this->mPtrSnake_1->moveFoward();
        int eaten_2 = this->mPtrSnake_2->moveFoward();

        this->renderSnake();
        if(eaten_1 || eaten_2) {
            if(this->mPtrSnake_1->WheAddLife()) ++this->mLife_1;
            if(this->mPtrSnake_2->WheAddLife()) ++this->mLife_2;
            this->mPoints_1 += eaten_1;
            this->mPoints_2 += eaten_2;
            this->mAwardExist = this->createRandom();
            this->createLifeFruit();
            this->mPtrSnake_1->Awardsense(mAwardExist);
            this->mPtrSnake_2->Awardsense(mAwardExist);
            this->mPtrSnake_1->senseFood(this->mFood);
            this->mPtrSnake_2->senseFood(this->mFood);
            this->mPtrSnake_1->LifeFruitsense(this->mLifeFruitExist);
            this->mPtrSnake_2->LifeFruitsense(this->mLifeFruitExist);

            if(this->mAwardExist) {
                this->mPtrSnake_1->senseAward(this->mAward);
                this->mPtrSnake_2->senseAward(this->mAward);
            }

            if(this->mLifeFruitExist){
                this->mPtrSnake_1->senseLifeFruit(this->mLifeFruit);
                this->mPtrSnake_2->senseLifeFruit(this->mLifeFruit);
            }

            this->adjustDelay();
        }


        if(this->mPtrSnake_1->checkCollision()) --this->mLife_1;
        if(this->mPtrSnake_2->checkCollision()) --this->mLife_2;
        if(this->hitEachOther()) {
            --this->mLife_1;
            --this->mLife_2;
        }

        if(!this->mLife_1 || !this->mLife_2){
		break;
	}

        this->renderRandom();

        this->renderLife_2();
        this->renderPoints_2();
        this->renderDifficulty_2();

        std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));

        refresh();
    }

}

void Game::startGame()
{
    refresh();
    bool choice;
    int game_mode = 1;
    this->readLeaderBoard();
    this->renderBoards_1();
    while (true)
    {
        while(game_mode == 1) game_mode = this->renderStartMenu();
        if(game_mode == 2) break;

        if(game_mode == 0) this->GameMode = this->chooseGameMode();

        if(!this->GameMode)this->readLeaderBoard();
        (this->GameMode)? this->renderBoards_2() : this->renderBoards_1();
        werase(this->mWindows[1]);
	wrefresh(this->mWindows[1]);
        this->initializeGame();
        (this->GameMode)? this->TwoRunGame() : this->OneRunGame();

        if(!this->GameMode) {
            this->updateLeaderBoard();
            this->writeLeaderBoard();
	}
        (this->GameMode)? choice = this->renderRestartMenu_2() : choice = this->renderRestartMenu_1();
        if (choice == false)
        {
            break;
        }
        else this->isEnd = 0;

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
    int newScore = this->mPoints_1;
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        if (this->mLeaderBoard[i] >= this->mPoints_1)
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








