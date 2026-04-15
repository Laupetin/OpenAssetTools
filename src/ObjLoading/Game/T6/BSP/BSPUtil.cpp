#pragma once

#include "BSPUtil.h"

#include <cmath>
#include <format>
#include <numbers>

namespace BSP
{
    std::string BSPUtil::getFileNameForBSPAsset(const std::string& assetName)
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

    size_t BSPUtil::allignBy128(size_t size)
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

    void BSPUtil::calculateXmodelBounds(XModel* xmodel, vec3_t axis[3], vec3_t& out_mins, vec3_t& out_maxs)
    {
        out_mins.x = 0.0f;
        out_mins.y = 0.0f;
        out_mins.z = 0.0f;
        out_maxs.x = 0.0f;
        out_maxs.y = 0.0f;
        out_maxs.z = 0.0f;

        for (auto surfaceIndex = 0u; surfaceIndex < xmodel->lodInfo[0].numsurfs; surfaceIndex++)
        {
            const auto& surface = xmodel->surfs[surfaceIndex + xmodel->lodInfo[0].surfIndex];

            if (!surface.verts0)
                continue;

            for (auto vertIndex = 0u; vertIndex < surface.vertCount; vertIndex++)
            {
                const auto& vertex = surface.verts0[vertIndex].xyz;

                vec3_t rotatedVert;
                rotatedVert.x = (vertex.x * axis[0].x) + (vertex.y * axis[1].x) + (vertex.z * axis[2].x);
                rotatedVert.y = (vertex.x * axis[0].y) + (vertex.y * axis[1].y) + (vertex.z * axis[2].y);
                rotatedVert.z = (vertex.x * axis[0].z) + (vertex.y * axis[1].z) + (vertex.z * axis[2].z);

                if (vertIndex == 0 && surfaceIndex == 0)
                {
                    out_mins = rotatedVert;
                    out_maxs = rotatedVert;
                }
                else
                    BSPUtil::updateAABBWithPoint(rotatedVert, out_mins, out_maxs);
            }
        }
    }

    void BSPUtil::convertAnglesToAxis(vec3_t* angles, vec3_t axis[3])
    {
        float cosX = cos(angles->x);
        float sinX = sin(angles->x);
        float cosY = cos(angles->y);
        float sinY = sin(angles->y);
        float cosZ = cos(angles->z);
        float sinZ = sin(angles->z);

        axis[0].x = cosX * cosY;
        axis[0].y = cosX * sinY;
        axis[0].z = -sinX;
        axis[1].x = ((sinZ * sinX) * cosY) - (cosZ * sinY);
        axis[1].y = ((sinZ * sinX) * sinY) + (cosZ * cosY);
        axis[1].z = sinZ * cosX;
        axis[2].x = ((cosZ * sinX) * cosY) + (sinZ * sinY);
        axis[2].y = ((cosZ * sinX) * sinY) - (sinZ * cosY);
        axis[2].z = cosZ * cosX;
    }

    void BSPUtil::convertQuaternionToAxis(vec4_t* quat, vec3_t axis[3])
    {
        float quatX = quat->v[0];
        float quatY = quat->v[1];
        float quatZ = quat->v[2];
        float quatW = quat->v[3];

        float xx = (quatX * 2.0f) * quatX;
        float xy = (quatX * 2.0f) * quatY;
        float xz = (quatX * 2.0f) * quatZ;
        float xw = (quatX * 2.0f) * quatW;

        float yy = (quatY * 2.0f) * quatY;
        float yz = (quatY * 2.0f) * quatZ;
        float yw = (quatY * 2.0f) * quatW;

        float zz = (quatZ * 2.0f) * quatZ;
        float zw = (quatZ * 2.0f) * quatW;

        axis->x = 1.0f - (zz + yy);
        axis->y = zw + xy;
        axis->z = xz - yw;

        axis[1].x = xy - zw;
        axis[1].y = 1.0f - (zz + xx);
        axis[1].z = yz + xw;

        axis[2].x = yw + xz;
        axis[2].y = yz - xw;
        axis[2].z = 1.0f - (yy + xx);
    }

    vec3_t BSPUtil::convertForwardVectorToViewAngles(vec3_t& forwardVec)
    {
        vec3_t viewAngles;

        if (forwardVec.x == 0.0f && forwardVec.y == 0.0f)
        {
            if (-forwardVec.z < 0.0f)
                viewAngles.x = 270.0f;
            else
                viewAngles.x = 90.0f;
            viewAngles.y = 0.0f;
            viewAngles.z = 0.0f;
            return viewAngles;
        }

        float xAndYDist = sqrtf((forwardVec.x * forwardVec.x) + (forwardVec.y * forwardVec.y));
        float atanXRadians = atan2f(forwardVec.z, xAndYDist);
        float atanXDegrees = atanXRadians * (-180.0f / std::numbers::pi_v<float>);
        if (atanXDegrees < 0.0f)
            atanXDegrees += 360.0f;
        viewAngles.x = atanXDegrees;

        float atanYRadians = atan2f(forwardVec.y, forwardVec.x);
        float atanYDegrees = atanYRadians * (180.0f / std::numbers::pi_v<float>);
        if (atanYDegrees < 0.0f)
            atanYDegrees += 360.0f;
        viewAngles.y = atanYDegrees;

        viewAngles.z = 0.0f;

        return viewAngles;
    }

    float BSPUtil::getPitchFromVector(vec3_t& vector)
    {
        if (vector.x == 0.0f && vector.y == 0.0f)
        {
            if (-vector.z < 0.0f)
                return -90.0f;
            else
                return 90.0f;
        }

        float xAndYDist = sqrtf((vector.x * vector.x) + (vector.y * vector.y));
        float pitchRadians = atan2f(vector.z, xAndYDist);
        float pitchDegrees = pitchRadians * (-180.0f / std::numbers::pi_v<float>);
        return pitchDegrees;
    }

    vec3_t BSPUtil::convertAxisToAngles(vec3_t axis[3])
    {
        vec3_t tempAngles = convertForwardVectorToViewAngles(axis[0]);
        float xRadiansNeg = -tempAngles.x * (std::numbers::pi_v<float> / 180.0f);
        float yRadiansNeg = -tempAngles.y * (std::numbers::pi_v<float> / 180.0f);
        float cosX = cos(xRadiansNeg);
        float sinX = sin(xRadiansNeg);
        float cosY = cos(yRadiansNeg);
        float sinY = sin(yRadiansNeg);

        vec3_t tempVec;
        float tempFloat = (axis[1].x * cosY) - (axis[1].y * sinY);
        tempVec.x = (axis[1].z * sinX) + (tempFloat * cosX);
        tempVec.y = (axis[1].x * sinY) + (axis[1].y * cosY);
        tempVec.z = (axis[1].z * cosX) - (tempFloat * sinX);
        float pitch = getPitchFromVector(tempVec);
        if (tempVec.y >= 0.0f)
            tempAngles.z = -pitch;
        else if (pitch >= 0.0f)
            tempAngles.z = pitch - 180.0f;
        else
            tempAngles.z = pitch + 180.0f;
        return tempAngles;
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
        std::string result = std::format("{} {} {}", roundf(vec.x), roundf(vec.y), roundf(vec.z));
        return result;
    }

} // namespace BSP
