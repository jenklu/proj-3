#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <ctime>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
using namespace std;
Actor::Actor(int imageID, int startX, int startY, Direction dir, StudentWorld* currWorld, double size, unsigned int depth)
:GraphObject(imageID, startX, startY, dir, size, depth)
{
    m_currWorld = currWorld;
    m_alive = true;
    setVisible(true);
}

double Actor::distance(Actor* check) const{
    double dxdy = pow(static_cast<double>(getX() - check->getX()), 2.0) + pow(static_cast<double>(getY() - check->getY()), 2.0);
    return sqrt(dxdy);
}

double Actor::distance(int x, int y) const{
    double dxdy = pow(static_cast<double>(getX() - x), 2.0) + pow(static_cast<double>(getY() - y), 2.0);
    return sqrt(dxdy);
}
int Actor::doSomething(){
    if(!isAlive())
        return GWSTATUS_PLAYER_DIED;
    return uniqueDoSomething();
}

/////////////////////////////////////////////////////////////////////////
Person::Person(int imageID, int startX, int startY,  Direction dir, StudentWorld* currWorld, int hitPoints, double size, unsigned int depth)
:Actor(imageID, startX, startY, dir, currWorld, size, depth){
    m_hitPoints = hitPoints;
}

void Person::getAnnoyed(int toSub){
    m_hitPoints -= toSub;
    if(m_hitPoints <= 0){
        setDead();
        playGiveUpSound();
    }
}

/////////////////////////////////////////////////////////////////////////
Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* currWorld, bool visibility, int permanence, int pointValue, int soundID, int forwho)
:Actor(imageID, startX, startY, right, currWorld, 1.0, 2){
    setVisible(visibility);
    m_permanence = permanence;
    m_forwhom = forwho;
    m_pointValue = pointValue;
    m_soundID = soundID;
    if(m_permanence == GOODIE_TEMP && imageID != IID_GOLD)
        m_ticksLeft = 100;
    else if(m_permanence == GOODIE_TEMP){
        int maxTicks = 300 - (getWorld()->getLevel() * 10);
        m_ticksLeft = max(100, maxTicks);
    }
    else
        m_ticksLeft = -1;
}

int Goodie::uniqueDoSomething(){
    if(distance(getWorld()->getPlayer()) <= 3.0 && m_forwhom == FMAN_GOODIE){
        setDead();
        getWorld()->increaseScore(m_pointValue);
        getWorld()->playSound(m_soundID);
        getFound();
    }
    return goodieDoSomething();
}

int Goodie::goodieDoSomething(){
    m_ticksLeft--;
    if(m_ticksLeft <= 0){
        setDead();
        return 0;
    }
    return 1;
}
/////////////////////////////////////////////////////////////////////////
FrackMan::FrackMan(StudentWorld* currWorld)
:Person(IID_PLAYER, 30, 60, right, currWorld, 10, 1, 0){
    m_squirts = 5;
    m_sonar = 1;
    m_nuggets = 0;
}

int FrackMan::uniqueDoSomething(){
    int move;
    if(getWorld()->eraseDirt(getX(), getY(), getX()+3, getY()+3)){
        getWorld()->playSound(SOUND_DIG);
        return GWSTATUS_CONTINUE_GAME;
    }
    else if(getWorld()->getKey(move)){
        int currX = getX();
        int currY = getY();
        switch (move) {
            case KEY_PRESS_ESCAPE:
                setDead();
                return GWSTATUS_PLAYER_DIED;
                break;
                
            case KEY_PRESS_SPACE:
            {
                if(m_squirts > 0){
                    Actor* newSquirt = createSquirt();
                    if(newSquirt != nullptr)
                        getWorld()->addToWorld(newSquirt);
                }
                break;
            }
                
            case KEY_PRESS_UP:
                if(getDirection() != up)
                    setDirection(up);
                else if(canMoveTo(currX, currY + 1))
                    moveTo(currX, currY+1);
                else
                    moveTo(currX, currY);
                break;
                
            case KEY_PRESS_RIGHT:
                if(getDirection() != right)
                    setDirection(right);
                else if(canMoveTo(currX + 1, currY))
                    moveTo(currX+1, currY);
                else
                    moveTo(currX, currY);
                break;
                
            case KEY_PRESS_DOWN:
                if(getDirection() != down)
                    setDirection(down);
                else if(canMoveTo(currX, currY - 1))
                    moveTo(currX, currY - 1);
                else
                    moveTo(currX, currY);
                break;
                
            case KEY_PRESS_LEFT:
                if(getDirection() != left)
                    setDirection(left);
                else if(canMoveTo(currX - 1, currY))
                    moveTo(currX - 1, currY);
                else
                    moveTo(currX, currY);
                break;
                
            default:
                break;
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

inline void FrackMan::playGiveUpSound() const{
    getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
}


void FrackMan::addBoulderLoc(int x, int y){
    Coord toAdd;
    toAdd.x = x;
    toAdd.y = y;
    boulderLocs.push_back(toAdd);
}

bool FrackMan::canMoveTo(int x, int y){
    if(x > VIEW_WIDTH - 4 || y > VIEW_HEIGHT - 4 || x < 0 || y < 0)
        return false;
    for(int i = 0; i != boulderLocs.size(); i++)
        if(distance(boulderLocs[i].x, boulderLocs[i].y) <= 3.00)
            return false;
    return true;
}

Actor* FrackMan::createSquirt(){
    m_squirts--;
    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
    Direction d = getDirection();
    int currX = getX();
    int currY = getY();
    switch(d){
        case up:
            if(canMoveTo(currX, currY+4) && !getWorld()->isDirtOverlap(currX, currY+4))
                return new Squirt(currX, currY+4, d, getWorld());
            break;
        case right:
            if(canMoveTo(currX+4, currY) && !getWorld()->isDirtOverlap(currX+4, currY))
               return new Squirt(currX+4, currY, d, getWorld());
            break;
        case down:
            if(canMoveTo(currX, currY-4) && !getWorld()->isDirtOverlap(currX, currY-4))
                return new Squirt(currX, currY - 4, d, getWorld());
            break;
        case left:
            if(canMoveTo(currX-4, currY) && !getWorld()->isDirtOverlap(currX-4, currY))
                return new Squirt(currX - 4, currY, d, getWorld());
            break;
    }
    return nullptr;
}

/////////////////////////////////////////////////////////////////////////
Boulder::Boulder(int startX, int startY, StudentWorld* currWorld)
:Actor(IID_BOULDER, startX, startY, down, currWorld, 1, 1){
    m_state = BSTATE_STABLE;
    getWorld()->eraseDirt(startX, startY, startX+3, startY+3);
    m_nTicksWaited = 0;
    currWorld->getPlayer()->addBoulderLoc(startX, startY);
}


int Boulder::uniqueDoSomething(){
    if(m_state == BSTATE_STABLE){
        if(!isDirtBelow())
            m_state = BSTATE_WAITING;
        return 1;
    }
    else if(m_state == BSTATE_WAITING){
        m_nTicksWaited++;
        if(m_nTicksWaited >= 30){
            getWorld()->playSound(SOUND_FALLING_ROCK);
            m_state = BSTATE_FALLING;
        }
        return 1;
    }
    else if(m_state == BSTATE_FALLING){
        if(isDirtBelow())
            setDead();
        moveTo(getX(), getY()-1);
        if(getY() < 0)
            setDead();
        if(distance(getWorld()->getPlayer()) <= 3.00)
            getWorld()->getPlayer()->getAnnoyed(100);
        
    }
    return 1;
}

bool Boulder::isDirtBelow(){
    int i = 0;
    for( ; i<4; i++){
        if(getWorld()->isDirtAt(getX()+i, getY()-1))
            break;
    }
    if(i == 4)
        return false;
    return true;
}

/////////////////////////////////////////////////////////////////////////
Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* currWorld)
:Actor(IID_WATER_SPURT, startX, startY, dir, currWorld, 1, 1){
    m_squaresLeft = 4;
}

int Squirt::uniqueDoSomething(){
    return 1;
}

/////////////////////////////////////////////////////////////////////////
Barrel::Barrel(int startX, int startY, StudentWorld* currWorld)
:Goodie(IID_BARREL, startX, startY, currWorld, false, GOODIE_PERMANENT, 1000, SOUND_FOUND_OIL){
    currWorld->addBarrel();
}

void Barrel::getFound(){
    getWorld()->playSound(SOUND_FOUND_OIL);
    getWorld()->increaseScore(1000);
    getWorld()->decBarrel();
}

int Barrel::goodieDoSomething(){
    if(distance(getWorld()->getPlayer()) <= 4.00 && distance(getWorld()->getPlayer()) > 3){
        setVisible(true);
    }
    return GWSTATUS_CONTINUE_GAME;
}

/////////////////////////////////////////////////////////////////////////
Nugget::Nugget(int startX, int startY, StudentWorld* currWorld, bool visibility, int permanence, int forwho)
:Goodie(IID_GOLD, startX, startY, currWorld, visibility, permanence, 10, SOUND_GOT_GOODIE, forwho){
}

int Nugget::goodieDoSomething(){
    if(distance(getWorld()->getPlayer()) <= 4.00 && distance(getWorld()->getPlayer()) > 3){
        setVisible(true);
    }
    return GWSTATUS_CONTINUE_GAME;
}

void Nugget::getFound(){
    getWorld()->getPlayer()->addGold();
}

/////////////////////////////////////////////////////////////////////////
SonarKit::SonarKit(int startX, int startY, StudentWorld* currWorld)
:Goodie(IID_SONAR, startX, startY, currWorld, true, GOODIE_TEMP, 75)
{}

void SonarKit::getFound(){
    getWorld()->getPlayer()->addSonar();
}

/////////////////////////////////////////////////////////////////////////
WaterPool::WaterPool(int startX, int startY, StudentWorld* currWorld)
:Goodie(IID_WATER_POOL, startX, startY, currWorld, true, GOODIE_TEMP, 100)
{}

void WaterPool::getFound(){
    getWorld()->getPlayer()->addFiveSquirts();
}

/////////////////////////////////////////////////////////////////////////
 // Person(int imageID, int startX, int startY,  Direction dir, StudentWorld* currWorld, int hitPoints, double size = 1.0, unsigned int depth = 0);
Protestor::Protestor(int startX, int startY, StudentWorld* currWorld, int hitPoints, int imageID)
:Person(imageID, startX, startY, left, currWorld, hitPoints){
    srand(time(NULL));
    m_stepsToKeepGoing = rand() % 53 + 8;
    m_leaveScreenState = false;
}

int Protestor::uniqueDoSomething(){
    return hardCoreDoSomething();
}

int Protestor::hardCoreDoSomething(){
    return GWSTATUS_CONTINUE_GAME;
}
void Protestor::playGiveUpSound() const{
    getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
}

/////////////////////////////////////////////////////////////////////////
HardCoreProtestor::HardCoreProtestor(int startX, int startY, StudentWorld* currWorld)
:Protestor(startX, startY, currWorld, 20, IID_HARD_CORE_PROTESTER)
{}
