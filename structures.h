#ifndef STRUCTURES_H
#define STRUCTURES_H

#define FPS 50
#define SET_NULL_AFTER_COLLIZION
#define TOP_LINE_Y 0.60
#define BOTTOM_LINE_Y -0.60
#define BAT_LENGTH 0.4

#define TOP 0
#define BOTTOM 1
#define STOP 3

#define TIME_TO_CREATE_NEW_BALL 600 // In miliseconds.
#define SPEED_OF_BATS 1.5 // 1 is equal one second from one edge the second edge. 

class vector2d
{
public:
    float x;
    float y;

    inline vector2d(float x = 0, float y = 0)
    {
        this->x = x;
        this->y = y;
    }
};

struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

//static std::once_flag onceFlagRandSeed;


#endif //STRUCTURES