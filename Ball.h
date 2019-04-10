#ifndef BALL_H
#define BALL_H

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>

#include "structures.h"

class Ball 
{
 public:
    Ball(float posX, float posY, float r, float movX = 0, float movY = 0);


    float getX()
    {
        
        //std::cout << "GET " << this << " | " << possitX << " | " << possitY << "\n";
        return possitX;
        // return position.x;
        //return positionX.load();
    }

    float getY()
    {
        return possitY;
        // return position.y;
        //return positionY.load();
    }

    float getR()
    {
        return r;
    }

    void setEndLoop()
    {
        end = true;
    }

    int calculateNevCoordinate();

private:
    bool end;
    float possitX;
    float possitY;
    vector2d position;
    vector2d directionVector;
    double verticalDirect;
    double horizontalDirect;
    int isMovingY;
    int isMovingX;
    float r;

    static bool setSeed;
    static void setRandomDirectionVertex(Ball *ball);
};

#endif //BALL_H