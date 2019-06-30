#include "Ball.h"

std::once_flag Ball::onceFlagRandSeedForBall;

Ball::Ball(float posX, float posY, float r, float movX, float movY) : position(posX, posY),
                                                                      directionVector(movX, movY),
                                                                      isMovingY(true),
                                                                      isMovingX(true),
                                                                      isFrozen(false)
{
    std::call_once(Ball::onceFlagRandSeedForBall, []{srand(time(NULL)); std::cout << "--> Log: Set seed for Ball\n";});

    position.x = posX;
    position.y = posY;

    end = false;

    this->r = r;
    verticalDirect = 1;

    if (movX == 0 and movY == 0)
    {
        Ball::setRandomDirectionVertex();
    }

    color.r = rand() %100;
    color.g = rand() %100;
    color.b = rand() %100;
}

float Ball::getX() const
{
    return position.x;
}

float Ball::getY() const
{
    return position.y;
}

float Ball::getR() const
{
    return r;
}

const Color * const Ball::getColor() const
{
    return &color;
}

bool Ball::getFrozeStatus() const
{
    return isFrozen.load();
}

void Ball::setEndLoop()
{
    end = true;
    condition_variableFreeze.notify_all();
}

void Ball::setFrozze()
{
    isFrozen.store(true);
    condition_variableFreeze.notify_all();
}

void Ball::setDefrozze()
{
    isFrozen.store(false);
    condition_variableFreeze.notify_all();
}

void Ball::calculateNevCoordinate( const std::atomic<bool> * pause,
                                     const Bat * const leftBat,
                                        const Bat * const rightBat,
                                            std::atomic<int> * const leftScore,
                                                std::atomic<int> * const rightScore)
{
    while (end == false)
    {
        {
            std::unique_lock<std::mutex> uniqueLockMutexChangingDirectionVector(mutexChangingDirectionVector);

            if(pause->load() == false && isFrozen.load() == false)
            {
                vector2d tempPosition = position;

                if (isMovingY.load() == true)
                {  
                    //
                    // Check colizion of top and bottom wall.
                    //

                    if ((tempPosition.y + r) > TOP_LINE_Y)
                    {
                        //
                        // Align the position of ball.
                        //

                        tempPosition.y = TOP_LINE_Y - r;

                        //
                        // Change direction.
                        //

                        verticalDirect = -1;
                    }
                    else if ((tempPosition.y - r) < BOTTOM_LINE_Y)
                    {
                        //
                        // Align the position of ball.
                        //

                        tempPosition.y = BOTTOM_LINE_Y + r;

                        //
                        // Change direction.
                        //

                        verticalDirect = 1;
                    }

                    //
                    // Calculate new coordinates for Y.
                    //
                    tempPosition.y += (0.001 * directionVector.y * directionVector.y * verticalDirect);
                }

                if (isMovingX.load() == true)
                {
                    //
                    // Check collision of right and left wall.
                    //

                    if ((tempPosition.x + r) > 1)
                    {
                        if(rightBat->getBottomEdgeYPossition() < tempPosition.y &&
                            (rightBat->getBottomEdgeYPossition() + rightBat->getLength()) > tempPosition.y)
                        {
                            tempPosition.x = 1.0 - r;
                            directionVector.x *= -1.0;
                        }
                        else
                        {
                            tempPosition.x = 2;
                            end = true;
                            leftScore->store(leftScore->load()+1);
                        }
                    }
                    else if ((tempPosition.x - r) < -1)
                    {
                        if(leftBat->getBottomEdgeYPossition() < tempPosition.y &&
                            (leftBat->getBottomEdgeYPossition() + leftBat->getLength()) > tempPosition.y)
                        {
                            tempPosition.x = -1.0 + r;
                            directionVector.x *= -1.0;
                        }
                        else
                        {
                            tempPosition.x = -2;
                            end = true;
                            rightScore->store(rightScore->load()+1);   
                        }
                    }

                    //
                    // Calculate new coordinates for X.
                    //

                    tempPosition.x += (0.005 * directionVector.x);
                }

                //
                // Switch coordinates.
                //

                position = tempPosition;
            }
            else
            {
                condition_variableFreeze.wait(uniqueLockMutexChangingDirectionVector, [this]{return (this->isFrozen.load() == false || end == true);});
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
    }
}

void Ball::handleCillizion(Ball *firstBall, Ball *secondBall)
{
    if(firstBall->isFrozen.load() == false)
    {
        firstBall->setFrozze();// isFrozen.store(true);
        secondBall->setDefrozze();// isFrozen.store(false);

        std::lock_guard<std::mutex> lg4(secondBall->mutexChangingDirectionVector);

        //
        // X
        //

        if(firstBall->directionVector.x > 0)
        {
            // First -->
            if(secondBall->directionVector.x > 0)
            {
                // Second -->
                if(secondBall->position.x < firstBall->position.x)
                {
                    secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                    secondBall->position.x = (firstBall->position.x - firstBall->r) - secondBall->r;
                }
                else 
                {
                    secondBall->directionVector.x = secondBall->directionVector.x + firstBall->directionVector.x;
                    secondBall->position.x = (firstBall->position.x + firstBall->r) + secondBall->r;
                }
            }
            else
            {
                // Second <--
                secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                secondBall->position.x = (firstBall->position.x + firstBall->r) + secondBall->r;
            }
        }
        else
        {
            // First <--
            if(secondBall->directionVector.x >= 0)
            {  
                    secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                    secondBall->position.x = (firstBall->position.x - firstBall->r) - secondBall->r;
            }
            else
            {
                // Second <--
                if(secondBall->position.x < firstBall->position.x)
                {
                    secondBall->directionVector.x = (secondBall->directionVector.x + firstBall->directionVector.x);
                    secondBall->position.x = (firstBall->position.x - firstBall->r) - secondBall->r;
                }
                else
                {
                    secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                    secondBall->position.x = (firstBall->position.x + firstBall->r) + secondBall->r;
                }
            }
        }

        //
        // Y
        //

        if(firstBall->verticalDirect > 0)
        {
            // First ^
            if(secondBall->verticalDirect > 0)
            {
                // Second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // Firsrt
                    // Second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
                else
                {
                    // Second
                    // First
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = 1;
                    secondBall->position.y = (firstBall->position.y + firstBall->r) + secondBall->r;
                }
            }
            else
            {
                // Second v
                secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                secondBall->verticalDirect = 1;
            }
        }
        else
        {
            // First v
            if(secondBall->verticalDirect > 0)
            {
                // second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // Firsrt
                    // Second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
            }
            else
            {
                // Second v

                if(secondBall->position.y < firstBall->position.y)
                {
                    // Firsrt
                    // Second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
                else
                {
                    // second
                    // first
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = 1;
                    secondBall->position.y = (firstBall->position.y + firstBall->r) + secondBall->r;
                }
            }

        }

        #ifdef SET_NULL_AFTER_COLLIZION
        {
            std::lock_guard<std::mutex> lg3(firstBall->mutexChangingDirectionVector);
            firstBall->directionVector.x = 0.1;
            firstBall->directionVector.y = 0.1;
        }
        #endif // SET_NULL_AFTER_COLLIZION
    }
    else
    {
        //
        // Swap names of ball.
        //

        Ball *tmp = firstBall;
        firstBall = secondBall;
        secondBall = tmp;

        //
        // Handle colizion.
        //

        firstBall->setFrozze();// isFrozen.store(true);
        secondBall->setDefrozze();// isFrozen.store(false);

        std::lock_guard<std::mutex> lg1(secondBall->mutexChangingDirectionVector);

        //
        // X
        //

        if(firstBall->directionVector.x > 0)
        {
            // First -->
            if(secondBall->directionVector.x > 0)
            {
                if(secondBall->position.x < firstBall->position.x)
                {
                    secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                    secondBall->position.x = (firstBall->position.x - firstBall->r) - secondBall->r;
                }
                else
                {
                    secondBall->directionVector.x = secondBall->directionVector.x + firstBall->directionVector.x;
                    secondBall->position.x = (firstBall->position.x + firstBall->r) + secondBall->r;
                }
            }
            else
            {
                secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                secondBall->position.x = (firstBall->position.x + firstBall->r) + secondBall->r;
            }
        }
        else
        {
            // First <--
            if(secondBall->directionVector.x > 0)
            {  
                // Second -->
                secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                secondBall->position.x = (firstBall->position.x - firstBall->r) - secondBall->r;
            }
            else
            {
                // Second <--
                if(secondBall->position.x < firstBall->position.x)
                {
                    secondBall->directionVector.x = (secondBall->directionVector.x + firstBall->directionVector.x);
                    secondBall->position.x = (firstBall->position.x - firstBall->r) - secondBall->r;
                }
                else
                {
                    secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                    secondBall->position.x = (firstBall->position.x + firstBall->r) + secondBall->r;
                }
            }
        }

        //
        // Y
        //

        if(firstBall->verticalDirect > 0)
        {
            // First ^
            if(secondBall->verticalDirect > 0)
            {
                // Second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // Firsrt
                    // Second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
                else
                {
                    // Second
                    // First
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = 1;
                    secondBall->position.y = (firstBall->position.y + firstBall->r) + secondBall->r;
                }
            }
            else
            {
                // Second v
                secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                secondBall->verticalDirect = 1;
            }
        }
        else
        {
            // First v
            if(secondBall->verticalDirect > 0)
            {
                // Second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // Firsrt
                    // Second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
            }
            else
            {
                // Second v

                if(secondBall->position.y < firstBall->position.y)
                {
                    // Firsrt
                    // Second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
                else
                {
                    // Second
                    // First
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = 1;
                    secondBall->position.y = (firstBall->position.y + firstBall->r) + secondBall->r;
                }
            }

        }

        #ifdef SET_NULL_AFTER_COLLIZION
        {
            std::lock_guard<std::mutex> lg2(firstBall->mutexChangingDirectionVector);
            firstBall->directionVector.x = 0.1;
            firstBall->directionVector.y = 0.1;
        }
        #endif
    }
}

void Ball::setRandomDirectionVertex()
{
    int rn = rand();
    int y = 6 - (rn % 12) ;
    int x = 7 - (rn % 14);

    x = x?x:x+1;
    y = y?y:y+1;

    directionVector.x = x;
    directionVector.y = y;
}