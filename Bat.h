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
    int getDirect() const;
    int getThickness() const;
    float getLength() const;
    float getBottomEdgeYPossition() const;
    Color getColor() const;

private:
    //
    // Function move should be run in new thread.
    // It's responsible for control the bat and it ends
    // when "std::atomic<bool> end" storages true. 
    //

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