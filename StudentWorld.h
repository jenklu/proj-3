#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class FrackMan;
class Dirt;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
	}
    
    ~StudentWorld();
    
    virtual int init();

    virtual int move();

    virtual void cleanUp();
    
    bool eraseDirt(int startX, int startY, int endX, int endY);
    
    inline bool isDirtAt(int x, int y) { return dirtArr[x][y] != nullptr; }
    
    void setDisplayText();
    
    bool isActorAt(int imageID, int x, int y);
    
    inline FrackMan* getPlayer() { return player; }
    
    inline void decBarrel() { m_nBarrels--; }
    
    inline void addBarrel() { m_nBarrels++; }
    
    inline void addToWorld(Actor* newActor) { actors.emplace_back(newActor); }
    
    bool isDirtOverlap(int x, int y);
    
private:
    void addActors();
    
    void addProtestor();
    
    void addGoodie();
    
    inline void addFrontChar(char c, std::string& s, int desLength);
    
    void canAddHere(int& x, int& y);
    
    FrackMan* player;
    
    Dirt* dirtArr[VIEW_WIDTH][VIEW_HEIGHT];
    
    std::list<Actor*> actors;
    
    int m_nBarrels, m_nProtestors, m_nTicksSinceLastProtest;
};

#endif // STUDENTWORLD_H_
