#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <ctime>
#include <string>
#include <iomanip>
#include <vector>

using namespace sf;
using namespace std;

//Prepare(function prototypes & other)
void CellOpen(int, int);
void CellAction(int, int, int);

//public game variables

// Const
int fieldWidth = 30;
int fieldHeight = 16;

// Objects
vector<vector<RectangleShape>> cell;
RectangleShape restartButton(Vector2f(24, 24));
vector<RectangleShape> flagsCounter(3);
vector<RectangleShape> timerCounter(3);
RectangleShape backButton(Vector2f(24, 24));

RenderWindow results(VideoMode(50, 70), "Results");
RenderWindow gameWindow;

VideoMode gameWindowSize;

Texture sprites;
Font font;


//Game
bool started = false;
vector<vector<pair<bool, int>>> cellType; // 0 - empty, 1 - mine, 2 - mine detected(with number),  3 - with flag(empty), 4 - with flag(mine), 5 - question empty, 6 - question with mine, 7 - question with number
int baseFlagAmmount = 10;
int flags = baseFlagAmmount;
int mine = 0;
int conditions = 0;
bool win = false;
bool lose = false;
bool neutral = false;
int mineDestroyPlayer[2];
Clock gameTime;
Time gameTimer;
int click = 0;
float effencyCounter = 0;
float effency = 0.f;


bool CheckPosition(RectangleShape square, Mouse mouse, RenderWindow &localWindow)
{
    if ((mouse.getPosition(localWindow).x <= -square.getOrigin().x + square.getPosition().x + square.getSize().x &&
        mouse.getPosition(localWindow).x >= -square.getOrigin().x + square.getPosition().x)
        &&
        (mouse.getPosition(localWindow).y >= -square.getOrigin().y + square.getPosition().y &&
         mouse.getPosition(localWindow).y <= -square.getOrigin().y + square.getPosition().y + square.getSize().y))
        return true;
    return false;
}

void Generating() 
{
    int yCord, xCord, i;
    int generateMine = 1 + rand()% 100;
    flags = baseFlagAmmount;
    vector<int> positionOffset(2, 0);

    //Creating or clearing field
    cellType.clear();
    cellType.resize(cell.size(), vector<pair<bool, int>>(cell[0].size(), pair<bool, int>(false, 0)));

    //creating mines
    while (mine < flags)
    {
        for (yCord = 0; yCord < cell.size(); yCord++)
            for (xCord = 0; xCord < cell[yCord].size(); xCord++)
            {
                if (generateMine >= 90 && mine < flags && cellType[yCord][xCord].second != 1)
                {
                    cellType[yCord][xCord].second = 1;
                    mine++;
                }
                generateMine = 1 + rand() % 100;
            }
    }

    //set cells with numbers
    for (yCord = 0; yCord < cell.size(); yCord++)
        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
            if (cellType[yCord][xCord].second != 1)
            {
                if (yCord > 0)
                {
                    if (xCord > 0)
                        if (cellType[yCord - 1][xCord - 1].second == 1)
                            cellType[yCord][xCord].second = 2;
                    if (cellType[yCord - 1][xCord].second == 1)
                        cellType[yCord][xCord].second = 2;
                    if (xCord + 1 != cell[yCord].size())
                        if (cellType[yCord - 1][xCord + 1].second == 1)
                            cellType[yCord][xCord].second = 2;
                }
                if (yCord+1 != cell.size())
                {
                    if (xCord > 0)
                        if (cellType[yCord + 1][xCord - 1].second == 1)
                            cellType[yCord][xCord].second = 2;
                    if (cellType[yCord + 1][xCord].second == 1)
                        cellType[yCord][xCord].second = 2;
                    if (xCord + 1 != cell[yCord].size())
                        if (cellType[yCord + 1][xCord + 1].second == 1)
                            cellType[yCord][xCord].second = 2;
                }
                if (xCord > 0)
                    if (cellType[yCord][xCord - 1].second == 1)
                        cellType[yCord][xCord].second = 2;
                if (xCord+1 != cell[yCord].size())
                    if (cellType[yCord][xCord + 1].second == 1)
                        cellType[yCord][xCord].second = 2;
            }

    // create/recreate correct objects
    //create cells
    for (yCord = 0; yCord < cell.size(); yCord++)
        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
        {
            cell[yCord][xCord].setSize(Vector2f(24, 24));
            cell[yCord][xCord].setTexture(&sprites);
            cell[yCord][xCord].setTextureRect(IntRect(0, 49, 16, 16));
            cell[yCord][xCord].setOrigin(-24 * xCord, -76 - 24 * yCord);
            cell[yCord][xCord].setPosition(0, 0);
        }
    //create flags & time counters
    for (i = 0; i < 3; i++)
    {
        flagsCounter[i].setSize(Vector2f(13, 23));
        flagsCounter[i].setTexture(&sprites);
        flagsCounter[i].setTextureRect(IntRect(0, 0, 13, 23));
        flagsCounter[i].setPosition(25 + i * 13, 24);
        cout << "Flag counter[" << i << "] successfully created" << endl;

        timerCounter[i].setSize(Vector2f(13, 23));
        timerCounter[i].setTexture(&sprites);
        timerCounter[i].setTextureRect(IntRect(0, 0, 13, 23));
        timerCounter[i].setPosition(fieldWidth * 24 - 64 + i * 13, 24);
        cout << "Timer counter[" << i << "] successfully created" << endl;
    }
    //create restart button
    restartButton.setTexture(&sprites);
    restartButton.setTextureRect(IntRect(0, 24, 24, 24));
    restartButton.setPosition(fieldWidth *12 - 12, 24);

    backButton.setTexture(&sprites);
    backButton.setTextureRect(IntRect(136, 49, 24, 24));
    backButton.setPosition(0, 0);

    cout << "Generation result: {\n";
    for (yCord = 0; yCord < cell.size(); yCord++)
    {
        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
            cout << cellType[yCord][xCord].second;
        cout << endl;
    }
    cout << "}\n";
}
void Restart()
{
    results.setVisible(false);
    mine = 0;
    click = 0;
    win = false;
    lose = false;
    mineDestroyPlayer[0] = 0;
    mineDestroyPlayer[1] = 0;
    neutral = false;
    conditions = 0;

    Generating();
    gameTime.restart();
    //cout << "Restart succes" << endl;
}

void CellOpen(int cellY, int cellX) 
{
    int mineCounter = 0;
    switch (cellType[cellY][cellX].second)
    {
    case 0:
        cell[cellY][cellX].setTextureRect(IntRect(17, 49, 16, 16));
        cellType[cellY][cellX].first = true;
        CellAction(cellY, cellX, 1);
        break;
    case 1:
        cout << "This is mine cell" << endl;
        break;
    case 2:
        if (cellY > 0)
        {
            if (cellX > 0)
                if (cellType[cellY - 1][cellX - 1].second == 1 ||cellType[cellY - 1][cellX - 1].second == 4 || cellType[cellY-1][cellX-1].second == 6)
                    mineCounter++;
            if (cellType[cellY - 1][cellX].second == 1 || cellType[cellY - 1][cellX].second == 4 || cellType[cellY-1][cellX].second == 6)
                mineCounter++;
            if (cellX + 1 != cell[cellY].size())
                if (cellType[cellY - 1][cellX + 1].second == 1 || cellType[cellY - 1][cellX + 1].second == 4 || cellType[cellY-1][cellX+1].second == 6)
                    mineCounter++;
        }
        if (cellY + 1 != cell.size())
        {
            if (cellX > 0)
                if (cellType[cellY + 1][cellX - 1].second == 1 || cellType[cellY + 1][cellX - 1].second == 4 || cellType[cellY+1][cellX-1].second == 6)
                    mineCounter++;
            if (cellType[cellY + 1][cellX].second == 1 || cellType[cellY + 1][cellX].second == 4 || cellType[cellY+1][cellX].second == 6)
                mineCounter++;
            if (cellX + 1 != cell[cellY].size())
                if (cellType[cellY+1][cellX+1].second == 1 || cellType[cellY + 1][cellX + 1].second == 4 || cellType[cellY+1][cellX+1].second == 6)
                    mineCounter++;
        }
        if (cellX > 0)
            if (cellType[cellY][cellX - 1].second == 1 || cellType[cellY][cellX - 1].second == 4 || cellType[cellY][cellX-1].second == 6)
                mineCounter++;
        if (cellX + 1 != cell[cellY].size())
            if (cellType[cellY][cellX + 1].second == 1 || cellType[cellY][cellX + 1].second == 4 || cellType[cellY][cellX+1].second == 6)
                mineCounter++;
        switch (mineCounter)
        {
        case 1:
            cell[cellY][cellX].setTextureRect(IntRect(0, 66, 16, 16));
            break;
        case 2:
            cell[cellY][cellX].setTextureRect(IntRect(17, 66, 16, 16));
            break;
        case 3:
            cell[cellY][cellX].setTextureRect(IntRect(34, 66, 16, 16));
            break;
        case 4:
            cell[cellY][cellX].setTextureRect(IntRect(51, 66, 16, 16));
            break;
        case 5:
            cell[cellY][cellX].setTextureRect(IntRect(68, 66, 16, 16));
            break;
        case 6:
            cell[cellY][cellX].setTextureRect(IntRect(85, 66, 16, 16));
            break;
        case 7:
            cell[cellY][cellX].setTextureRect(IntRect(102, 66, 16, 16));
            break;
        case 8:
            cell[cellY][cellX].setTextureRect(IntRect(119, 66, 16, 16));
            break;
        default:
            cout << "error" << endl;
            break;
        }
        cellType[cellY][cellX].first = true;
        break;
    }
 
}
void CellAction(int cellY, int cellX, int actionType) 
{
    switch (actionType)
    {
    case 1: // open cell
        switch (cellType[cellY][cellX].second)
        {
        case 0:
            if (cellY > 0)
            {
                if (cellX > 0 && !cellType[cellY-1][cellX-1].first)
                    CellOpen(cellY - 1, cellX - 1);
                if(!cellType[cellY - 1][cellX].first)
                    CellOpen(cellY - 1, cellX);
                if (cellX + 1 != cell[cellY].size() && !cellType[cellY - 1][cellX + 1].first)
                    CellOpen(cellY - 1, cellX + 1);
            }
            if (cellY + 1 < cell.size())
            {
                if (cellX > 0 && !cellType[cellY + 1][cellX - 1].first)
                {
                    CellOpen(cellY + 1, cellX - 1);
                }
                if(!cellType[cellY + 1][cellX].first)
                CellOpen(cellY + 1, cellX);
                if (cellX + 1 != cell[cellY].size() && !cellType[cellY + 1][cellX + 1].first)
                    CellOpen(cellY + 1, cellX + 1);
            }
            if(cellX > 0 && !cellType[cellY][cellX - 1].first)
                CellOpen(cellY, cellX-1);
            if(cellX + 1 != cell[cellY].size() && !cellType[cellY][cellX + 1].first)
                CellOpen(cellY, cellX+1);
            if(!cellType[cellY][cellX].first)
                CellOpen(cellY, cellX);

            break;
        case 1:
            mineDestroyPlayer[0] = cellX;
            mineDestroyPlayer[1] = cellY;
            lose = true;
            break;
        case 2:
            CellOpen(cellY, cellX);
            break;
        default:
            cout << "This cell type not maybeen to open" << endl;
            break;
        }

        break;
    case 2: // set flag
        cout << "cell[" << cellY << "][" << cellX << "] set Flag (texture)" << endl;
        if (cellType[cellY][cellX].second == 1)
        {
            cellType[cellY][cellX].first = true;
            cellType[cellY][cellX].second = 4;
            cell[cellY][cellX].setTextureRect(IntRect(34, 49, 16, 16));
        }
        else
        {
            cellType[cellY][cellX].first = true;
            cellType[cellY][cellX].second = 3;
            cell[cellY][cellX].setTextureRect(IntRect(34, 49, 16, 16));
        }
        flags--;

        break;
    case 3: //set question
        cell[cellY][cellX].setTextureRect(IntRect(51, 49, 16, 16));

        switch (cellType[cellY][cellX].second)
        {
        case 0:
            cellType[cellY][cellX].second = 5;
            break;
        case 1:
            cellType[cellY][cellX].second = 6;
            break;
        case 2:
            cellType[cellY][cellX].second = 7;
            break;
        }
        break;
    case 4:
        cell[cellY][cellX].setTextureRect(IntRect(0, 49, 16, 16));

        switch (cellType[cellY][cellX].second)
        {
        case 5:
            cellType[cellY][cellX].second = 0;
            break;
        case 6:
            cellType[cellY][cellX].second = 1;
            break;
        case 7:
            cellType[cellY][cellX].second = 2;
            break;
        }
        break;
    }
}
void WinLoseConditions()
{
    int yCord, xCord;
    int flagError = 0;
    int closedCells = fieldHeight*fieldWidth;
    for (yCord = 0; yCord < cell.size(); yCord++)
        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
            if (cellType[yCord][xCord].first == true || cellType[yCord][xCord].second == 1 || cellType[yCord][xCord].second == 6)
                closedCells--;
    if (closedCells == 0)
    {
        for (yCord = 0; yCord < cell.size(); yCord++)
            for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                if (cellType[yCord][xCord].second == 3)
                {
                    neutral = true;
                    return ;
                }

           win = true;
    }
}

void TimerAndFlags()
{
    int localTime = gameTimer.asSeconds();
    switch (localTime % 10)
    {
    case 0:
        timerCounter[2].setTextureRect(IntRect(126, 0, 13, 23));
        break;
    case 1:
        timerCounter[2].setTextureRect(IntRect(0, 0, 13, 23));
        break;
    case 2:
        timerCounter[2].setTextureRect(IntRect(14, 0, 13, 23));
        break;
    case 3:
        timerCounter[2].setTextureRect(IntRect(28, 0, 13, 23));
        break;
    case 4:
        timerCounter[2].setTextureRect(IntRect(42, 0, 13, 23));
        break;
    case 5:
        timerCounter[2].setTextureRect(IntRect(56, 0, 13, 23));
        break;
    case 6:
        timerCounter[2].setTextureRect(IntRect(70, 0, 13, 23));
        break;
    case 7:
        timerCounter[2].setTextureRect(IntRect(84, 0, 13, 23));
        break;
    case 8:
        timerCounter[2].setTextureRect(IntRect(98, 0, 13, 23));
        break;
    case 9:
        timerCounter[2].setTextureRect(IntRect(112, 0, 13, 23));
        break;
    }
    switch ((localTime % 100) / 10)
    {
    case 0:
        timerCounter[1].setTextureRect(IntRect(126, 0, 13, 23));
        break;
    case 1:
        timerCounter[1].setTextureRect(IntRect(0, 0, 13, 23));
        break;
    case 2:
        timerCounter[1].setTextureRect(IntRect(14, 0, 13, 23));
        break;
    case 3:
        timerCounter[1].setTextureRect(IntRect(28, 0, 13, 23));
        break;
    case 4:
        timerCounter[1].setTextureRect(IntRect(42, 0, 13, 23));
        break;
    case 5:
        timerCounter[1].setTextureRect(IntRect(55, 0, 13, 23));
        break;
    case 6:
        timerCounter[1].setTextureRect(IntRect(69, 0, 13, 23));
        break;
    case 7:
        timerCounter[1].setTextureRect(IntRect(83, 0, 13, 23));
        break;
    case 8:
        timerCounter[1].setTextureRect(IntRect(98, 0, 13, 23));
        break;
    case 9:
        timerCounter[1].setTextureRect(IntRect(112, 0, 13, 23));
        break;
    }
    switch ((localTime % 1000) / 100)
    {
    case 0:
        timerCounter[0].setTextureRect(IntRect(126, 0, 13, 23));
        break;
    case 1:
        timerCounter[0].setTextureRect(IntRect(0, 0, 13, 23));
        break;
    case 2:
        timerCounter[0].setTextureRect(IntRect(14, 0, 13, 23));
        break;
    case 3:
        timerCounter[0].setTextureRect(IntRect(28, 0, 13, 23));
        break;
    case 4:
        timerCounter[0].setTextureRect(IntRect(42, 0, 13, 23));
        break;
    case 5:
        timerCounter[0].setTextureRect(IntRect(55, 0, 13, 23));
        break;
    case 6:
        timerCounter[0].setTextureRect(IntRect(69, 0, 13, 23));
        break;
    case 7:
        timerCounter[0].setTextureRect(IntRect(83, 0, 13, 23));
        break;
    case 8:
        timerCounter[0].setTextureRect(IntRect(98, 0, 13, 23));
        break;
    case 9:
        timerCounter[0].setTextureRect(IntRect(112, 0, 13, 23));
        break;
    }

    switch (flags % 10)
    {
    case 0:
        flagsCounter[2].setTextureRect(IntRect(126, 0, 13, 23));
        break;
    case 1:
        flagsCounter[2].setTextureRect(IntRect(0, 0, 13, 23));
        break;
    case 2:
        flagsCounter[2].setTextureRect(IntRect(14, 0, 13, 23));
        break;
    case 3:
        flagsCounter[2].setTextureRect(IntRect(28, 0, 13, 23));
        break;
    case 4:
        flagsCounter[2].setTextureRect(IntRect(42, 0, 13, 23));
        break;
    case 5:
        flagsCounter[2].setTextureRect(IntRect(56, 0, 13, 23));
        break;
    case 6:
        flagsCounter[2].setTextureRect(IntRect(70, 0, 13, 23));
        break;
    case 7:
        flagsCounter[2].setTextureRect(IntRect(84, 0, 13, 23));
        break;
    case 8:
        flagsCounter[2].setTextureRect(IntRect(98, 0, 13, 23));
        break;
    case 9:
        flagsCounter[2].setTextureRect(IntRect(112, 0, 13, 23));
        break;
    }
    switch ((flags % 100) / 10)
    {
    case 0:
        flagsCounter[1].setTextureRect(IntRect(126, 0, 13, 23));
        break;
    case 1:
        flagsCounter[1].setTextureRect(IntRect(0, 0, 13, 23));
        break;
    case 2:
        flagsCounter[1].setTextureRect(IntRect(14, 0, 13, 23));
        break;
    case 3:
        flagsCounter[1].setTextureRect(IntRect(28, 0, 13, 23));
        break;
    case 4:
        flagsCounter[1].setTextureRect(IntRect(42, 0, 13, 23));
        break;
    case 5:
        flagsCounter[1].setTextureRect(IntRect(55, 0, 13, 23));
        break;
    case 6:
        flagsCounter[1].setTextureRect(IntRect(69, 0, 13, 23));
        break;
    case 7:
        flagsCounter[1].setTextureRect(IntRect(83, 0, 13, 23));
        break;
    case 8:
        flagsCounter[1].setTextureRect(IntRect(98, 0, 13, 23));
        break;
    case 9:
        flagsCounter[1].setTextureRect(IntRect(112, 0, 13, 23));
        break;
    }
    switch ((flags % 1000) / 100)
    {
    case 0:
        flagsCounter[0].setTextureRect(IntRect(126, 0, 13, 23));
        break;
    case 1:
        flagsCounter[0].setTextureRect(IntRect(0, 0, 13, 23));
        break;
    case 2:
        flagsCounter[0].setTextureRect(IntRect(14, 0, 13, 23));
        break;
    case 3:
        flagsCounter[0].setTextureRect(IntRect(28, 0, 13, 23));
        break;
    case 4:
        flagsCounter[0].setTextureRect(IntRect(42, 0, 13, 23));
        break;
    case 5:
        flagsCounter[0].setTextureRect(IntRect(55, 0, 13, 23));
        break;
    case 6:
        flagsCounter[0].setTextureRect(IntRect(69, 0, 13, 23));
        break;
    case 7:
        flagsCounter[0].setTextureRect(IntRect(83, 0, 13, 23));
        break;
    case 8:
        flagsCounter[0].setTextureRect(IntRect(98, 0, 13, 23));
        break;
    case 9:
        flagsCounter[0].setTextureRect(IntRect(112, 0, 13, 23));
        break;
    }
}


int main()
{
    // Counters & positions buffers
    results.setVisible(false);
    gameWindow.setVisible(false);
    srand(time(0));
    
    int yCord, xCord, i;

    //set Textures & fonts
    Image icon;
    icon.loadFromFile("icon.png");
    font.loadFromFile("Textures\\Font\\Arial.ttf");
    sprites.loadFromFile("Textures\\SpriteFile.png");

    // controller
    Mouse mouse;
    // player results
    Text resultsText;

    //dificulty LVL change window
    bool playerSettings = false;
    RectangleShape dificultySelectButton[4];
    RenderWindow dificultySelectWindow(VideoMode(384, 200), "MineSweeper");
    dificultySelectWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    Text difficultyText("Select difficulty level", font);
    Text dificulty9x9("9x9",font,28);
    Text dificulty16x16("16x16", font, 28);
    Text dificulty16x30("16x30", font, 28);
    
    Text dificultyPlayer("Set my parametrs...", font, 25);
    RectangleShape playerHeightButton(Vector2f(180, 24));
    playerHeightButton.setFillColor(Color::White);
    playerHeightButton.setPosition(210, 8);
    RectangleShape playerWidthButton(Vector2f(180, 24));
    playerWidthButton.setFillColor(Color::White);
    playerWidthButton.setPosition(210, 33);
    RectangleShape playerFlagsButton(Vector2f(180, 24));
    playerFlagsButton.setFillColor(Color::White);
    playerFlagsButton.setPosition(210, 58);
    RectangleShape applyPlayerParametrs(Vector2f(98, 48));
    applyPlayerParametrs.setTexture(&sprites);
    applyPlayerParametrs.setTextureRect(IntRect(125, 24, 24, 24));
    applyPlayerParametrs.setPosition(dificultySelectWindow.getSize().x - 98, dificultySelectWindow.getSize().y - 48);

    Text playerHeightText("Enter field Height:", font, 20);
    playerHeightText.setOrigin(0, -9);
    playerHeightText.setFillColor(Color::Black);
    Text playerWidthText("Enter field Width:", font, 20);
    playerWidthText.setOrigin(0, -33);
    playerWidthText.setFillColor(Color::Black);
    Text playerFlagsText("Enter mines ammount:", font, 20);
    playerFlagsText.setOrigin(0, -57);
    playerFlagsText.setFillColor(Color::Black);

    Text playerHeightTextMask("", font, 28);
    playerHeightTextMask.setOrigin(-playerHeightButton.getPosition().x, -playerHeightButton.getPosition().y+6);
    playerHeightTextMask.setFillColor(Color::Black);
    Text playerWidthTextMask("", font, 28);
    playerWidthTextMask.setOrigin(-playerWidthButton.getPosition().x, -playerWidthButton.getPosition().y+6);
    playerWidthTextMask.setFillColor(Color::Black);
    Text playerFlagsTextMask("", font, 28);
    playerFlagsTextMask.setOrigin(-playerFlagsButton.getPosition().x, -playerFlagsButton.getPosition().y+6);
    playerFlagsTextMask.setFillColor(Color::Black);

    Text applyPlayerParametrsText("Apply", font, 28);
    applyPlayerParametrsText.setOrigin(-applyPlayerParametrs.getPosition().x -9, -applyPlayerParametrs.getPosition().y-2);
    applyPlayerParametrsText.setFillColor(Color::Black);

    string playerWidthAmmount = "", playerHeightAmmount = "", playerFlagsAmmount = "";
    int activeButton = 0;
    RectangleShape playerSettingsBackButton(Vector2f(24, 24));
    playerSettingsBackButton.setTexture(&sprites);
    playerSettingsBackButton.setTextureRect(IntRect(136, 49, 24, 24));
    playerSettingsBackButton.setPosition(0, dificultySelectWindow.getSize().y - 24);

    for(i = 0; i < 3; i++)
    {
        dificultySelectButton[i].setSize(Vector2f(96, 48));
        dificultySelectButton[i].setTexture(&sprites);
        dificultySelectButton[i].setTextureRect(IntRect(125, 24, 24, 24));
        dificultySelectButton[i].setPosition(24 + i*120, 100);
    }
    dificultySelectButton[3].setSize(Vector2f(272, 48));
    dificultySelectButton[3].setTexture(&sprites);
    dificultySelectButton[3].setTextureRect(IntRect(125, 24, 24, 24));
    dificultySelectButton[3].setPosition(dificultySelectWindow.getSize().x/2-144, 150);

    dificulty9x9.setOrigin(-dificultySelectButton[0].getPosition().x - 26, -dificultySelectButton[0].getPosition().y - 5);
    dificulty16x16.setOrigin(-dificultySelectButton[1].getPosition().x - 8, -dificultySelectButton[1].getPosition().y - 5);
    dificulty16x30.setOrigin(-dificultySelectButton[2].getPosition().x - 9, -dificultySelectButton[2].getPosition().y - 5);
    dificultyPlayer.setOrigin(-dificultySelectButton[3].getPosition().x - 23, -dificultySelectButton[3].getPosition().y - 9);


    while (gameWindow.isOpen() || dificultySelectWindow.isOpen())
    {

        while (dificultySelectWindow.isOpen())
        {
            
            Event event;
            while (dificultySelectWindow.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    results.close();
                    dificultySelectWindow.close();
                }
                if (playerSettings)
                {
                    switch(activeButton)
                    {
                    case 1:
                        if (event.type == Event::TextEntered)
                        {
                            if (event.text.unicode == 8)
                            {
                                if (!playerWidthAmmount.empty())
                                    playerWidthAmmount.pop_back();
                            }
                            else if (event.text.unicode >= 48 && event.text.unicode <= 57)
                                playerWidthAmmount += char(event.text.unicode);
                        }
                        break;
                    case 2:
                        if (event.type == Event::TextEntered)
                        {
                            if (event.text.unicode == 8)
                            {
                                if (!playerHeightAmmount.empty())
                                    playerHeightAmmount.pop_back();
                            }
                            else if (event.text.unicode >= 48 && event.text.unicode <= 57)
                                playerHeightAmmount += char(event.text.unicode);
                        }
                        break;
                    case 3:
                        if (event.type == Event::TextEntered)
                        {
                            if (event.text.unicode == 8)
                            {
                                if (!playerFlagsAmmount.empty())
                                    playerFlagsAmmount.pop_back();
                            }
                            else if (event.text.unicode >= 48 && event.text.unicode <= 57)
                                playerFlagsAmmount += char(event.text.unicode);
                        }
                        break;
                    }
                }
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
                {
                    cout << "Left detected" << endl;
                    if (!playerSettings)
                    {
                        for (i = 0; i < 4; i++)
                            if (CheckPosition(dificultySelectButton[i], mouse, dificultySelectWindow))
                            {
                                cout << "Start..." << endl;
                                switch (i)
                                {
                                case 0:
                                    baseFlagAmmount = 10;
                                    fieldHeight = 9;
                                    fieldWidth = 9;
                                    break;
                                case 1:
                                    baseFlagAmmount = 40;
                                    fieldHeight = 16;
                                    fieldWidth = 16;
                                    break;
                                case 2:
                                    baseFlagAmmount = 99;
                                    fieldHeight = 16;
                                    fieldWidth = 30;
                                    break;
                                case 3:
                                    playerSettings = true;
                                    break;
                                default:
                                    cout << "Error" << endl;
                                    break;
                                }
                                if (!playerSettings)
                                {
                                    cell.resize(fieldHeight, vector<RectangleShape>(fieldWidth));
                                    gameWindowSize = VideoMode(fieldWidth * 24, fieldHeight * 24 + 76);
                                    Restart();
                                    gameWindow.create(gameWindowSize, "Minesweeper");
                                    gameWindow.setIcon(32, 32, icon.getPixelsPtr());
                                    dificultySelectWindow.close();
                                    started = true;
                                    break;
                                }
                            }
                    }
                    else if (playerSettings)
                    {
                        if (CheckPosition(playerHeightButton, mouse, dificultySelectWindow))
                            activeButton = 2;
                        else if (CheckPosition(playerWidthButton, mouse, dificultySelectWindow))
                            activeButton = 1;
                        else if (CheckPosition(playerFlagsButton, mouse, dificultySelectWindow))
                            activeButton = 3;
                        else if (CheckPosition(applyPlayerParametrs, mouse, dificultySelectWindow))
                        {
                            fieldHeight = stoi(playerHeightAmmount);
                            fieldWidth = stoi(playerWidthAmmount);
                            baseFlagAmmount = stoi(playerFlagsAmmount);
                            cell.resize(fieldHeight, vector<RectangleShape>(fieldWidth));
                            gameWindowSize = VideoMode(fieldWidth * 24, fieldHeight * 24 + 76);
                            Restart();
                            gameWindow.create(gameWindowSize, "Minesweeper");
                            gameWindow.setIcon(32, 32, icon.getPixelsPtr());
                            dificultySelectWindow.close();
                            started = true;
                            playerSettings = false;
                            playerHeightAmmount = "";
                            playerWidthAmmount = "";
                            playerFlagsAmmount = "";
                        }
                        else if (CheckPosition(playerSettingsBackButton, mouse, dificultySelectWindow))
                        {
                            playerSettings = false;
                            playerHeightAmmount = "";
                            playerWidthAmmount = "";
                            playerFlagsAmmount = "";
                        }
                    }
                }

            }

            dificultySelectWindow.clear(Color(187, 187, 187, 100));
            if (!playerSettings)
            {
                for (i = 0; i < 4; i++)
                    dificultySelectWindow.draw(dificultySelectButton[i]);
                dificultySelectWindow.draw(difficultyText);
                dificultySelectWindow.draw(dificulty9x9);
                dificultySelectWindow.draw(dificulty16x16);
                dificultySelectWindow.draw(dificulty16x30);
                dificultySelectWindow.draw(dificultyPlayer);
            }
            else if (playerSettings)
            {
                playerHeightTextMask.setString(playerHeightAmmount);
                playerWidthTextMask.setString(playerWidthAmmount);
                playerFlagsTextMask.setString(playerFlagsAmmount);
                dificultySelectWindow.draw(playerHeightText);
                dificultySelectWindow.draw(playerWidthText);
                dificultySelectWindow.draw(playerFlagsText);
                dificultySelectWindow.draw(playerHeightButton);
                dificultySelectWindow.draw(playerWidthButton);
                dificultySelectWindow.draw(playerFlagsButton);
                dificultySelectWindow.draw(playerHeightTextMask);
                dificultySelectWindow.draw(playerWidthTextMask);
                dificultySelectWindow.draw(playerFlagsTextMask);
                dificultySelectWindow.draw(applyPlayerParametrs);
                dificultySelectWindow.draw(applyPlayerParametrsText);
                dificultySelectWindow.draw(playerSettingsBackButton);
            }
            dificultySelectWindow.display();
        }


        while (gameWindow.isOpen())
        {
            Event gameEvent;

            //time
            if(!win && !lose && !neutral)
                gameTimer = gameTime.getElapsedTime();

            //Events
            while (gameWindow.pollEvent(gameEvent))
            {
                if (gameEvent.type == Event::Closed)
                {
                    gameWindow.close();
                    results.close();
                }
                if (gameEvent.type == Event::MouseButtonPressed && gameEvent.mouseButton.button == Mouse::Left)
                {
                    if (CheckPosition(restartButton, mouse, gameWindow))
                        Restart();
                    if (CheckPosition(backButton, mouse, gameWindow))
                    {
                        dificultySelectWindow.create(VideoMode(384, 200), "MineSweeper");
                        dificultySelectWindow.setIcon(32, 32, icon.getPixelsPtr());
                        gameWindow.close();
                    }
                    if (!lose && !win && !neutral)
                        for (yCord = 0; yCord < cell.size(); yCord++)
                            for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                                if (CheckPosition(cell[yCord][xCord], mouse, gameWindow))
                                {
                                    if (click < 1 && cellType[yCord][xCord].second == 1)
                                    {
                                        do
                                        {
                                            mine = 0;
                                            Generating();
                                        } while (cellType[yCord][xCord].second == 1);
                                    }

                                    if (cellType[yCord][xCord].second != 3 && cellType[yCord][xCord].second != 4)
                                    {
                                        cout << "Start Cell Action" << endl;
                                        CellAction(yCord, xCord, 1);
                                    }
                                    click++;
                                }
                }
                if (gameEvent.type == Event::MouseButtonPressed && gameEvent.mouseButton.button == Mouse::Right && !lose && !win && !neutral)
                {
                    for (yCord = 0; yCord < cell.size(); yCord++)
                        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                            if (CheckPosition(cell[yCord][xCord], mouse, gameWindow))

                            {
                                cout << "Detected right" << endl;
                                if (cellType[yCord][xCord].second != 3 && cellType[yCord][xCord].second != 4 && flags > 0 && !cellType[yCord][xCord].first)
                                    CellAction(yCord, xCord, 2);
                                click++;
                            }
                }
                if (gameEvent.type == Event::MouseButtonPressed && gameEvent.mouseButton.button == Mouse::Middle && !lose && !win && !neutral)
                {
                    for (yCord = 0; yCord < cell.size(); yCord++)
                        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                            if (CheckPosition(cell[yCord][xCord], mouse, gameWindow))
                            {
                                if ((cellType[yCord][xCord].second == 0 || cellType[yCord][xCord].second == 1 || cellType[yCord][xCord].second == 2) && !cellType[yCord][xCord].first)
                                    CellAction(yCord, xCord, 3);
                                else if (cellType[yCord][xCord].second == 5 || cellType[yCord][xCord].second == 6 || cellType[yCord][xCord].second == 7)
                                    CellAction(yCord, xCord, 4);
                            }
                }
            }

            // Other variables interactions && functions
            TimerAndFlags();
            if (!win && !lose && !neutral)
                WinLoseConditions();
            if (conditions == 0)
            {
                if (win)
                {
                    restartButton.setTextureRect(IntRect(75, 24, 24, 24));
                    conditions++;
                    results.setPosition(Vector2i(gameWindow.getPosition().x + gameWindow.getSize().x, gameWindow.getPosition().y));
                    resultsText.setFont(font);
                    resultsText.setString("Time: " + to_string(gameTimer.asSeconds()) + " sec\nClicks: " + to_string(click) + "\nEffency: 100%");
                    resultsText.setFillColor(sf::Color::Black);
                    results.setVisible(true);
                    for(yCord = 0; yCord < cell.size(); yCord++)
                        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                        {
                            if (cellType[yCord][xCord].second == 3)
                                cell[yCord][xCord].setTextureRect(IntRect(119, 49, 16, 16));
                            else if ((cellType[yCord][xCord].second == 1 && cell[yCord][xCord].getTextureRect() != IntRect(102, 49, 16, 16)) || cellType[yCord][xCord].second == 6)
                                cell[yCord][xCord].setTextureRect(IntRect(85, 49, 16, 16));
                        }
                }
                else if (neutral || lose)
                {
                    if (neutral)
                        restartButton.setTextureRect(IntRect(50, 24, 24, 24));
                    else if (lose)
                        restartButton.setTextureRect(IntRect(100, 24, 24, 24));

                    for (yCord = 0; yCord < cell.size(); yCord++)
                        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                        {
                            if (cellType[yCord][xCord].second == 3)
                                cell[yCord][xCord].setTextureRect(IntRect(119, 49, 16, 16));
                            else if ((cellType[yCord][xCord].second == 1 && cell[yCord][xCord].getTextureRect() != IntRect(102, 49, 16, 16)) || cellType[yCord][xCord].second == 6)
                                cell[yCord][xCord].setTextureRect(IntRect(85, 49, 16, 16));
                        }
                    if (lose)
                        cell[mineDestroyPlayer[1]][mineDestroyPlayer[0]].setTextureRect(IntRect(102, 49, 16, 16));
                    effencyCounter = baseFlagAmmount - flags;
                    for (yCord = 0; yCord < cell.size(); yCord++)
                        for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                            if (cellType[yCord][xCord].second == 3)
                            {
                                effencyCounter--;
                                cout << "new effency counter: " << effencyCounter << endl;
                            }
                    effency = (effencyCounter / baseFlagAmmount) * 100;
                    cout << "new effency: " << effency << endl;

                    results.setPosition(Vector2i(gameWindow.getPosition().x + gameWindow.getSize().x, gameWindow.getPosition().y));
                    resultsText.setFont(font);
                    resultsText.setString("Time: " + to_string((int)gameTimer.asSeconds()) + " sec\nClicks: " + to_string(click) + "\nEffency: " + to_string((int) effency) + "%" );
                    resultsText.setCharacterSize(12);
                    resultsText.setFillColor(sf::Color::Black);
                    results.setVisible(true);
                    conditions++;
                }
            }

            // Visual
            gameWindow.clear(Color(187, 187, 187, 100));

            for (yCord = 0; yCord < cell.size(); yCord++)
                for (xCord = 0; xCord < cell[yCord].size(); xCord++)
                    gameWindow.draw(cell[yCord][xCord]);
            for (i = 0; i < 3; i++)
            {
                gameWindow.draw(timerCounter[i]);
                gameWindow.draw(flagsCounter[i]);
            }
            gameWindow.draw(restartButton);
            gameWindow.draw(backButton);
            gameWindow.display();

            if (win || lose || neutral)
            {
                while (results.pollEvent(gameEvent))
                    if (gameEvent.type == Event::Closed)
                        results.setVisible(false);
                results.clear(Color::White);


                results.draw(resultsText);
                results.display();
            }

        }
    }
    

    return 0;
} 