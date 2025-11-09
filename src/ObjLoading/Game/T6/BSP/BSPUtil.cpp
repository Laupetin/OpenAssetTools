#include "BSPUtil.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <numbers>

using namespace T6;

namespace BSP
{
    std::string BSPUtil::getFileNameForBSPAsset(const std::string& assetName)
    {
        return std::format("BSP/{}", assetName);
    }

    vec3_t BSPUtil::convertToBO2Coords(const vec3_t& coordinate)
    {
        vec3_t result;
        result.x = coordinate.x;
        result.y = -coordinate.z;
        result.z = coordinate.y;
        return result;
    }

    vec3_t BSPUtil::convertFromBO2Coords(const vec3_t& coordinate)
    {
        vec3_t result;
        result.x = coordinate.x;
        result.y = coordinate.z;
        result.z = -coordinate.y;
        return result;
    }

    void BSPUtil::updateAABB(const vec3_t& newAABBMins, const vec3_t& newAABBMaxs, vec3_t& AABBMins, vec3_t& AABBMaxs)
    {
        AABBMins.x = std::min(AABBMins.x, newAABBMins.x);
        AABBMaxs.x = std::max(AABBMaxs.x, newAABBMaxs.x);

        AABBMins.y = std::min(AABBMins.y, newAABBMins.y);
        AABBMaxs.y = std::max(AABBMaxs.y, newAABBMaxs.y);

        AABBMins.z = std::min(AABBMins.z, newAABBMins.z);
        AABBMaxs.z = std::max(AABBMaxs.z, newAABBMaxs.z);
    }

    void BSPUtil::updateAABBWithPoint(const vec3_t& point, vec3_t& AABBMins, vec3_t& AABBMaxs)
    {
        AABBMins.x = std::min(AABBMins.x, point.x);
        AABBMaxs.x = std::max(AABBMaxs.x, point.x);

        AABBMins.y = std::min(AABBMins.y, point.y);
        AABBMaxs.y = std::max(AABBMaxs.y, point.y);

        AABBMins.z = std::min(AABBMins.z, point.z);
        AABBMaxs.z = std::max(AABBMaxs.z, point.z);
    }

    vec3_t BSPUtil::calcMiddleOfAABB(const vec3_t& mins, const vec3_t& maxs)
    {
        vec3_t result;
        result.x = (mins.x + maxs.x) * 0.5f;
        result.y = (mins.y + maxs.y) * 0.5f;
        result.z = (mins.z + maxs.z) * 0.5f;
        return result;
    }

    vec3_t BSPUtil::calcHalfSizeOfAABB(const vec3_t& mins, const vec3_t& maxs)
    {
        vec3_t result;
        result.x = (maxs.x - mins.x) * 0.5f;
        result.y = (maxs.y - mins.y) * 0.5f;
        result.z = (maxs.z - mins.z) * 0.5f;
        return result;
    }

    float BSPUtil::distBetweenPoints(const vec3_t& p1, const vec3_t& p2)
    {
        const auto x = p2.x - p1.x;
        const auto y = p2.y - p1.y;
        const auto z = p2.z - p1.z;

        return sqrtf((x * x) + (y * y) + (z * z));
    }

    // angles are in euler degrees
    void BSPUtil::convertAnglesToAxis(const vec3_t* angles, vec3_t* axis)
    {
        constexpr auto conversionValue = std::numbers::pi_v<float> / 180.0f;
        const auto xRadians = angles->x * conversionValue;
        const auto yRadians = angles->y * conversionValue;
        const auto zRadians = angles->z * conversionValue;

        const auto cosX = cos(xRadians);
        const auto sinX = sin(xRadians);
        const auto cosY = cos(yRadians);
        const auto sinY = sin(yRadians);
        const auto cosZ = cos(zRadians);
        const auto sinZ = sin(zRadians);

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
