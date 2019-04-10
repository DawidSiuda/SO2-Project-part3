#include "Ball.h"

bool Ball::setSeed = false;

Ball::Ball(float posX, float posY, float r, float movX, float movY) : position(posX, posY),
                                                                      directionVector(movX, movY),
                                                                      possitX(posX),
                                                                      possitY(posY)
                                                                    //   positionX(0.0),
                                                                    //   positionY(0.0)
{
    //positionX.store(posX);
    //positionY.store(posY);

    end = false;

    this->r = r;
    verticalDirect = 1;
    horizontalDirect = 1;

    isMovingY = 1;
    isMovingX = 1;

    if (movX == 0 and movY == 0)
    {
        Ball::setRandomDirectionVertex(this);
    }
}

void Ball::setRandomDirectionVertex(Ball *ball)
{
    if (Ball::setSeed == false)
    {
        /* initialize random seed: */
        srand(time(NULL));

        Ball::setSeed == true;
    }

    int rn = rand();
    int y = rn % 5 + 1;
    int x = rn % 11 - 5;

    ball->directionVector.x = x;
    ball->directionVector.y = y;
}

int Ball::calculateNevCoordinate()
{
    while (end == false)
    {
        vector2d tempPosition = position;
        if (isMovingY == 1)
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
                    isMovingY = 0;
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

        if (isMovingX == 1)
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
                isMovingX = 0;
            }

            tempPosition.x += (0.01 * directionVector.x);
        }

        position = tempPosition;
        possitX = tempPosition.x;
        possitY = tempPosition.y;

        //std::cout << "LOOP " << this << " | " << possitX << " | " << possitY << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
    }
}