#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <algorithm>
#include <queue>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
using namespace std;

void giveNextLocInDir(GraphObject::Direction d, int& x, int& y){
    switch(d){
        case GraphObject::up:
            y++;
            break;
        case GraphObject::right:
            x++;
            break;
        case GraphObject::down:
            y--;
            break;
        case GraphObject::left:
            x--;
            break;
        case GraphObject::none:
            break;
    }
}

/////////////////////////////////////////////////////////////////////////
Actor::Actor(int imageID, int startX, int startY, Direction dir, StudentWorld* currWorld, double size, unsigned int depth)
:GraphObject(imageID, startX, startY, dir, size, depth)
{
    m_currWorld = currWorld;
    m_alive = true;
    setVisible(true);
    if(imageID == IID_BOULDER)
        m_type = boulder;
    else if(imageID == IID_HARD_CORE_PROTESTER || imageID == IID_PROTESTER)
        m_type = protester;
    else if(imageID == IID_WATER_SPURT)
        m_type = squirt;
    else m_type = other;
}

void Actor::doSomething(){
    if(!isAlive())
        return;
    uniqueDoSomething();
    return;
}

double Actor::distance(Actor* check) const{
    double dxdy = pow(static_cast<double>(getX() - check->getX()), 2.0) + pow(static_cast<double>(getY() - check->getY()), 2.0);
    return sqrt(dxdy);
}

double Actor::distance(int x, int y) const{
    double dxdy = pow(static_cast<double>(getX() - x), 2.0) + pow(static_cast<double>(getY() - y), 2.0);
    return sqrt(dxdy);
}

/////////////////////////////////////////////////////////////////////////
Person::Person(int imageID, int startX, int startY,  Direction dir, StudentWorld* currWorld, int hitPoints, int giveUpSound, double size, unsigned int depth)
:Actor(imageID, startX, startY, dir, currWorld, size, depth){
    m_hitPoints = hitPoints;
    m_giveUpSound = giveUpSound;
}

void Person::getAnnoyed(int toSub, actorType annoyer){
    m_hitPoints -= toSub;
    if(m_hitPoints <= 0){
        hpRanOut(annoyer);
        getWorld()->playSound(m_giveUpSound);
    }
    else
        wasAnnoyed();
}


/////////////////////////////////////////////////////////////////////////
Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* currWorld, bool visibility, int permanence, int pointValue, int soundID, int forWho)
:Actor(imageID, startX, startY, right, currWorld, 1.0, 2){
    setVisible(visibility);
    m_permanence = permanence;
    m_forWhom = forWho;
    m_pointValue = pointValue;
    m_soundID = soundID;
    if(m_permanence == GOODIE_TEMP && imageID == IID_GOLD)
        m_ticksLeft = 100;
    else if(m_permanence == GOODIE_TEMP){
        int maxTicks = 300 - (getWorld()->getLevel() * 10);
        m_ticksLeft = max(100, maxTicks);
    }
    else
        m_ticksLeft = -1;
}

void Goodie::uniqueDoSomething(){
    if(distance(getWorld()->getPlayer()) <= 3.0 && m_forWhom == FMAN_GOODIE){
        setDead();
        getWorld()->increaseScore(m_pointValue);
        getWorld()->playSound(m_soundID);
        getFound();
    }
    //Only permanent actors start off invisible, so they're the only ones we would want to setVisible
    else if(m_permanence == GOODIE_PERMANENT && distance(getWorld()->getPlayer()) <= 4.00){
        setVisible(true);
    }
    m_ticksLeft--;
    if(m_permanence == GOODIE_TEMP && m_ticksLeft <= 0){
        setDead();
        return;
    }
    goodieDoSomething();
    return;
}

/////////////////////////////////////////////////////////////////////////
FrackMan::FrackMan(StudentWorld* currWorld)
:Person(IID_PLAYER, 30, 60, right, currWorld, 10, SOUND_PLAYER_GIVE_UP, 1, 0){
    m_squirts = 5;
    m_sonar = 1;
    m_nuggets = 0;
}

void FrackMan::uniqueDoSomething(){
    int move;
    Direction d = getDirection();
    if(getWorld()->eraseDirt(getX(), getY(), getX()+3, getY()+3)){
        getWorld()->playSound(SOUND_DIG);
        return;
    }
    else if(getWorld()->getKey(move)){
        int currX = getX();
        int currY = getY();
        if(move == KEY_PRESS_ESCAPE){
                setDead();
                return;
        }
        else if(move == KEY_PRESS_SPACE){
            if(m_squirts > 0){
                Actor* newSquirt = createSquirt();
                if(newSquirt != nullptr)
                    getWorld()->addToWorld(newSquirt);
            }
        }
                
        else if(move== KEY_PRESS_UP)
            d = up;
                
        else if(move == KEY_PRESS_RIGHT)
            d = right;
        
        else if(move == KEY_PRESS_DOWN)
            d = down;
                
        else if(move == KEY_PRESS_LEFT)
             d = left;
        
        if(move == KEY_PRESS_UP || move == KEY_PRESS_RIGHT || move == KEY_PRESS_DOWN || move == KEY_PRESS_LEFT){
            if(getDirection() != d){
                setDirection(d);
                return;
            }
            int x = currX;
            int y = currY;
            giveNextLocInDir(d, x, y);
            moveStatus m = getWorld()->canMoveTo(x, y); //initialize this variable in all cases so that we do not have to call canMoveTo more than once, as that would be expensive
            if(m == canMove){
                moveTo(x, y);
                getWorld()->makeUpdateDistFromPlayer();
            }
            else if (m == edgeBlocked)
                moveTo(currX, currY);
        }
        else if(move == 'z' || move == 'Z'){
            if(m_sonar > 0){
                getWorld()->playSound(SOUND_SONAR);
                getWorld()->setRadiusVisible(getX(), getY(), 12.00);
                m_sonar--;
            }
        }
        //
        else if(move == KEY_PRESS_TAB){
            if(m_nuggets > 0){
                getWorld()->addToWorld(new Nugget(getX(), getY(), getWorld(), true, GOODIE_TEMP, PTESTOR_GOODIE));
                m_nuggets--;
            }
        }
    }
    return;
}

Actor* FrackMan::createSquirt(){
    m_squirts--;
    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
    Direction d = getDirection();
    int currX = getX();
    int currY = getY();
    switch(d){
        case up:
            if(getWorld()->canMoveTo(currX, currY+4) == canMove && !getWorld()->isDirtOverlap(currX, currY+4))
                return new Squirt(currX, currY+4, d, getWorld());
            break;
        case right:
            if(getWorld()->canMoveTo(currX+4, currY) == canMove && !getWorld()->isDirtOverlap(currX+4, currY))
               return new Squirt(currX+4, currY, d, getWorld());
            break;
        case down:
            if(getWorld()->canMoveTo(currX, currY-4) == canMove && !getWorld()->isDirtOverlap(currX, currY-4))
                return new Squirt(currX, currY - 4, d, getWorld());
            break;
        case left:
            if(getWorld()->canMoveTo(currX-4, currY) == canMove && !getWorld()->isDirtOverlap(currX-4, currY))
                return new Squirt(currX - 4, currY, d, getWorld());
            break;
        case none:
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
}


void Boulder::uniqueDoSomething(){
    if(m_state == BSTATE_STABLE){
        if(!isDirtBelow())
            m_state = BSTATE_WAITING;
        return;
    }
    else if(m_state == BSTATE_WAITING){
        m_nTicksWaited++;
        if(m_nTicksWaited >= 30){
            getWorld()->playSound(SOUND_FALLING_ROCK);
            m_state = BSTATE_FALLING;
        }
        return;
    }
    else if(m_state == BSTATE_FALLING){
        if(isDirtBelow())
            setDead();
        moveTo(getX(), getY()-1);
        if(getY() < 0)
            setDead();
        if(distance(getWorld()->getPlayer()) <= 3.00)
            getWorld()->getPlayer()->getAnnoyed(100);
        getWorld()->annoyProtestersNear(this, 100, getX(), getY(), 3.00);
        getWorld()->makeUpdateDistFromPlayer();
        getWorld()->makeUpdateDistFromExit();
    }
    return;
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

void Squirt::uniqueDoSomething(){
    if(getWorld()->annoyProtestersNear(this, 2, getX(), getY(), 3.00)){
        setDead();
        return;
    }
    else{
        if(m_squaresLeft <= 0){
            setDead();
            return;
        }
        m_squaresLeft--;
        Direction d = getDirection();
        int xCheck= getX();
        int yCheck = getY();
        giveNextLocInDir(d, xCheck, yCheck);
        if(getWorld()->canMoveTo(xCheck, yCheck) == canMove && !getWorld()->isDirtOverlap(xCheck, yCheck))
            moveTo(xCheck, yCheck);
        else setDead();
    }
    return;
}

/////////////////////////////////////////////////////////////////////////
Barrel::Barrel(int startX, int startY, StudentWorld* currWorld)
:Goodie(IID_BARREL, startX, startY, currWorld, false, GOODIE_PERMANENT, 1000, SOUND_FOUND_OIL){
    currWorld->addBarrel();
}

Barrel::~Barrel(){
    getWorld()->decBarrel();
}

void Barrel::getFound(){}


/////////////////////////////////////////////////////////////////////////
Nugget::Nugget(int startX, int startY, StudentWorld* currWorld, bool visibility, int permanence, int forWho)
:Goodie(IID_GOLD, startX, startY, currWorld, visibility, permanence, 10, SOUND_GOT_GOODIE, forWho)
{}

void Nugget::getFound(){
    getWorld()->getPlayer()->addGold();
}

void Nugget::goodieDoSomething(){
    if(forWhom() == PTESTOR_GOODIE){
        //Allows us to find a Protester
        Protester* prot = getWorld()->findProtesterNear(getX(), getY());
        if(prot != nullptr){
            setDead();
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
            prot->findGold();
        }
    }
    return;
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

Protester::Protester(int startX, int startY, StudentWorld* currWorld, int hitPoints, int imageID, bool isHardCore)
:Person(imageID, startX, startY, left, currWorld, hitPoints, SOUND_PROTESTER_GIVE_UP){
    m_stepsToKeepGoing = randInt(8, 60);
    m_leaveScreenState = false;
    int restTicks = 3 - (currWorld->getLevel() / 4);
    m_ticksToRest = max(0, restTicks);
    m_ticksRested = 0;
    m_ticksSinceYell = 15;
    m_ticksSincePerp = 200;
    m_isHardCore = isHardCore;
}

void Protester::uniqueDoSomething(){
    //Step 1:
    if(m_ticksToRest > m_ticksRested){
        m_ticksRested++;
        return;
    }
    
    //Initialize important variables
    m_ticksRested = 0;
    m_ticksSinceYell++;
    m_ticksSincePerp++;
    
    int currX = getX();
    int currY = getY();
    
    //Step 2:
    if(m_leaveScreenState){
        if(getX() == 60 && getY() == 60)
            setDead();
        else
            getWorld()->moveCloserTo(true, this);//Create a 2-D array/vector of DISTANCES away at each point.
        return;
    }
    
    //Step 4:
    if(canShoutAtFrackMan() && m_ticksSinceYell > 15){
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->getPlayer()->getAnnoyed(2);
        m_ticksSinceYell = 0;
        return;
    }
    
    //Step 5 for a hardCore Protester:
    //We only want to return if hardCoreDoSomething tells us to, which it never will for a regular protestor
    if(hardCoreDoSomething())
        return;
    
    //Step 5 (6 for HCP):
    else if(getWorld()->faceFrackMan(this)){
        Direction d = getDirection();
        giveNextLocInDir(d, currX, currY);
        moveTo(currX, currY);
        m_stepsToKeepGoing = 0;
        return;
    }
    
    //Step 6 (7 for HCP):
    m_stepsToKeepGoing--;
    int nextX = currX;
    int nextY = currY;
    Direction d;
    if(m_stepsToKeepGoing <= 0){
        do{
            nextX = currX;
            nextY = currY;
            int i = randInt(0, 3);
            switch(i){
                case 0: d = up; break;
                case 1: d = right; break;
                case 2: d = down; break;
                case 3: d = left; break;
                default: d = right; break;
            }
            giveNextLocInDir(d, nextX, nextY);
        }while(getWorld()->isDirtOverlap(nextX, nextY) || getWorld()->canMoveTo(nextX, nextY) != canMove);
        setDirection(d);
        m_stepsToKeepGoing = randInt(8, 60);
    }
    
    //Step 7 (8 for HCP):
    else if(m_ticksSincePerp >= 200 && canMovePerpInD(d)){
        setDirection(d);
        m_stepsToKeepGoing = randInt(8, 60);
        m_ticksSincePerp = 0;
    }
    
    //Step 8/9/10:
    giveNextLocInDir(getDirection(), nextX, nextY);
    if(getWorld()->isDirtOverlap(nextX, nextY) || getWorld()->canMoveTo(nextX, nextY) != canMove){
        m_stepsToKeepGoing = 0;
        return;
    }
    else{
        moveTo(nextX, nextY);
        return;
    }
}

bool Protester::canShoutAtFrackMan(){
    Direction d = getDirection();
    switch (d) {
        case up:
            if(getY() < getWorld()->getPlayer()->getY() && distance(getWorld()->getPlayer()) <= 4.0)
                return true;
            else return false;
            break;
            
        case right:
            if(getX() < getWorld()->getPlayer()->getX() && distance(getWorld()->getPlayer()) <= 4.0)
                return true;
            else return false;
            break;
            
        case down:
            if(getY() > getWorld()->getPlayer()->getY() && distance(getWorld()->getPlayer()) <= 4.0)
                return true;
            else return false;
            break;
            
        case left:
            if(getX() > getWorld()->getPlayer()->getX() && distance(getWorld()->getPlayer()) <= 4.0)
                return true;
            else return false;
            break;
            
        default:
            break;
    }
    return false;
}

void Protester::getStunned(){
    int t = 100 - (getWorld()->getLevel() * 10);
    int ticksToWait = max(50, t);
    int changeTicksRested = m_ticksToRest - ticksToWait;
    m_ticksRested = changeTicksRested;
}


void Protester::findGold(){
    getWorld()->increaseScore(25);
    m_leaveScreenState = true;
}

void Protester::hpRanOut(actorType annoyer){
    m_leaveScreenState = true;
    m_ticksRested = m_ticksToRest + 1;
    if(annoyer == squirt){
        if(m_isHardCore)
            getWorld()->increaseScore(250);
        else
            getWorld()->increaseScore(100);
    }
    else if(annoyer == boulder)
        getWorld()->increaseScore(500);
}

void Protester::wasAnnoyed(){
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    getStunned();
}

bool Protester::canMovePerpInD(Direction& d){
    int currX = getX();
    int currY = getY();
    if(getDirection() == left || getDirection() == right){
        bool canMoveUp = !getWorld()->isDirtOverlap(currX, currY+1) && getWorld()->canMoveTo(currX, currY+1) == canMove;
        bool canMoveDown = !getWorld()->isDirtOverlap(currX, currY-1) && getWorld()->canMoveTo(currX, currY-1) == canMove;
        if(canMoveUp){
            if(canMoveDown)
                d = (randInt(0, 1) ? up : down);
            else
                d = up;
            return true;
        }
        else if(canMoveDown){
            d = down;
            return true;
        }
    }
    else if(getDirection() == up || getDirection() == down){
        bool canMoveR = !getWorld()->isDirtOverlap(currX+1, currY) && getWorld()->canMoveTo(currX+1, currY) == canMove;
        bool canMoveL = !getWorld()->isDirtOverlap(currX-1, currY) && getWorld()->canMoveTo(currX-1, currY) == canMove;
        if(canMoveR){
            if(canMoveL)
                d = (randInt(0, 1) ? left : right);
            else
                d = right;
            return true;
        }
        else if(canMoveL){
            d = left;
            return true;
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////
HardCoreProtester::HardCoreProtester(int startX, int startY, StudentWorld* currWorld)
:Protester(startX, startY, currWorld, 20, IID_HARD_CORE_PROTESTER, false){
    m_cellPhoneRad = 16 + currWorld->getLevel()/2;
}

bool HardCoreProtester::hardCoreDoSomething(){
    if(getWorld()->numStepsFromPlayer(getX(), getY()) < m_cellPhoneRad){
        getWorld()->moveCloserTo(false, this);
        return true;
    }
    return false;
}

void HardCoreProtester::findGold(){
    getWorld()->increaseScore(50);
    getStunned();
}
