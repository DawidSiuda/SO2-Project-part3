#include "Bat.h"


Bat::Bat()
{
    bottomEdgeYPossition = 0 - (BAT_LENGTH / 2);
    length = BAT_LENGTH;
    thickness = 255;
    color.a = color.r = color.g = color.b = 1.0;
    direction.store(STOP);
    end.store(false);
    std::thread *leftBatMoving = new std::thread(&Bat::move, this);
}

void Bat::changeDirect(int newDirect)
{
    direction.store(newDirect);
    condition_variableWait.notify_all();
}

void Bat::endThread()
{
    end.store(true);
    condition_variableWait.notify_all();
}

int Bat::getDirect() const
{
    return direction.load();
}

int Bat::getThickness() const
{
    return thickness;
}

float Bat::getLength() const
{
    return length;
}

float Bat::getBottomEdgeYPossition() const
{
    return bottomEdgeYPossition;
}

Color Bat::getColor() const
{
    return color;
} 

void Bat::move()
{
    while(end.load() == false)
    {
        {
            std::unique_lock<std::mutex> uniqueLockMutexChangingDirectionVector(mutexBatPossition);

            if(direction.load() == TOP && bottomEdgeYPossition + length < TOP_LINE_Y)
            {
                bottomEdgeYPossition += SPEED_OF_BATS / FPS;
            }
            else if(direction.load() == BOTTOM && bottomEdgeYPossition > BOTTOM_LINE_Y)
            {
                bottomEdgeYPossition -= SPEED_OF_BATS / FPS;
            }
            else
            {
                condition_variableWait.wait(uniqueLockMutexChangingDirectionVector, [this]{return (this->direction.load() != STOP || this->end.load() == true);});
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
    }
}