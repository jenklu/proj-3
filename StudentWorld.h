#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Protester;
class FrackMan;
class Dirt;
enum moveStatus{ edgeBlocked, boulderBlocked, canMove };
enum oneOrAll{ one, all };
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
    
    inline bool isDirtAt(int x, int y) { return m_dirtArr[x][y] != nullptr; }
    
    inline FrackMan* getPlayer() { return m_player; }
    
    inline void decBarrel() { m_nBarrels--; }
    
    inline void addBarrel() { m_nBarrels++; }
    
    inline void addToWorld(Actor* newActor) { m_actors.push_back(newActor); }
    
    bool isDirtOverlap(int x, int y) const;
    
    moveStatus canMoveTo(int x, int y);
    
    void setRadiusVisible(double x, double y, double rad);
    
    //If there are no protesters within the radius of x&y, does nothing & returns nullptr. If annoyHowMany is one, the function annoys the first protester in the list of protesters by pointsToAnnoy points and returns a pointer to that protester. If annoyHowMany is all, the function annoys all the protesters and returns a pointer to the last protester in the array
    Protester* annoyProtestersNear(oneOrAll annoyHowMany, int pointsToAnnoy, double x, double y, double rad = 3.0);
    
    bool faceFrackMan(Actor* setFacing);
    
    void xAndYtoLeave(int& x, int& y);
    
private:
    void setDisplayText();
    
    void addActors();
    
    void addProtester();
    
    void addGoodie();
    
    inline void addFrontChar(char c, std::string& s, int desLength) const;
    
    void canAddHere(int& x, int& y, bool isBoulder);
    
    bool distanceGreaterThan6(int x, int y);
    
    bool isBoulderBlocking(int x, int y);
    
    FrackMan* m_player;
    
    Dirt* m_dirtArr[VIEW_WIDTH][VIEW_HEIGHT];
    
    struct distFromExit{
        int distance;
        bool beenMarked;
    };
    
    distFromExit* howFarFromExit[VIEW_WIDTH-4][VIEW_HEIGHT-4];
    
    std::list<Actor*> m_actors;
    
    int m_nBarrels, m_nProtesters, m_nTicksSinceLastProtest;
};

//Does not access any member variables, so should not be a member function - in this header so it can be accessed by Actor.cpp
int randInt(int min, int max);

#endif // STUDENTWORLD_H_
