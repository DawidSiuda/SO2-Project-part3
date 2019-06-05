#ifndef BAT_H
#define BAT_H

#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <condition_variable>

#include "structures.h"

class Bat
{
public:
    Bat();
    void changeDirect(int newDirect);
    void endThread();
    int getDirect()
    {
        return direction.load();
    }
    //void move(const std::atomic<int> * direction, const std::atomic<bool> *end);
    float getLength() const;
    float getBottomEdgeYPossition() const;
    int getThickness() const;
    Color getColor() const;
    //float getYCenter();

private:
    //void move(const std::atomic<int> * direction, const std::atomic<bool> *end);
    void move();

    std::atomic<int> direction;
    std::atomic<bool> end;
    float bottomEdgeYPossition;
    float length;
    int thickness;
    Color color;
    std::mutex mutexBatPossition;
    std::condition_variable condition_variableWait;

};

#endif // BAT_H