#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
const int BSTATE_STABLE = 0;
const int BSTATE_WAITING = 1;
const int BSTATE_FALLING = 2;

/////////////////////////////////////////////////////////////////////////
class Actor : public GraphObject{
 public:
    Actor(int imageID, int startX, int startY, Direction dir, StudentWorld* currWorld, double size = 1.0, unsigned int depth = 0);
    
    virtual int doSomething() = 0;
    
    inline bool isAlive() { return m_alive; }
    
    inline void setDead() { m_alive = false; }
    
    inline StudentWorld* getWorld() { return m_currWorld; }
    
    bool overlaps(Actor* check);
 private:
    StudentWorld* m_currWorld;
    bool m_alive;
};

/////////////////////////////////////////////////////////////////////////
class Person : public Actor{
 public:
    Person(int imageID, int startX, int startY,  Direction dir, StudentWorld* currWorld, int hitPoints, double size = 1.0, unsigned int depth = 0);
    
    inline void subtractPoints(int toSub) { m_hitPoints -= toSub; }
    
    virtual void getAnnoyed() = 0;
    
    int getHP();
 private:
    int m_hitPoints;
};

/////////////////////////////////////////////////////////////////////////
class FrackMan : public Person{
 public:
    FrackMan(StudentWorld* currWorld);
    
    int doSomething();
    
    inline int getSquirts() { return m_squirts; }
    
    inline int getCharges() { return m_charges; }
    
    inline int getNuggets() { return m_nuggets; }
    
    void getAnnoyed();
 private:
    int m_squirts, m_charges, m_nuggets;

};

/////////////////////////////////////////////////////////////////////////
class Dirt : public GraphObject{
 public:
    Dirt(int startX, int startY)
    :GraphObject(IID_DIRT, startX, startY, right, .25, 3){
        setVisible(true);
    }
};

/////////////////////////////////////////////////////////////////////////
class Boulder : public Actor{
 public:
    Boulder(int startX, int startY, StudentWorld* currWorld);
    
    int doSomething();
 private:
    bool isDirtBelow();
    int m_state, m_nTicksWaited;
};

#endif // ACTOR_H_
