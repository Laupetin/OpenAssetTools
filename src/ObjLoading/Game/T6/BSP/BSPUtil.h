#pragma once

#include "BSP.h"

namespace BSP
{
    class BSPUtil
    {
    public:
        static std::string getFileNameForBSPAsset(const std::string& assetName);
        static T6::vec3_t convertToBO2Coords(const T6::vec3_t& OGL_coordinate);
        static T6::vec3_t convertFromBO2Coords(const T6::vec3_t& bo2_coordinate);
        static void updateAABB(const T6::vec3_t& newAABBMins, const T6::vec3_t& newAABBMaxs, T6::vec3_t& AABBMins, T6::vec3_t& AABBMaxs);
        static void updateAABBWithPoint(const T6::vec3_t& point, T6::vec3_t& AABBMins, T6::vec3_t& AABBMaxs);
        static T6::vec3_t calcMiddleOfAABB(const T6::vec3_t& mins, const T6::vec3_t& maxs);
        static T6::vec3_t calcHalfSizeOfAABB(const T6::vec3_t& mins, const T6::vec3_t& maxs);
        static float distBetweenPoints(const T6::vec3_t& p1, const T6::vec3_t& p2);
        static void convertAnglesToAxis(const T6::vec3_t* angles, T6::vec3_t* axis);
        static void matrixTranspose3x3(const T6::vec3_t* in, T6::vec3_t* out);
    };
} // namespace BSP
