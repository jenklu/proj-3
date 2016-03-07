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
class GraphObject;
enum moveStatus{ edgeBlocked, boulderBlocked, canMove };
enum distanceFromWhere { topright, player};

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
    
    bool isDirtOverlap(int x, int y) const;
    
    moveStatus canMoveTo(int x, int y);
    
    void setRadiusVisible(double x, double y, double rad);
    
    //If there are no protesters within the radius of x&y, does nothing & returns nullptr.  Annoyer is passed as a pointer so that it's type can be deduced within the implementation of the function and passed to the getAnnoyed function. This is necessary because certain cases of being annoyed for protesters require knowing what is annoying the protester
    bool annoyProtestersNear(Actor* annoyer, int pointsToAnnoy, double x, double y, double rad = 3.0);
    
    Protester* findProtesterNear(double x, double y);
    
    int numStepsFromPlayer(int x, int y);
    
    bool faceFrackMan(Actor* setFacing);
    
    void moveCloserTo(bool movingToExit, Actor* looking);
    
    inline void makeUpdateDistFromPlayer() { m_shouldUpdateDistPlayer = true; }
    
    inline void makeUpdateDistFromExit() { m_shouldUpdateDistExit = true; }

    inline bool isDirtAt(int x, int y) { return m_dirtArr[x][y] != nullptr; }
    
    inline FrackMan* getPlayer() { return m_player; }
    
    inline void decBarrel() { m_nBarrels--; }
    
    inline void addBarrel() { m_nBarrels++; }
    
    inline void addToWorld(Actor* newActor) { m_actors.push_back(newActor); }
    
    
    
private:
    void setDisplayText();
    
    inline void addFrontChar(char c, std::string& s, int desLength) const;
    
    void addActors();
    
    void addProtester();
    
    void addGoodie();
    
    bool isBoulderBlocking(int x, int y);
    
    void canAddHere(int& x, int& y, bool isBoulder);
    
    bool isEmptyLoc(int x, int y);
    
    bool leavingProtesterCanGoTo(int x, int y, bool movingToExit, int currMinSteps);
    
    bool distanceGreaterThan6(int x, int y);
    
    void fillDistFrom(distanceFromWhere from);
    
    int getStepsFrom(bool exit, int x, int y);
    
    bool areProtestersLeaving();
    
    FrackMan* m_player;
    
    Dirt* m_dirtArr[VIEW_WIDTH][VIEW_HEIGHT];
    
    struct distFrom{
        distFrom(int fromPlayer, int fromExit, bool marked)
        :fromPlayer(fromPlayer), fromExit(fromExit), beenMarked(marked) {}
        int fromPlayer, fromExit;
        bool beenMarked;
    };
    
    struct distAndLoc{
        distAndLoc(int x, int y, int dist)
        :m_x(x), m_y(y), m_dist(dist) {}
        int m_x, m_y, m_dist;
    };
    
    distFrom* howFarFrom[VIEW_WIDTH-3][VIEW_HEIGHT-3];
    
    std::list<Actor*> m_actors;
    
    int m_nBarrels, m_nProtesters, m_nTicksSinceLastProtest, m_maxProtesters;
    
    bool m_shouldUpdateDistExit, m_shouldUpdateDistPlayer;
};

//Does not access any member variables, so should not be a member function - in this header so it can be accessed by Actor.cpp
int randInt(int min, int max);

#endif // STUDENTWORLD_H_
