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

    const Color * const getColor()
    {
        return &color;
    }

    bool getFrozeStatus()
    {
        return isFrozen.load();
    }

    void setEndLoop()
    {
        end = true;
    }

    void setFrozze()
    {
        isFrozen.store(true);
    }

    void setDefrozze()
    {
        isFrozen.store(false);
    }

    int calculateNevCoordinate(const std::atomic<bool> * const pause);

    static int handleCillizion(Ball *firstBall, Ball *secondBall);

private:

    bool end;

    float r;

    int verticalDirect;

    static std::once_flag onceFlagRandSeedForBall;

    std::atomic<bool> isMovingY;
    std::atomic<bool> isMovingX;
    std::atomic<bool> isFrozen;

    Color color;

    vector2d position;
    vector2d directionVector;

    void setRandomDirectionVertex();

    std::mutex mutexChangingDirectionVector;
};

#endif //BALL_H