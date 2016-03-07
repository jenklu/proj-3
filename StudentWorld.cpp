#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <random>
#include <queue>
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
    for(int i = 0; i < 61; i++)
        for(int j = 0; j < 61; j++){
            delete howFarFrom[i][j];
            howFarFrom[i][j] = nullptr;
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
    m_nTicksSinceLastProtest = 500;
    int p = 2 + (getLevel() * 1.5);
    m_maxProtesters = min(15, p);
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
    fillDistFrom(topright);
    fillDistFrom(player);
    m_shouldUpdateDistExit = false;
    m_shouldUpdateDistPlayer = false;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    setDisplayText();
    if(m_shouldUpdateDistExit && areProtestersLeaving()){
        fillDistFrom(topright);
        m_shouldUpdateDistExit = false;
    }
    if(m_shouldUpdateDistPlayer){
        fillDistFrom(player);
        m_shouldUpdateDistPlayer = false;
    }
    
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
            if((*p)->getType() == protester)
                m_nProtesters--;
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
    
    for(int i = 0; i < 61; i++)
        for(int j = 0; j < 61; j++){
            delete howFarFrom[i][j];
            howFarFrom[i][j] = nullptr;
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
                m_shouldUpdateDistExit = true;
                m_shouldUpdateDistPlayer = true;
                returnValue = true;
            }
        }
    }
    return returnValue;
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

void StudentWorld::setRadiusVisible(double x, double y, double rad){
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if((*p)->distance(x, y) <= rad)
            (*p)->setVisible(true);
    }
}

bool StudentWorld::annoyProtestersNear(Actor* annoyer, int pointsToAnnoy, double x, double y, double rad){
    bool toReturn = false;
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        if((*p)->getType() == protester){
            //mark toReturn as a protester
            Protester* prot = static_cast<Protester*>(*p);
            if(prot->canBeAnnoyed() && prot->distance(x, y) <= rad){
                prot->getAnnoyed(pointsToAnnoy, annoyer->getType());
                toReturn = true;
            }
        }
    }
    return toReturn;
}

Protester* StudentWorld::findProtesterNear(double x, double y){
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        if((*p)->getType() == protester && (*p)->distance(x, y) <= 3.0)
            return static_cast<Protester*>(*p);
    }
    return nullptr;
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
    m_nTicksSinceLastProtest++;
    int t = 200 - getLevel();
    int minTicks = max(25, t);
    if(m_nTicksSinceLastProtest >= minTicks){
        if(m_nProtesters < m_maxProtesters)
            addProtester();
    }
    //code for checking whether or not to add the goodie
    int goodieRand = getLevel()*25 + 300;
    int goodieNum = randInt(0, goodieRand);
    if(goodieNum == 66){
        addGoodie();
    }
}

void StudentWorld::addProtester(){
    m_nTicksSinceLastProtest = 0;
    m_nProtesters++;
    int t = (getLevel() * 10) + 30;
    int protestNum = min(90, t);
    int protestRand = randInt(0, 100);
    if(protestRand < protestNum)
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

int StudentWorld::numStepsFromPlayer(int x, int y){
    if(howFarFrom[x][y] != nullptr)
        return howFarFrom[x][y]->fromPlayer;
    else
        return 64*64;
}

bool StudentWorld::faceFrackMan(Actor* setFacing){
    if(setFacing->distance(m_player) <= 4)
        return false;
    int xDiff = abs(setFacing->getX() - m_player->getX());
    int yDiff = abs(setFacing->getY() - m_player->getY());
    int lowerX = (m_player->getX() <= setFacing->getX() ? m_player ->getX() : setFacing->getX());
    int lowerY = (m_player->getY() <= setFacing->getY()? m_player->getY() : setFacing->getY());
    if(xDiff == 0){
        for(int i = 0; i < yDiff; i++){
            if(isDirtOverlap(lowerX, lowerY+i) || isBoulderBlocking(lowerX, lowerY+i))
                return false;
        }
        
        if(lowerY == setFacing->getY())
            setFacing->setDirection(GraphObject::up);
        else setFacing->setDirection(GraphObject::down);
        
        return true;
    }
    else if(yDiff == 0){
        for(int i = 0; i < xDiff; i++){
            if(isDirtOverlap(lowerX+i, lowerY) || isBoulderBlocking(lowerX+i, lowerY))
                return false;
        }
        
        if(lowerX == setFacing->getX())
            setFacing->setDirection(GraphObject::right);
        else setFacing->setDirection(GraphObject::left);
        
        return true;
    }
    return false;
}

void StudentWorld::moveCloserTo(bool movingToExit, Actor* looking){
    int minSteps = 64*64;
    int x = looking->getX();
    int y = looking->getY();
    GraphObject::Direction directionToGo = looking->getDirection();
    if(leavingProtesterCanGoTo(x, y+1, movingToExit, minSteps)){
        directionToGo = GraphObject::up;
        minSteps = getStepsFrom(movingToExit, x, y+1);
    }
    if(leavingProtesterCanGoTo(x+1, y, movingToExit, minSteps)){
        directionToGo = GraphObject::right;
        minSteps = getStepsFrom(movingToExit, x+1, y);
    }
    if(leavingProtesterCanGoTo(x, y-1, movingToExit, minSteps)){
        directionToGo = GraphObject::down;
        minSteps = getStepsFrom(movingToExit, x, y-1);
    }
    if(leavingProtesterCanGoTo(x-1, y, movingToExit, minSteps)){
        directionToGo = GraphObject::left;
        minSteps = getStepsFrom(movingToExit, x-1, y);
    }
    looking->setDirection(directionToGo);
    giveNextLocInDir(directionToGo, x, y);
    looking->moveTo(x, y);
}

bool StudentWorld::leavingProtesterCanGoTo(int x, int y, bool movingToExit, int currMinSteps){
    if(howFarFrom[x][y] != nullptr && getStepsFrom(movingToExit, x, y) < currMinSteps && isEmptyLoc(x, y))
        return true;
    return false;
}

void StudentWorld::fillDistFrom(distanceFromWhere from){
    int currX, currY;
    bool exitStatus;
    if(from == topright){
        exitStatus = true;
        currX = 60;
        currY = 60;
    }
    else if(from == player){
        exitStatus = false;
        currX = m_player->getX();
        currY = m_player->getY();
    }
    int dist = 0;
    
    queue<distAndLoc*> locQueue;
    int countPushes = 0;
    locQueue.push(new distAndLoc(currX, currY, dist));
    while(!locQueue.empty()){
        distAndLoc* current = locQueue.front();
        currX = current->m_x;
        currY = current->m_y;
        dist = current->m_dist;
        
        if(howFarFrom[currX][currY] == nullptr){
            if(from == topright)
                howFarFrom[currX][currY] = new distFrom(0, dist, true);
            else if(from == player)
                howFarFrom[currX][currY] = new distFrom(dist, 0, true);
        }
        else{
            howFarFrom[currX][currY]->beenMarked = true;
            if(from == topright)
                howFarFrom[currX][currY]->fromExit = dist;
            else if(from == player)
                howFarFrom[currX][currY]->fromPlayer = dist;
        }
        delete current;
        locQueue.pop();
        
        if(isEmptyLoc(currX, currY+1) && (howFarFrom[currX][currY + 1] == nullptr || !howFarFrom[currX][currY + 1]->beenMarked)){
            locQueue.push(new distAndLoc(currX, currY + 1, dist+1));
            if(howFarFrom[currX][currY + 1] != nullptr)
                howFarFrom[currX][currY + 1]->beenMarked = true;
            countPushes++;
        }
        if(isEmptyLoc(currX+1, currY) && (howFarFrom[currX + 1][currY] == nullptr || !howFarFrom[currX+1][currY]->beenMarked)){
            locQueue.push(new distAndLoc(currX + 1, currY, dist+1));
            if(howFarFrom[currX + 1][currY] != nullptr)
                howFarFrom[currX + 1][currY]->beenMarked = true;
            countPushes++;
        }
        
        if(isEmptyLoc(currX, currY-1) && (howFarFrom[currX][currY - 1] == nullptr || !howFarFrom[currX][currY - 1]->beenMarked)){
            locQueue.push(new distAndLoc(currX, currY - 1, dist+1));
            if(howFarFrom[currX][currY - 1] != nullptr)
                howFarFrom[currX][currY - 1]->beenMarked = true;
            countPushes++;
        }
        
        if(isEmptyLoc(currX-1, currY)&& (howFarFrom[currX - 1][currY] == nullptr || !howFarFrom[currX-1][currY]->beenMarked)){
            locQueue.push(new distAndLoc(currX - 1, currY, dist+1));
            if(howFarFrom[currX - 1][currY] != nullptr)
                howFarFrom[currX - 1][currY]->beenMarked = true;
            countPushes++;
        }
        
    }
    if(from == topright)
       cout<<"Exit pushes: "<<countPushes<<endl;
    else if(from == player)
        cout<<"Player pushes: "<<countPushes<<endl;
    for(int i = 0; i < 61; i++){
        for(int j = 0; j < 61; j++){
            if(howFarFrom[i][j] != nullptr)
                howFarFrom[i][j]->beenMarked = false;
        }
    }
}

bool StudentWorld::isEmptyLoc(int x, int y){
    if(!isDirtOverlap(x, y) && canMoveTo(x, y) == canMove)
        return true;
    return false;
}

int StudentWorld::getStepsFrom(bool exitStatus, int x, int y){
    if(howFarFrom[x][y] == nullptr)
        return 65*65;
    if(exitStatus)
        return howFarFrom[x][y]->fromExit;
    return howFarFrom[x][y]->fromPlayer;
}

bool StudentWorld::areProtestersLeaving(){
    for(list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++){
        if((*p)->getType() == protester){
            Protester* prot = static_cast<Protester*>(*p);
            if(!prot->canBeAnnoyed())
                return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////
int randInt(int min, int max){
    if (max < min)
        swap(max, min);
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}