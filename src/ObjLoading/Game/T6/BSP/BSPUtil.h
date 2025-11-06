#pragma once

#include "BSP.h"

namespace BSP
{
    class BSPUtil
    {
    public:
        static std::string getFileNameForBSPAsset(std::string& assetName);
        static vec3_t convertToBO2Coords(vec3_t& OGL_coordinate);
        static vec3_t convertFromBO2Coords(vec3_t& bo2_coordinate);
        static void updateAABB(vec3_t& newAABBMins, vec3_t& newAABBMaxs, vec3_t& AABBMins, vec3_t& AABBMaxs);
        static void updateAABBWithPoint(vec3_t& point, vec3_t& AABBMins, vec3_t& AABBMaxs);
        static vec3_t calcMiddleOfAABB(vec3_t& mins, vec3_t& maxs);
        static vec3_t calcHalfSizeOfAABB(vec3_t& mins, vec3_t& maxs);
        static float distBetweenPoints(vec3_t& p1, vec3_t& p2);
        static void convertAnglesToAxis(vec3_t* angles, vec3_t* axis);
        static void matrixTranspose3x3(const vec3_t* in, vec3_t* out);
        static vec3_t convertStringToVec3(std::string& str);
        static std::string convertVec3ToString(vec3_t& vec);
    };
} // namespace BSP
