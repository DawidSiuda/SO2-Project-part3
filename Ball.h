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
#include <condition_variable>

#include "structures.h"
#include "Bat.h"

class Ball 
{
 public:
    Ball(float posX, float posY, float r, float movX = 0, float movY = 0);


    float getX()
    {
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
        condition_variableFreeze.notify_all();
    }

    void setFrozze()
    {
        isFrozen.store(true);
        condition_variableFreeze.notify_all();
    }

    void setDefrozze()
    {
        isFrozen.store(false);
        condition_variableFreeze.notify_all();
    }

    int calculateNevCoordinate(const std::atomic<bool> * pause,
                                     const Bat * const leftBat,
                                        const Bat * const rightBat,
                                            std::atomic<int> * const leftScore,
                                                std::atomic<int> * const rightScore);

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

    std::condition_variable condition_variableFreeze;
};

#endif //BALL_H