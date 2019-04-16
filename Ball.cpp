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

void Ball::setRandomDirectionVertex()
{
    int rn = rand();
    int y = rn % 5 + 1;
    int x = rn % 21 - 10;

    directionVector.x = x;
    directionVector.y = y;
}

int Ball::calculateNevCoordinate(const std::atomic<bool> * const pause)
{
    while (end == false)
    {
        mutexChangingDirectionVector.lock();

        if(pause->load() == false && isFrozen.load() == false)
        {
            vector2d tempPosition = position;
            if (isMovingY.load() == true)
            {
                // Check collision of top wall.
                if ((tempPosition.y + r) > 1)
                {
                    // Align the position of ball.
                    tempPosition.y = 1.0 - r;

                    // Bounce the ball and subtract 1/4 of acceleration.
                    directionVector.y = (6.0 * directionVector.y / 7.0);
                    verticalDirect = -1;
                }
                // Check colizion of bottom wall.
                else if ((tempPosition.y - r) < -1)
                {
                    // Set acceleration as null if its end of bounced.
                    if (directionVector.y < 0.022)
                    {
                        directionVector.y = 0.0;
                        isMovingY.store(false);
                    }

                    // Align the position of ball.
                    tempPosition.y = -1.0 + r;

                    // Bounce the ball and subtract 1/4 of acceleration
                    directionVector.y = (6.0 * directionVector.y / 7.0);
                    verticalDirect = 1;
                }
                // If the ball hang in the air:
                else if (directionVector.y < 0.01)
                {
                    directionVector.y = 0.01;
                    verticalDirect = -1;
                }

                //if(isMovingY == 1 && directionVector.y >0)
                {
                    // If ball in going up.
                    if (verticalDirect == 1)
                    {
                        directionVector.y -= ((5 - directionVector.y) / 500) + (0.1 / (FPS));  // ==> 10m/s
                        //directionVector.y -= ((5 - directionVector.y) / 500) + (0.02 / (FPS)); // air resistance
                    }
                    // If ball in going down.
                    else if (verticalDirect == -1)
                    {
                        //std::cout << "ZMIANA77" << std::endl;
                        directionVector.y += ((5 - directionVector.y) / 500) + (0.1 / (FPS)); // ==> 10m/s
                    }
                }
                tempPosition.y += (0.01 * (directionVector.y * directionVector.y) * verticalDirect);
            }

            if (isMovingX.load() == true)
            {
                // Check collision of right wall.
                if ((tempPosition.x + r) > 1)
                {
                    // Align the position of ball.
                    tempPosition.x = 1.0 - r;

                    // Bounce the ball and subtract 1/7 of acceleration
                    directionVector.x = (6.0 * directionVector.x / 7.0);
                    directionVector.x *= -1.0;
                }
                // Check collision of left wall.
                else if ((tempPosition.x - r) < -1)
                {

                    // Align the position of ball.
                    tempPosition.x = -1.0 + r;

                    // Bounce the ball and subtract 1/7 of acceleration
                    directionVector.x = (6.0 * directionVector.x / 7.0);
                    directionVector.x *= -1.0;
                }

                if (directionVector.x > 0)
                {
                    directionVector.x -= (0.1 / (FPS));
                }
                else
                {
                    directionVector.x += (0.1 / (FPS));
                }

                if (directionVector.x < 0.023 && directionVector.x > -0.023)
                {
                    isMovingX.store(false);
                }

                tempPosition.x += (0.01 * directionVector.x);
            }

            position = tempPosition;
            // possitX = tempPosition.x;
            // possitY = tempPosition.y;

            //std::cout << "LOOP " << this << " | " << possitX << " | " << possitY << "\n";
        }

        mutexChangingDirectionVector.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
    }
}

int Ball::handleCillizion(Ball *firstBall, Ball *secondBall)
{
    if(firstBall->isFrozen.load() == false)
    {
        firstBall->isFrozen.store(true);
        secondBall->isFrozen.store(false);

        secondBall->mutexChangingDirectionVector.lock();

        //
        // X
        //

        if(firstBall->directionVector.x > 0)
        {
            // First -->
            if(secondBall->directionVector.x > 0)
            {
                // SECOND -->
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
                // SECOND <--
                secondBall->directionVector.x = -1.0 *(secondBall->directionVector.x + firstBall->directionVector.x);
                secondBall->position.x = (firstBall->position.x + firstBall->r) + secondBall->r;
            }
        }
        else
        {
            // First <--
            if(secondBall->directionVector.x >= 0)
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
            // first ^
            if(secondBall->verticalDirect > 0)
            {
                // second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // firsrt
                    // second 
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
            else
            {
                // second v
                secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                secondBall->verticalDirect = 1;
            }
        }
        else
        {
            // first v
            if(secondBall->verticalDirect > 0)
            {
                // second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // firsrt
                    // second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
            }
            else
            {
                // second v

                if(secondBall->position.y < firstBall->position.y)
                {
                    // firsrt
                    // second 
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

        secondBall->mutexChangingDirectionVector.unlock();

        #ifdef SET_NULL_AFTER_COLLIZION
        firstBall->mutexChangingDirectionVector.lock();
        firstBall->directionVector.x = 0.1;
        firstBall->directionVector.y = 0.1;
        firstBall->mutexChangingDirectionVector.unlock();
        #endif
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

        firstBall->isFrozen.store(true);
        secondBall->isFrozen.store(false);

        secondBall->mutexChangingDirectionVector.lock();

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
            // first ^
            if(secondBall->verticalDirect > 0)
            {
                // second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // firsrt
                    // second 
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
            else
            {
                // second v
                secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                secondBall->verticalDirect = 1;
            }
        }
        else
        {
            // first v
            if(secondBall->verticalDirect > 0)
            {
                // second ^
                if(secondBall->position.y < firstBall->position.y)
                {
                    // firsrt
                    // second 
                    secondBall->directionVector.y = secondBall->directionVector.y + firstBall->directionVector.y;
                    secondBall->verticalDirect = -1;
                    secondBall->position.y = (firstBall->position.y - firstBall->r) - secondBall->r;
                }
            }
            else
            {
                // second v

                if(secondBall->position.y < firstBall->position.y)
                {
                    // firsrt
                    // second 
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

        secondBall->mutexChangingDirectionVector.unlock();

        #ifdef SET_NULL_AFTER_COLLIZION
        firstBall->mutexChangingDirectionVector.lock();
        firstBall->directionVector.x = 0.1;
        firstBall->directionVector.y = 0.1;
        firstBall->mutexChangingDirectionVector.unlock();
        #endif
    }
    return 0;
}