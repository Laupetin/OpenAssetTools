#pragma once

#include "BSP.h"

namespace BSP
{
    class BSPUtil
    {
    public:
        static std::string getFileNameForBSPAsset(std::string& assetName);
        static T6::vec3_t convertToBO2Coords(T6::vec3_t& OGL_coordinate);
        static T6::vec3_t convertFromBO2Coords(T6::vec3_t& bo2_coordinate);
        static void updateAABB(T6::vec3_t& newAABBMins, T6::vec3_t& newAABBMaxs, T6::vec3_t& AABBMins, T6::vec3_t& AABBMaxs);
        static void updateAABBWithPoint(T6::vec3_t& point, T6::vec3_t& AABBMins, T6::vec3_t& AABBMaxs);
        static T6::vec3_t calcMiddleOfAABB(T6::vec3_t& mins, T6::vec3_t& maxs);
        static T6::vec3_t calcHalfSizeOfAABB(T6::vec3_t& mins, T6::vec3_t& maxs);
        static float distBetweenPoints(T6::vec3_t& p1, T6::vec3_t& p2);
        static void convertAnglesToAxis(T6::vec3_t* angles, T6::vec3_t* axis);
        static void matrixTranspose3x3(const T6::vec3_t* in, T6::vec3_t* out);
    };
} // namespace BSP
