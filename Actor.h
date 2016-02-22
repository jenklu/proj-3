#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
class StudentWorld;
const int BSTATE_STABLE = 0;
const int BSTATE_WAITING = 1;
const int BSTATE_FALLING = 2;

const int GOODIE_PERMANENT = 1;
const int GOODIE_TEMP = 2;
const int FMAN_GOODIE = 3;
const int PTESTOR_GOODIE = 4;

/////////////////////////////////////////////////////////////////////////
class Actor : public GraphObject{
 public:
    Actor(int imageID, int startX, int startY, Direction dir, StudentWorld* currWorld, double size = 1.0, unsigned int depth = 0);
    
    ~Actor() {}
    
    virtual int doSomething();
    
    virtual int uniqueDoSomething() = 0;
    
    inline bool isAlive() const { return m_alive; }
    
    inline void setDead() { m_alive = false; }
    
    inline StudentWorld* getWorld() const { return m_currWorld; }
    
    double distance(Actor* check) const;
    
    double distance(int x, int y) const;
    
 private:
    StudentWorld* m_currWorld;
    bool m_alive;
};

/////////////////////////////////////////////////////////////////////////
class Person : public Actor{
 public:
    Person(int imageID, int startX, int startY,  Direction dir, StudentWorld* currWorld, int hitPoints, double size = 1.0, unsigned int depth = 0);
    
    ~Person() {}
    
    void getAnnoyed(int toSub);
    
    virtual void playGiveUpSound() const = 0;
    
    inline int getHP() const { return m_hitPoints; }
    
 private:
    int m_hitPoints;
};

/////////////////////////////////////////////////////////////////////////
class Goodie : public Actor{
 public:
    Goodie(int imageID, int startX, int startY, StudentWorld* currWorld, bool isVisible, int permanence, int pointValue, int soundID = SOUND_GOT_GOODIE, int forwho = FMAN_GOODIE);
    
    ~Goodie() {}
    
    int uniqueDoSomething();
    
    virtual void getFound() = 0;
    
    virtual int goodieDoSomething();
    
 private:
    int m_permanence, m_forwhom, m_pointValue, m_soundID, m_ticksLeft;
};

/////////////////////////////////////////////////////////////////////////
class FrackMan : public Person{
 public:
    FrackMan(StudentWorld* currWorld);
    
    ~FrackMan() {}
    
    int uniqueDoSomething();
    
    inline void addFiveSquirts() { m_squirts += 5; }
    
    inline void addGold() { m_nuggets++; }
    
    inline void addSonar() { m_sonar++; }
    
    inline int getSquirts() const { return m_squirts; }
    
    inline int getSonar() const { return m_sonar; }
    
    inline int getNuggets() const { return m_nuggets; }
    
    void playGiveUpSound() const;
    
    void addBoulderLoc(int x, int y);
    
    bool canMoveTo(int x, int y);
    
 private:
    Actor* createSquirt();
    int m_squirts, m_sonar, m_nuggets;
    struct Coord{
        int x, y;
    };
    std::vector<Coord> boulderLocs;
    
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
    
    int uniqueDoSomething();
    
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
    
    int uniqueDoSomething();
    
 private:
    int m_squaresLeft;
};

/////////////////////////////////////////////////////////////////////////
class Barrel : public Goodie{
 public:
    Barrel(int startX, int startY, StudentWorld* currWorld);
    
    ~Barrel() {}
    
    void getFound();
    
    int goodieDoSomething();
 
};

/////////////////////////////////////////////////////////////////////////
class Nugget : public Goodie{
 public:
    Nugget(int startX, int startY, StudentWorld* currWorld, bool visibility, int permanence, int forwho);
    
    ~Nugget() {}
    
    void getFound();
    
    int goodieDoSomething();
    
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
class Protestor : public Person{
 public:
    Protestor(int startX, int startY, StudentWorld* currWorld, int hitPoints = 5, int imageID = IID_PROTESTER);
    
    int uniqueDoSomething();
    
    virtual int hardCoreDoSomething();
    
    void playGiveUpSound() const;
    
 private:
    bool m_leaveScreenState, m_restState;
    int m_stepsToKeepGoing;
    int m_ticksWaited;
};

/////////////////////////////////////////////////////////////////////////
class HardCoreProtestor : public Protestor{
 public:
   // HardCoreProtestor
    HardCoreProtestor(int startX, int startY, StudentWorld* currworld);
};