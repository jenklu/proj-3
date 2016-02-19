#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
StudentWorld::~StudentWorld(){
    delete player;
    eraseDirt(0, 0, 63, 63);
}

int StudentWorld::init()
{
    player = new FrackMan(this);
    for(int i = 0; i<VIEW_WIDTH; i++){
        for(int j = 0; j<VIEW_HEIGHT; j++){
            if((i >= 30 && i <= 33 && j >=4) || j >= 60)
                dirtArr[i][j] = nullptr;
            else
                dirtArr[i][j] = new Dirt(i, j);
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    /*
     srand(getScore());
     int goodieRand = getLevel()*25 + 300;
     int goodieNum = rand() % goodieRand;
     if(goodieNum == 66)
        addGoodie();
     */
    setDisplayText();
    return player->doSomething();
}

void StudentWorld::cleanUp()
{
    delete player;
    eraseDirt(0, 0, 63, 63);
}

bool StudentWorld::eraseDirt(int startX, int startY, int endX, int endY){
    bool returnValue = false;
    for(int i = startX; i <= endX; i++){
        for(int j = startY; j <= endY; j++){
            if(dirtArr[i][j] != nullptr){
                delete dirtArr[i][j];
                dirtArr[i][j] = nullptr;
                returnValue = true;
            }
        }
    }
    return returnValue;
}

void StudentWorld::setDisplayText()
{
    string score = to_string(getScore());
    addFrontChar('0', score, 6);
    string level = to_string(getLevel());
    addFrontChar(' ', level, 2);
    string lives = to_string(getLives());
    string healthPercent = to_string(player->getHP()*10);
    addFrontChar(' ', healthPercent, 3);
    string squirts = to_string(player->getSquirts());
    addFrontChar(' ', squirts, 2);
    string gold = to_string(player->getNuggets());
    addFrontChar(' ', gold, 2);
    string sonar = to_string(player->getCharges());
    addFrontChar(' ', sonar, 2);
    //*********
    //*********
    //FIX NUMBER OF BARRELS LEFT ONCE BARREL CLASS IS MADE
    string barrelsLeft = "0";
    addFrontChar(' ', barrelsLeft, 2);
    //************
    //************
    // Next, create a string from your statistics, of the form: // “Scr: 0321000 Lvl: 52 Lives: 3 Hlth: 80% Water: 20 Gld: 3 Sonar: 1 Oil Left: 2”
    string s = "Scr: " + score + "  Lvl: " + level + "  Lives: " + lives + "  Hlth: " + healthPercent + "%  Wtr: " + squirts + "  Gld: " + gold + "  Sonar: " + sonar + "  Oil Left: " + barrelsLeft;
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

inline void StudentWorld::addFrontChar(char c, string& s, int desLength){
    for(size_t i = s.length(); i<desLength; i++)
        s = c + s;
}

void StudentWorld::killPlayer(){
    player->setDead();
}
