#include "BSPUtil.h"

#include <algorithm>
#include <cmath>
#include <format>

using namespace T6;

namespace BSP
{
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
        AABBMins.x = std::min(AABBMins.x, newAABBMins.x);
        AABBMaxs.x = std::max(AABBMaxs.x, newAABBMaxs.x);

        AABBMins.y = std::min(AABBMins.y, newAABBMins.y);
        AABBMaxs.y = std::max(AABBMaxs.y, newAABBMaxs.y);

        AABBMins.z = std::min(AABBMins.z, newAABBMins.z);
        AABBMaxs.z = std::max(AABBMaxs.z, newAABBMaxs.z);
    }

    void BSPUtil::updateAABBWithPoint(vec3_t& point, vec3_t& AABBMins, vec3_t& AABBMaxs)
    {
        AABBMins.x = std::min(AABBMins.x, point.x);
        AABBMaxs.x = std::max(AABBMaxs.x, point.x);

        AABBMins.y = std::min(AABBMins.y, point.y);
        AABBMaxs.y = std::max(AABBMaxs.y, point.y);

        AABBMins.z = std::min(AABBMins.z, point.z);
        AABBMaxs.z = std::max(AABBMaxs.z, point.z);
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
} // namespace BSP
