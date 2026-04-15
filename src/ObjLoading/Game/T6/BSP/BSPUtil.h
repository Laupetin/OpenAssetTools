#pragma once

#include "BSP.h"

namespace BSP
{
    class BSPUtil
    {
    public:
        static std::string getFileNameForBSPAsset(const std::string& assetName);
        static vec3_t convertToBO2Coords(vec3_t& OGL_coordinate);
        static vec3_t convertFromBO2Coords(vec3_t& bo2_coordinate);
        static void updateAABB(vec3_t& newAABBMins, vec3_t& newAABBMaxs, vec3_t& AABBMins, vec3_t& AABBMaxs);
        static void updateAABBWithPoint(vec3_t& point, vec3_t& AABBMins, vec3_t& AABBMaxs);
        static vec3_t calcMiddleOfAABB(vec3_t& mins, vec3_t& maxs);
        static vec3_t calcHalfSizeOfAABB(vec3_t& mins, vec3_t& maxs);
        static size_t allignBy128(size_t size);
        static float distBetweenPoints(vec3_t& p1, vec3_t& p2);
        static void calculateXmodelBounds(XModel* xmodel, vec3_t axis[3], vec3_t& out_mins, vec3_t& out_maxs);
        static void convertAnglesToAxis(vec3_t* angles, vec3_t axis[3]);
        static void convertQuaternionToAxis(vec4_t* quat, vec3_t axis[3]);
        static vec3_t convertForwardVectorToViewAngles(vec3_t& forwardVec);
        static float getPitchFromVector(vec3_t& vector);
        static vec3_t convertAxisToAngles(vec3_t axis[3]);
        static void matrixTranspose3x3(const vec3_t* in, vec3_t* out);
        static vec3_t convertStringToVec3(std::string& str);
        static std::string convertVec3ToString(vec3_t& vec);
    };
} // namespace BSP
