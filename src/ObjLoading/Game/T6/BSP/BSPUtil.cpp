#pragma once

#include <cmath>
#include <format>

#include "BSPUtil.h"

std::string BSPUtil::getFileNameForBSPAsset(std::string& assetName)
{
    return std::format("BSP/{}", assetName);
}

vec3_t BSPUtil::convertToBO2Coords(vec3_t& coordinate)
{
    vec3_t result;
    result.x = coordinate.x;
    result.y = -coordinate.z;
    result.z = coordinate.y;
    return result;
}

vec3_t BSPUtil::convertFromBO2Coords(vec3_t& coordinate)
{
    vec3_t result;
    result.x = coordinate.x;
    result.y = coordinate.z;
    result.z = -coordinate.y;
    return result;
}

void BSPUtil::updateAABB(vec3_t& newAABBMins, vec3_t& newAABBMaxs, vec3_t& AABBMins, vec3_t& AABBMaxs)
{
    if (AABBMins.x > newAABBMins.x)
        AABBMins.x = newAABBMins.x;

    if (newAABBMaxs.x > AABBMaxs.x)
        AABBMaxs.x = newAABBMaxs.x;

    if (AABBMins.y > newAABBMins.y)
        AABBMins.y = newAABBMins.y;

    if (newAABBMaxs.y > AABBMaxs.y)
        AABBMaxs.y = newAABBMaxs.y;

    if (AABBMins.z > newAABBMins.z)
        AABBMins.z = newAABBMins.z;

    if (newAABBMaxs.z > AABBMaxs.z)
        AABBMaxs.z = newAABBMaxs.z;
}

void BSPUtil::updateAABBWithPoint(vec3_t& point, vec3_t& AABBMins, vec3_t& AABBMaxs)
{
    if (AABBMins.x > point.x)
        AABBMins.x = point.x;

    if (point.x > AABBMaxs.x)
        AABBMaxs.x = point.x;

    if (AABBMins.y > point.y)
        AABBMins.y = point.y;

    if (point.y > AABBMaxs.y)
        AABBMaxs.y = point.y;

    if (AABBMins.z > point.z)
        AABBMins.z = point.z;

    if (point.z > AABBMaxs.z)
        AABBMaxs.z = point.z;
}

vec3_t BSPUtil::calcMiddleOfAABB(vec3_t& mins, vec3_t& maxs)
{
    vec3_t result;
    result.x = (mins.x + maxs.x) * 0.5f;
    result.y = (mins.y + maxs.y) * 0.5f;
    result.z = (mins.z + maxs.z) * 0.5f;
    return result;
}

vec3_t BSPUtil::calcHalfSizeOfAABB(vec3_t& mins, vec3_t& maxs)
{
    vec3_t result;
    result.x = (maxs.x - mins.x) * 0.5f;
    result.y = (maxs.y - mins.y) * 0.5f;
    result.z = (maxs.z - mins.z) * 0.5f;
    return result;
}

int BSPUtil::allignBy128(int size)
{
    return ((size + 127) & 0xFFFFFF80);
}

float BSPUtil::distBetweenPoints(vec3_t& p1, vec3_t& p2)
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

vec3_t BSPUtil::convertStringToVec3(std::string& str)
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

std::string BSPUtil::convertVec3ToString(vec3_t& vec)
{
    std::string result = std::format("{} {} {}", vec.x, vec.y, vec.z);
    return result;
}