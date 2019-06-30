#ifndef BALL_H
#define BALL_H

#include <iostream>
#include <stdlib.h>
#include <time.h>
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

    float getX() const;
    float getY() const;
    float getR() const;
    const Color * const getColor() const;
    bool getFrozeStatus() const;
    void setEndLoop();
    void setFrozze();
    void setDefrozze();

    //
    // Function "calculateNevCoordinate "should be run in new thread.
    // It's responsible for calculate new coordinates of ball
    // for every frame and it ends when " bool end" storages true. 
    //

    void calculateNevCoordinate(const std::atomic<bool> * pause,
                                     const Bat * const leftBat,
                                        const Bat * const rightBat,
                                            std::atomic<int> * const leftScore,
                                                std::atomic<int> * const rightScore);

    static void handleCillizion(Ball *firstBall, Ball *secondBall);

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
    std::mutex mutexChangingDirectionVector;
    std::condition_variable condition_variableFreeze;

    void setRandomDirectionVertex();
};

#endif //BALL_H