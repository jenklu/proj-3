#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <ctime>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
StudentWorld::~StudentWorld(){
    delete player;
    eraseDirt(0, 0, 63, 63);
    for(list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        delete *p;
        list<Actor*>::iterator temp = actors.erase(p);
        temp--;
        p = temp;
    }
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
    m_nProtestors = 0;
    int barrels = 2+getLevel();
    m_nBarrels = min(barrels, 20);
    m_nTicksSinceLastProtest = 500;
    
    actors.emplace_back(new Barrel(5, 5, this));
    actors.emplace_back(new Boulder(25, 25, this));
    actors.emplace_back(new Protestor(60, 60, this));
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    setDisplayText();
    for(list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        (*p)->doSomething();
        if(!player->isAlive())
            return GWSTATUS_PLAYER_DIED;
        if(m_nBarrels <= 0){
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    for(list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        if(!(*p)->isAlive()){
            delete *p;
            list<Actor*>::iterator temp = actors.erase(p);
            temp--;
            p = temp;
        }
    }
    addActors();
    if(player->doSomething() == GWSTATUS_PLAYER_DIED){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    else if(m_nBarrels <= 0){
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete player;
    eraseDirt(0, 0, 63, 63);
    for(list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        delete *p;
        list<Actor*>::iterator temp = actors.erase(p);
        temp--;
        p = temp;
    }
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
    string sonar = to_string(player->getSonar());
    addFrontChar(' ', sonar, 2);
    string barrelsLeft = to_string(m_nBarrels);
    addFrontChar(' ', barrelsLeft, 2);
    // Next, create a string from your statistics, of the form: // “Scr: 0321000 Lvl: 52 Lives: 3 Hlth: 80% Water: 20 Gld: 3 Sonar: 1 Oil Left: 2”
    string s = "Scr: " + score + "  Lvl: " + level + "  Lives: " + lives + "  Hlth: " + healthPercent + "%  Wtr: " + squirts + "  Gld: " + gold + "  Sonar: " + sonar + "  Oil Left: " + barrelsLeft;
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

inline void StudentWorld::addFrontChar(char c, string& s, int desLength){
    for(size_t i = s.length(); i<desLength; i++)
        s = c + s;
}

void StudentWorld::addActors(){
    //Code for checking whether or not to add protestor
    bool canAddProtest = false;
    int t = 200 - getLevel();
    int minTicks = max(25, t);
    if(m_nTicksSinceLastProtest >= minTicks)
        canAddProtest = true;
    int p = 2 + (getLevel() * 1.5);
    int maxProtestors = min(15, p);
    if(m_nProtestors < maxProtestors && canAddProtest)
        addProtestor();
    //code for checking whether or not to add the goodie
    srand(time(NULL));
    int goodieRand = getLevel()*25 + 300;
    int goodieNum = rand() % goodieRand;
    if(goodieNum == 66){
        addGoodie();
    }
}

void StudentWorld::addProtestor(){
    m_nTicksSinceLastProtest = 0;
    srand(time(NULL));
    double t = (getLevel() / 10) + .3;
    double protestRand = min(.9, t);
    double protestNum = rand()/RAND_MAX;
    if(protestNum < protestRand)
        actors.emplace_back(new HardCoreProtestor(60, 60, this));
    else
        actors.emplace_back(new Protestor(60, 60, this));
}

void StudentWorld::addGoodie(){
    srand(time(NULL));
    double probability = .8;
    double result = rand()/RAND_MAX;
    if(result <= probability){
        int randomX, randomY;
        do{
        randomX = rand() % 60;
        randomY = rand() % 60;
        }while(isDirtOverlap(randomX, randomY));
        actors.emplace_back(new WaterPool(randomX, randomY, this));
    }
    else
        actors.emplace_back(new SonarKit(0, 60, this));
}

bool StudentWorld::isDirtOverlap(int x, int y){
    for(int i = x; i<4; i++){
        for(int j = y; j<4; j++){
            if(i < 64 && j < 64 && dirtArr[i][j] != nullptr)
                return true;
        }
    }
    return false;
}

void canAddHere(int& x, int& y){
    
}
