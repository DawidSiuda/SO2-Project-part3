#ifndef STRUCTURES_H
#define STRUCTURES_H

#define FPS 50
#define SET_NULL_AFTER_COLLIZION

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