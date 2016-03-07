#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <queue>
class StudentWorld;

const int BSTATE_STABLE = 0;
const int BSTATE_WAITING = 1;
const int BSTATE_FALLING = 2;

const int GOODIE_PERMANENT = 1;
const int GOODIE_TEMP = 2;
const int FMAN_GOODIE = 3;
const int PTESTOR_GOODIE = 4;

const bool MOVETO_EXIT = true;
const bool MOVETO_PLAYER = false;

enum actorType {
    boulder, protester, squirt, other
};

void giveNextLocInDir(GraphObject::Direction d, int& x, int& y); //Declared outside of Actor so it can also be used by StudentWorld, because it actually has nothing to do with member variables
/////////////////////////////////////////////////////////////////////////
class Actor : public GraphObject{
 public:
    Actor(int imageID, int startX, int startY, Direction dir, StudentWorld* currWorld, double size = 1.0, unsigned int depth = 0);
    
    virtual ~Actor() {}
    
    void doSomething();
    
    double distance(Actor* check) const;
    
    double distance(int x, int y) const;
    
    virtual void uniqueDoSomething() = 0;
    
    inline bool isAlive() const { return m_alive; }
    
    inline void setDead() { m_alive = false; }
    
    inline StudentWorld* getWorld() const { return m_currWorld; }
    
    inline actorType getType() const { return m_type; }
    
 private:
    StudentWorld* m_currWorld;
    bool m_alive;
    actorType m_type;
};

/////////////////////////////////////////////////////////////////////////
class Person : public Actor{
 public:
    Person(int imageID, int startX, int startY,  Direction dir, StudentWorld* currWorld, int hitPoints, int giveUpSound, double size = 1.0, unsigned int depth = 0);
    
    virtual ~Person() {}
    
    void getAnnoyed(int toSub, actorType annoyer = other);
    
    virtual void hpRanOut(actorType annoyer) = 0;
    
    inline int getHP() const { return m_hitPoints; }
    
    inline virtual void wasAnnoyed() {}
    
 private:
    int m_hitPoints, m_giveUpSound;
};

/////////////////////////////////////////////////////////////////////////
class Goodie : public Actor{
 public:
    Goodie(int imageID, int startX, int startY, StudentWorld* currWorld, bool isVisible, int permanence, int pointValue, int soundID = SOUND_GOT_GOODIE, int forwho = FMAN_GOODIE);
    
    virtual ~Goodie() {}
    
    void uniqueDoSomething();
    
    virtual void getFound() = 0;
    
    inline int forWhom() { return m_forWhom; }
    
    virtual void goodieDoSomething() { return; }
 private:
    int m_permanence, m_forWhom, m_pointValue, m_soundID, m_ticksLeft;
};

/////////////////////////////////////////////////////////////////////////
class FrackMan : public Person{
 public:
    FrackMan(StudentWorld* currWorld);
    
    ~FrackMan() {}
    
    void uniqueDoSomething();
    
    inline void addFiveSquirts() { m_squirts += 5; }
    
    inline void addGold() { m_nuggets++; }
    
    inline void addSonar() { m_sonar++; }
    
    inline int getSquirts() const { return m_squirts; }
    
    inline int getSonar() const { return m_sonar; }
    
    inline int getNuggets() const { return m_nuggets; }
    
    inline void hpRanOut(actorType annoyer) { setDead(); }
    
 private:
    Actor* createSquirt();
    int m_squirts, m_sonar, m_nuggets;
};

/////////////////////////////////////////////////////////////////////////
class Dirt : public GraphObject{
 public:
    Dirt(int startX, int startY)
    :GraphObject(IID_DIRT, startX, startY, right, .25, 3){
        setVisible(true);
    }
    
    ~Dirt() {}
};

/////////////////////////////////////////////////////////////////////////
class Boulder : public Actor{
 public:
    Boulder(int startX, int startY, StudentWorld* currWorld);
    
    ~Boulder() {}
    
    void uniqueDoSomething();
    
 private:
    bool isDirtBelow();
    int m_state, m_nTicksWaited;
};

#endif // ACTOR_H_

/////////////////////////////////////////////////////////////////////////
class Squirt : public Actor{
 public:
    Squirt(int startX, int startY, Direction dir, StudentWorld* currWorld);
    
    ~Squirt() {}
    
    void uniqueDoSomething();
    
 private:
    int m_squaresLeft;
};

/////////////////////////////////////////////////////////////////////////
class Barrel : public Goodie{
 public:
    Barrel(int startX, int startY, StudentWorld* currWorld);
    
    ~Barrel();
    
    void getFound();
 
};

/////////////////////////////////////////////////////////////////////////
class Nugget : public Goodie{
 public:
    Nugget(int startX, int startY, StudentWorld* currWorld, bool visibility, int permanence, int forwho);
    
    ~Nugget() {}
    
    void getFound();
    
    virtual void goodieDoSomething();
};

/////////////////////////////////////////////////////////////////////////
class SonarKit : public Goodie{
 public:
    SonarKit(int startX, int startY, StudentWorld* currWorld);
    
    ~SonarKit() {}
    
    void getFound();
};

/////////////////////////////////////////////////////////////////////////
class WaterPool : public Goodie{
 public:
    WaterPool(int startX, int startY, StudentWorld* currWorld);
    
    ~WaterPool() {}
    
    void getFound();
};

/////////////////////////////////////////////////////////////////////////
class Protester : public Person{
 public:
    Protester(int startX, int startY, StudentWorld* currWorld, int hitPoints = 5, int imageID = IID_PROTESTER, bool isHardCore = false);
    ~Protester() {}
    
    void uniqueDoSomething();
    
    virtual void findGold();
    
    void hpRanOut(actorType annoyer);
    
    virtual void wasAnnoyed();
    
    inline bool canBeAnnoyed() { return !m_leaveScreenState; }
    
 protected:
    void getStunned();
    
    inline virtual bool hardCoreDoSomething() { return false; }
    
 private:
    bool canShoutAtFrackMan();
    
    bool canMovePerpInD(Direction& d);
    
    bool m_leaveScreenState, m_isHardCore;
    int m_stepsToKeepGoing;
    int m_ticksToRest, m_ticksRested, m_ticksSinceYell, m_ticksSincePerp;
    
};

/////////////////////////////////////////////////////////////////////////
class HardCoreProtester : public Protester{
 public:
   // HardCoreProtester
    HardCoreProtester(int startX, int startY, StudentWorld* currworld);
    
    virtual bool hardCoreDoSomething();
    
    virtual void findGold();
    
 private:
    int m_cellPhoneRad;
    
};