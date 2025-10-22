#pragma once

#include "BSP.h"


class BSPUtil
{
public:
    static vec3_t convertToBO2Coords(vec3_t OGL_coordinate);
    static vec3_t convertFromBO2Coords(vec3_t bo2_coordinate);
    static void calcNewBounds(vec3_t* newmins, vec3_t* newmaxs, vec3_t* currmins, vec3_t* currmaxs);
    static void calcNewBoundsWithPoint(vec3_t* point, vec3_t* currmins, vec3_t* currmaxs);
    static vec3_t calcMiddleOfBounds(vec3_t* mins, vec3_t* maxs);
    static int allignBy128(int size);
    static float distBetweenPoints(vec3_t p1, vec3_t p2);
    static void convertAnglesToAxis(vec3_t* angles, vec3_t* axis);
    static void matrixTranspose3x3(const vec3_t* in, vec3_t* out);
    static vec3_t convertStringToVec3(std::string str);
    static std::string convertVec3ToString(vec3_t vec);
};
