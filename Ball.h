#ifndef BALL_H
#define BALL_H

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

#include "structures.h"

class Ball 
{
 public:
    Ball(float posX, float posY, float r, float movX = 0, float movY = 0);


    float getX()
    {
        
        //std::cout << "GET " << this << " | " << possitX << " | " << possitY << "\n";
        // return possitX;
        return position.x;
        //return positionX.load();
    }

    float getY()
    {
        // return possitY;
         return position.y;
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

    const Color * const getColor()
    {
        return &color;
    }

    int calculateNevCoordinate(const std::atomic<bool> * const pause);

private:

    bool end;

    // int isMovingY;
    // int isMovingX;

    float r;

    double verticalDirect;

    static std::once_flag onceFlagRandSeedForBall;

    std::atomic<bool> isMovingY;
    std::atomic<bool> isMovingX;

    Color color;

    vector2d position;
    vector2d directionVector;

    void setRandomDirectionVertex();
};

#endif //BALL_H