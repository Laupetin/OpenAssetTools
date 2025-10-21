#pragma once

#include <cmath>
#include <format>

#include "BSPUtil.h"

// BO2 uses a different coordinate system, so this converts it back from OpenGLs default
vec3_t BSPUtil::convertToBO2Coords(vec3_t OGL_coordinate)
{
    vec3_t result;
    result.x = OGL_coordinate.x;
    result.y = -OGL_coordinate.z;
    result.z = OGL_coordinate.y;
    return result;
}

// BO2 uses a weird coordinate system, so this converts it to OpenGLs default
vec3_t BSPUtil::convertFromBO2Coords(vec3_t bo2_coordinate)
{
    vec3_t result;
    result.x = bo2_coordinate.x;
    result.y = bo2_coordinate.z;
    result.z = -bo2_coordinate.y;
    return result;
}

void BSPUtil::calcNewBounds(vec3_t* newmins, vec3_t* newmaxs, vec3_t* currmins, vec3_t* currmaxs)
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

void BSPUtil::calcNewBoundsWithPoint(vec3_t* point, vec3_t* currmins, vec3_t* currmaxs)
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

vec3_t BSPUtil::calcMiddleOfBounds(vec3_t* mins, vec3_t* maxs)
{
    // Origin is the midpoint: (min + max) / 2
    vec3_t temp;
    temp.x = mins->x + maxs->x;
    temp.y = mins->y + maxs->y;
    temp.z = mins->z + maxs->z;
    temp.x *= 0.5f;
    temp.y *= 0.5f;
    temp.z *= 0.5f;
    return temp;
}

int BSPUtil::allignBy128(int size)
{
    return ((size + 127) & 0xFFFFFF80);
}

float BSPUtil::distBetweenPoints(vec3_t p1, vec3_t p2)
{
    float x = p2.x - p1.x;
    float y = p2.y - p1.y;
    float z = p2.z - p1.z;
    return sqrtf((x * x) + (y * y) + (z * z));
}

// angles are in euler degrees
void BSPUtil::convertAnglesToAxis(vec3_t* angles, vec3_t* axis)
{
    float xRadians = angles->x * 0.017453292f; // M_PI / 180.0f
    float yRadians = angles->y * 0.017453292f; // M_PI / 180.0f
    float zRadians = angles->z * 0.017453292f; // M_PI / 180.0f
    
    float cosX = cos(xRadians);
    float sinX = sin(xRadians);
    float cosY = cos(yRadians);
    float sinY = sin(yRadians);
    float cosZ = cos(zRadians);
    float sinZ = sin(zRadians);
    
    axis[0].x = cosX * cosY;
    axis[0].y = cosX * sinY;
    axis[0].z = -sinX;
    axis[1].x = (sinZ * sinX * cosY) - (cosZ * sinY);
    axis[1].y = (sinZ * sinX * sinY) + (cosZ * cosY);
    axis[1].z = sinZ * cosX;
    axis[2].x = (cosZ * sinX * cosY) + (sinZ * sinY);
    axis[2].y = (cosZ * sinX * sinY) - (sinZ * cosY);
    axis[2].z = cosZ * cosX;
}

void BSPUtil::matrixTranspose3x3(const vec3_t* in, vec3_t* out)
{
    out[0].x = in[0].x;
    out[0].y = in[1].x;
    out[0].z = in[2].x;
    out[1].x = in[0].y;
    out[1].y = in[1].y;
    out[1].z = in[2].y;
    out[2].x = in[0].z;
    out[2].y = in[1].z;
    out[2].z = in[2].z;
}

vec3_t BSPUtil::convertStringToVec3(std::string str)
{
    std::string v1Str = str;

    int nextValIndex = 0;
    while (v1Str[nextValIndex] != ' ')
        nextValIndex++;
    nextValIndex++; // skip past space
    std::string v2Str = &v1Str[nextValIndex];

    nextValIndex = 0;
    while (v2Str[nextValIndex] != ' ')
        nextValIndex++;
    nextValIndex++; // skip past space
    std::string v3Str = &v2Str[nextValIndex];

    vec3_t result;
    result.x = static_cast<float>(atof(v1Str.c_str()));
    result.y = static_cast<float>(atof(v2Str.c_str()));
    result.z = static_cast<float>(atof(v3Str.c_str()));
    return result;
}

std::string BSPUtil::convertVec3ToString(vec3_t vec)
{
    std::string result = std::format("{} {} {}", vec.x, vec.y, vec.z);
    return result;
}