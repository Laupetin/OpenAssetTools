#pragma once


#include "Game/T6/T6.h"
using namespace T6;

class CMUtil
{
public:
    // BO2 uses a different coordinate system, so this converts it back from OpenGLs default
    static vec3_t convertToBO2Coords(vec3_t OGL_coordinate)
    {
        vec3_t result;
        result.x = OGL_coordinate.x;
        result.y = -OGL_coordinate.z;
        result.z = OGL_coordinate.y;
        return result;
    }

    // BO2 uses a weird coordinate system, so this converts it to OpenGLs default
    static vec3_t convertFromBO2Coords(vec3_t bo2_coordinate)
    {
        vec3_t result;
        result.x = bo2_coordinate.x;
        result.y = bo2_coordinate.z;
        result.z = -bo2_coordinate.y;
        return result;
    }

    static void calcNewBounds(vec3_t* newmins, vec3_t* newmaxs, vec3_t* currmins, vec3_t* currmaxs)
    {
        if (currmins->x > newmins->x)
            currmins->x = newmins->x;

        if (newmaxs->x > currmaxs->x)
            currmaxs->x = newmaxs->x;

        if (currmins->y > newmins->y)
            currmins->y = newmins->y;

        if (newmaxs->y > currmaxs->y)
            currmaxs->y = newmaxs->y;

        if (currmins->z > newmins->z)
            currmins->z = newmins->z;

        if (newmaxs->z > currmaxs->z)
            currmaxs->z = newmaxs->z;
    }

    static void calcNewBoundsWithPoint(vec3_t* point, vec3_t* currmins, vec3_t* currmaxs)
    {
        if (currmins->x > point->x)
            currmins->x = point->x;

        if (point->x > currmaxs->x)
            currmaxs->x = point->x;

        if (currmins->y > point->y)
            currmins->y = point->y;

        if (point->y > currmaxs->y)
            currmaxs->y = point->y;

        if (currmins->z > point->z)
            currmins->z = point->z;

        if (point->z > currmaxs->z)
            currmaxs->z = point->z;
    }

    static int allignBy128(int size)
    {
        return ((size + 127) & 0xFFFFFF80);
    }
};
