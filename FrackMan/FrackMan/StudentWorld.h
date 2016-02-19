#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
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
    
    void killPlayer();
private:
    inline void addFrontChar(char c, std::string& s, int desLength);
    FrackMan* player;
    Dirt* dirtArr[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // STUDENTWORLD_H_
