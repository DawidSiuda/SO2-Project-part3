#include "Ball.h"

std::once_flag Ball::onceFlagRandSeedForBall;

Ball::Ball(float posX, float posY, float r, float movX, float movY) : position(posX, posY),
                                                                      directionVector(movX, movY),
                                                                      isMovingY(true),
                                                                      isMovingX(true)
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
        if(pause->load() == false)
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

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
    }
}