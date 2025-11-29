#pragma once

#include "BSP.h"

namespace BSP
{
    std::string GetFileNameForBSPAsset(const std::string& assetName);

    T6::vec3_t ConvertToBO2Coords(const T6::vec3_t& OGL_coordinate);
    T6::vec3_t ConvertFromBO2Coords(const T6::vec3_t& bo2_coordinate);

    void UpdateAABB(const T6::vec3_t& newAABBMins, const T6::vec3_t& newAABBMaxs, T6::vec3_t& AABBMins, T6::vec3_t& AABBMaxs);
    void UpdateAABBWithPoint(const T6::vec3_t& point, T6::vec3_t& AABBMins, T6::vec3_t& AABBMaxs);

    T6::vec3_t CalcMiddleOfAABB(const T6::vec3_t& mins, const T6::vec3_t& maxs);
    T6::vec3_t CalcHalfSizeOfAABB(const T6::vec3_t& mins, const T6::vec3_t& maxs);

    float DistBetweenPoints(const T6::vec3_t& p1, const T6::vec3_t& p2);

    void ConvertAnglesToAxis(const T6::vec3_t* angles, T6::vec3_t* axis);

    void MatrixTranspose3x3(const T6::vec3_t* in, T6::vec3_t* out);
} // namespace BSP
