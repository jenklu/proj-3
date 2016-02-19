#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, int startX, int startY, Direction dir, StudentWorld* currWorld, double size, unsigned int depth)
:GraphObject(imageID, startX, startY, dir, size, depth)
{
    m_currWorld = currWorld;
    m_alive = true;
    setVisible(true);
}

bool Actor::overlaps(Actor* check){
    double dxdy = pow(static_cast<double>(getX() - check->getX()), 2.0) + pow(static_cast<double>(getY() - check->getY()), 2.0);
    double distance = sqrt(dxdy);
    if(distance < 3)
        return true;
    return false;
}

/////////////////////////////////////////////////////////////////////////
Person::Person(int imageID, int startX, int startY,  Direction dir, StudentWorld* currWorld, int hitPoints, double size, unsigned int depth)
:Actor(imageID, startX, startY, dir, currWorld, size, depth){
    m_hitPoints = hitPoints;
}

int Person::getHP(){
    return m_hitPoints;
}

/////////////////////////////////////////////////////////////////////////
FrackMan::FrackMan(StudentWorld* currWorld)
:Person(IID_PLAYER, 30, 60, right, currWorld, 10, 1, 0){
    m_squirts = 5;
    m_charges = 1;
    m_nuggets = 0;
}

int FrackMan::doSomething(){
    int move;
    if(!isAlive())
        return GWSTATUS_PLAYER_DIED;
    else if(getWorld()->eraseDirt(getX(), getY(), getX()+3, getY()+3)){
        getWorld()->playSound(SOUND_DIG);
        return GWSTATUS_CONTINUE_GAME;
    }
    else if(getWorld()->getKey(move)){
        switch (move) {
            case KEY_PRESS_UP:
                if(getDirection() != up)
                    setDirection(up);
                else if(getY() < VIEW_HEIGHT - 4)
                    moveTo(getX(), getY()+1);
                else
                    moveTo(getX(), getY());
                break;
                
            case KEY_PRESS_RIGHT:
                if(getDirection() != right)
                    setDirection(right);
                else if(getX() < VIEW_WIDTH - 4)
                    moveTo(getX()+1, getY());
                else
                    moveTo(getX(), getY());
                break;
                
            case KEY_PRESS_DOWN:
                if(getDirection() != down)
                    setDirection(down);
                else if(getY() > 0)
                    moveTo(getX(), getY() - 1);
                else
                    moveTo(getX(), getY());
                break;
                
            case KEY_PRESS_LEFT:
                if(getDirection() != left)
                    setDirection(left);
                else if(getX() > 0)
                    moveTo(getX()-1, getY());
                else
                    moveTo(getX(), getY());
                break;
            default:
                break;
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

void FrackMan::getAnnoyed(){
    subtractPoints(2);
    if(getHP() <= 0){
        setDead();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

/////////////////////////////////////////////////////////////////////////
Boulder::Boulder(int startX, int startY, StudentWorld* currWorld)
:Actor(IID_BOULDER, startX, startY, down, currWorld, 1, 1){
    m_state = BSTATE_STABLE;
    getWorld()->eraseDirt(startX, startY, startX+3, startY+3);
    m_nTicksWaited = 0;
}


int Boulder::doSomething(){
    if(!isAlive())
        return 0;
    if(m_state == BSTATE_STABLE){
        if(!isDirtBelow())
            m_state = BSTATE_WAITING;
        return 1;
    }
    else if(m_state == BSTATE_WAITING){
        m_nTicksWaited++;
        if(m_nTicksWaited >= 30)
            m_state = BSTATE_FALLING;
        return 1;
    }
    else if(m_state == BSTATE_FALLING){
        if(isDirtBelow())
            setDead();
        moveTo(getX(), getY()-1);
        if(getY() < 0)
            setDead();
        
    }
    return 1;
}

bool Boulder::isDirtBelow(){
    for(int i = 0; i<4; i++){
        if(getWorld()->isDirtAt(getX()+i, getY()-1))
            break;
        if(i == 4)
            return false;
    }
    return true;
}


