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

void Bat::endThread()
{
    end.store(true);
    condition_variableWait.notify_all();
    
}

void Bat::changeDirect(int newDirect)
{
    direction.store(newDirect);
    condition_variableWait.notify_all();
}
void Bat::move()
{
    // std::condition_variable condition_variableWait;
    // std::mutex mutexBatPossition;

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
            
            //std::cout<< "Bat direction: " << direction.load() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
    }
}

// void Bat::move(const std::atomic<int> * direction, const std::atomic<bool> *end)
// {
//     std::condition_variable condition_variableWait;
//     std::mutex mutexBatPossition;

//     while(end->load() == false)
//     {
//         std::unique_lock<std::mutex> uniqueLockMutexChangingDirectionVector(mutexBatPossition);

//         if(direction->load() == TOP && bottomEdgeYPossition + length < TOP_LINE_Y)
//         {
//             bottomEdgeYPossition += SPEED_OF_BATS / FPS;
//         }
//         else if(direction->load() == BOTTOM && bottomEdgeYPossition > BOTTOM_LINE_Y)
//         {
//             bottomEdgeYPossition -= SPEED_OF_BATS / FPS;
//         }
//         else
//         {
//             condition_variableWait.wait(uniqueLockMutexChangingDirectionVector, [direction, end]{return (direction->load() != STOP || end->load() == true);});
//         }
        
//         //std::cout<< "Bat direction: " << direction->load() << std::endl;

//         std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
//     }
// }

float Bat::getLength() const
{
    return length;
}

float Bat::getBottomEdgeYPossition() const
{
    return bottomEdgeYPossition;
}

int Bat::getThickness() const
{
    return thickness;
}

Color Bat::getColor() const
{
    return color;
} 