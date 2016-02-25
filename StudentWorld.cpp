#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <random>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
StudentWorld::~StudentWorld(){
    delete m_player;
    eraseDirt(0, 0, 63, 63);
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        delete *p;
        list<Actor*>::iterator temp = m_actors.erase(p);
        temp--;
        p = temp;
    }
}

int StudentWorld::init()
{
    m_player = new FrackMan(this);
    for(int i = 0; i<VIEW_WIDTH; i++){
        for(int j = 0; j<VIEW_HEIGHT; j++){
            if((i >= 30 && i <= 33 && j >=4) || j >= 60)
                m_dirtArr[i][j] = nullptr;
            else
                m_dirtArr[i][j] = new Dirt(i, j);
        }
    }
    
    m_nBarrels = 0;
    m_nProtesters = 0;
    m_nTicksSinceLastProtest = 0;
    //Initialization of Game Objects
    int boulders = (getLevel() / 2) + 2;
    int gold = 5 - (getLevel() / 2);
    int barrels = 2 + getLevel();
    int bouldToAdd = min(boulders, 6);
    int goldToAdd = max(gold, 2);
    int barrelsToAdd = min(barrels, 20);
    int randX, randY;
    for( ; bouldToAdd > 0; bouldToAdd--){
        canAddHere(randX, randY, true);
        m_actors.push_back(new Boulder(randX, randY, this));
    }
    for( ; goldToAdd > 0; goldToAdd --){
        canAddHere(randX, randY, false);
        m_actors.push_back(new Nugget(randX, randY, this, false, GOODIE_PERMANENT, FMAN_GOODIE));
    }
    for( ; barrelsToAdd > 0; barrelsToAdd--){
        canAddHere(randX, randY, false);
        m_actors.push_back(new Barrel(randX, randY, this));
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    setDisplayText();
    string t = "false";
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        Actor* pointer = *p;
        if(pointer->getType() == protester && faceFrackMan(pointer))
            t = "true";
    }
    cout<<t;
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        (*p)->doSomething();
        if(!m_player->isAlive())
            return GWSTATUS_PLAYER_DIED;
        if(m_nBarrels <= 0){
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        if(!(*p)->isAlive()){
            delete *p;
            list<Actor*>::iterator temp = m_actors.erase(p);
            temp--;
            p = temp;
        }
    }
    
    m_player->doSomething();
    
    if(!m_player->isAlive()){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    else if(m_nBarrels <= 0){
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }

    addActors();
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    eraseDirt(0, 0, 63, 63);
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        delete *p;
        list<Actor*>::iterator temp = m_actors.erase(p);
        temp--;
        p = temp;
    }
    m_nBarrels = 0;
    m_nProtesters = 0;
}


bool StudentWorld::eraseDirt(int startX, int startY, int endX, int endY){
    bool returnValue = false;
    for(int i = startX; i <= endX; i++){
        for(int j = startY; j <= endY; j++){
            if(m_dirtArr[i][j] != nullptr){
                delete m_dirtArr[i][j];
                m_dirtArr[i][j] = nullptr;
                returnValue = true;
            }
        }
    }
    return returnValue;
}

void StudentWorld::setDisplayText(){
    string score = to_string(getScore());
    addFrontChar('0', score, 6);
    string level = to_string(getLevel());
    addFrontChar(' ', level, 2);
    string lives = to_string(getLives());
    string healthPercent = to_string(m_player->getHP()*10);
    addFrontChar(' ', healthPercent, 3);
    string squirts = to_string(m_player->getSquirts());
    addFrontChar(' ', squirts, 2);
    string gold = to_string(m_player->getNuggets());
    addFrontChar(' ', gold, 2);
    string sonar = to_string(m_player->getSonar());
    addFrontChar(' ', sonar, 2);
    string barrelsLeft = to_string(m_nBarrels);
    addFrontChar(' ', barrelsLeft, 2);
    // Next, create a string from your statistics, of the form: // “Scr: 0321000 Lvl: 52 Lives: 3 Hlth: 80% Water: 20 Gld: 3 Sonar: 1 Oil Left: 2”
    string s = "Scr: " + score + "  Lvl: " + level + "  Lives: " + lives + "  Hlth: " + healthPercent + "%  Wtr: " + squirts + "  Gld: " + gold + "  Sonar: " + sonar + "  Oil Left: " + barrelsLeft;
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

inline void StudentWorld::addFrontChar(char c, string& s, int desLength) const{
    for(size_t i = s.length(); i<desLength; i++)
        s = c + s;
}

void StudentWorld::addActors(){
    //Code for checking whether or not to add Protester
    bool canAddProtest = false;
    int t = 200 - getLevel();
    int minTicks = max(25, t);
    if(m_nTicksSinceLastProtest >= minTicks)
        canAddProtest = true;
    int p = 2 + (getLevel() * 1.5);
    int maxProtesters = min(15, p);
    if(m_nProtesters < maxProtesters && canAddProtest)
        addProtester();
    //code for checking whether or not to add the goodie
    int goodieRand = getLevel()*25 + 300;
    int goodieNum = randInt(0, goodieRand);
    if(goodieNum == 66){
        addGoodie();
    }
}

void StudentWorld::addProtester(){
    m_nTicksSinceLastProtest = 0;
    int t = (getLevel() * 10) + 30;
    int protestRand = min(90, t);
    int protestNum = randInt(0, protestRand);
    if(protestNum < protestRand)
        m_actors.push_back(new HardCoreProtester(60, 60, this));
    else
        m_actors.push_back(new Protester(60, 60, this));
}

void StudentWorld::addGoodie(){
    double result = randInt(1, 5);
    if(result <= 4){
        int randomX, randomY;
        do{
            randomX = randInt(0, 60);
            randomY = randInt(0, 60);
        }while(isDirtOverlap(randomX, randomY));
        m_actors.push_back(new WaterPool(randomX, randomY, this));
    }
    else
        m_actors.push_back(new SonarKit(0, 60, this));
}

bool StudentWorld::isDirtOverlap(int x, int y) const{
    for(int i = 0; i<4; i++){
        for(int j = 0; j<4; j++){
            if(x+i < 64 && y+j < 64 && m_dirtArr[x+i][y+j] != nullptr)
                return true;
        }
    }
    return false;
}

moveStatus StudentWorld::canMoveTo(int x, int y){
    if(x > VIEW_WIDTH - 4 || y > VIEW_HEIGHT - 4 || x < 0 || y < 0)
        return edgeBlocked;
    if(isBoulderBlocking(x, y))
        return boulderBlocked;
    return canMove;
}

bool StudentWorld::isBoulderBlocking(int x, int y){
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        Actor* obj = *p;
        if(obj->getType() == boulder && obj->distance(x, y) <= 3.0)
            return true;
    }
    return false;
}

void StudentWorld::canAddHere(int& randX, int& randY, bool isBoulder){
    bool okDistance;
    do{
        randX = randInt(0, 60);
        if(isBoulder)
            randY = randInt(20, 56);
        else
            randY = randInt(0, 56);
        okDistance = distanceGreaterThan6(randX, randY);
    }while(!okDistance || (randX >= 26 && randX <= 33));
}

bool StudentWorld::distanceGreaterThan6(int x, int y){
    if(m_actors.empty())
        return true;
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        if((*p)->distance(x, y) <= 6.0)
            return false;
    }
    return true;
}

void StudentWorld::setRadiusVisible(double x, double y, double rad){
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if((*p)->distance(x, y) <= rad)
            (*p)->setVisible(true);
    }
}

Protester* StudentWorld::annoyProtestersNear(oneOrAll annoyHowMany, int pointsToAnnoy, double x, double y, double rad){
    Protester* toReturn = nullptr;
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        if((*p)->getType() == protester){
            //mark toReturn as a protester
            toReturn = static_cast<Protester*>(*p);
            if(toReturn->canBeAnnoyed() && toReturn->distance(x, y) <= rad){
                toReturn->getAnnoyed(pointsToAnnoy);
                if(pointsToAnnoy == 100)
                    increaseScore(500);
                if(annoyHowMany == one)
                    return toReturn;
            }
            else
                toReturn = nullptr;
        }
    }
    return toReturn;
}

bool StudentWorld::faceFrackMan(Actor* setFacing){
    if(m_player->distance(setFacing) <= 4)
        return false;
    int xDiff = abs(setFacing->getX() - m_player->getX());
    int yDiff = abs(setFacing->getY() - m_player->getY());
    int lowerX = (m_player->getX() <= setFacing->getX() ? m_player ->getX() : setFacing->getX());
    int lowerY = (m_player->getY() <= setFacing->getY()? m_player->getY() : setFacing->getY());
    if(xDiff == 0){
        for(int i = 0; i < yDiff; i++){
            if(isDirtAt(lowerX, lowerY+i) || isBoulderBlocking(lowerX, lowerY+i))
                return false;
        }
        
        if(lowerY == setFacing->getY())
            setFacing->setDirection(GraphObject::up);
        else setFacing->setDirection(GraphObject::down);
        
        return true;
    }
    else if(yDiff == 0){
        for(int i = 0; i < xDiff; i++){
            if(isDirtAt(lowerX+i, lowerY) || isBoulderBlocking(lowerX+i, lowerY))
                return false;
        }
        
        if(lowerX == setFacing->getX())
            setFacing->setDirection(GraphObject::right);
        else setFacing->setDirection(GraphObject::left);
        
        return true;
    }
    return false;
}

void StudentWorld::xAndYtoLeave(int& x, int& y){
    
}

int randInt(int min, int max){
    if (max < min)
        swap(max, min);
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}
