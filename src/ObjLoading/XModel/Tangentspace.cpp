#include "Tangentspace.h"

#include <cassert>
#include <cmath>
#include <numbers>

namespace tangent_space
{
    constexpr float NULL_VEC3[3]{0, 0, 0};

    typedef float tvec2[2];
    typedef float tvec3[3];

    const tvec2& GetVec2(const void* dest, const size_t index, const size_t stride)
    {
        return *reinterpret_cast<const tvec2*>(static_cast<const char*>(dest) + stride * index);
    }

    const tvec3& GetVec3(const void* dest, const size_t index, const size_t stride)
    {
        return *reinterpret_cast<const tvec3*>(static_cast<const char*>(dest) + stride * index);
    }

    tvec3& GetVec3(void* dest, const size_t index, const size_t stride)
    {
        return *reinterpret_cast<tvec3*>(static_cast<char*>(dest) + stride * index);
    }

    void SetVec3(void* dest, const size_t index, const size_t stride, const tvec3& data)
    {
        auto* out = reinterpret_cast<float (*)[3]>(static_cast<char*>(dest) + stride * index);
        (*out)[0] = data[0];
        (*out)[1] = data[1];
        (*out)[2] = data[2];
    }

    bool Vec3_IsNormalized(const tvec3& a1)
    {
        const auto len = a1[0] * a1[0] + a1[1] * a1[1] + a1[2] * a1[2];
        const auto error = fabs(len - 1.0f);
        return error < 0.0020000001;
    }

    float Vec3_Normalize(tvec3& vector)
    {
        float length = std::sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
        if (-length >= 0.0f)
            length = 1.0f;
        const auto lengthInv = 1.0f / length;
        vector[0] = lengthInv * vector[0];
        vector[1] = lengthInv * vector[1];
        vector[2] = lengthInv * vector[2];
        return length;
    }

    void Vec3_Cross(const tvec3& v0, const tvec3& v1, tvec3& cross)
    {
        cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
        cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
        cross[2] = v0[0] * v1[1] - v1[0] * v0[1];
    }

    float AngleBetweenOriginVectors(const tvec3& a1, const tvec3& a2)
    {
        const auto v4 = a1[0] * a2[0] + a1[1] * a2[1] + a1[2] * a2[2];
        if (v4 <= -1.0)
            return -std::numbers::pi_v<float>;
        if (v4 >= 1.0)
            return std::numbers::pi_v<float>;

        return acos(v4);
    }

    void sub_10022E80(const VertexData& vertexData, const uint16_t i0, const uint16_t i1, const uint16_t i2, tvec3& outVector, tvec3& outCross)
    {
        const auto& i0_uv = GetVec2(vertexData.uvData, i0, vertexData.uvDataStride);
        const auto& i1_uv = GetVec2(vertexData.uvData, i1, vertexData.uvDataStride);
        const auto& i2_uv = GetVec2(vertexData.uvData, i2, vertexData.uvDataStride);
        const auto& i0_position = GetVec3(vertexData.positionData, i0, vertexData.positionDataStride);
        const auto& i1_position = GetVec3(vertexData.positionData, i1, vertexData.positionDataStride);
        const auto& i2_position = GetVec3(vertexData.positionData, i2, vertexData.positionDataStride);

        const auto uv0_1m0 = i1_uv[0] - i0_uv[0];
        const auto uv0_2m0 = i2_uv[0] - i0_uv[0];
        const auto uv1_1m0 = i1_uv[1] - i0_uv[1];
        const auto uv1_2m0 = i2_uv[1] - i0_uv[1];

        const auto p0_1m0 = i1_position[0] - i0_position[0];
        const auto p0_2m0 = i2_position[0] - i0_position[0];
        const auto p1_1m0 = i1_position[1] - i0_position[1];
        const auto p1_2m0 = i2_position[1] - i0_position[1];
        const auto p2_1m0 = i1_position[2] - i0_position[2];
        const auto p2_2m0 = i2_position[2] - i0_position[2];

        if (uv1_2m0 * uv0_1m0 >= uv1_1m0 * uv0_2m0)
        {
            outVector[0] = p0_1m0 * uv1_2m0 - p0_2m0 * uv1_1m0;
            outCross[0] = p0_2m0 * uv0_1m0 - p0_1m0 * uv0_2m0;
            outVector[1] = p1_1m0 * uv1_2m0 - p1_2m0 * uv1_1m0;
            outCross[1] = p1_2m0 * uv0_1m0 - p1_1m0 * uv0_2m0;
            outVector[2] = uv1_2m0 * p2_1m0 - uv1_1m0 * p2_2m0;
            outCross[2] = uv0_1m0 * p2_2m0 - uv0_2m0 * p2_1m0;
        }
        else
        {
            outVector[0] = p0_2m0 * uv1_1m0 - p0_1m0 * uv1_2m0;
            outCross[0] = p0_1m0 * uv0_2m0 - p0_2m0 * uv0_1m0;
            outVector[1] = p1_2m0 * uv1_1m0 - p1_1m0 * uv1_2m0;
            outCross[1] = p1_1m0 * uv0_2m0 - p1_2m0 * uv0_1m0;
            outVector[2] = uv1_1m0 * p2_2m0 - uv1_2m0 * p2_1m0;
            outCross[2] = uv0_2m0 * p2_1m0 - p2_2m0 * uv0_1m0;
        }

        Vec3_Normalize(outVector);
        Vec3_Normalize(outCross);
    }

    void GetExteriorAnglesOfTri(const VertexData& vertexData, tvec3& outExteriorAngles, const uint16_t i0, const uint16_t i1, const uint16_t i2)
    {
        tvec3 L21;
        tvec3 L02;
        tvec3 L10;

        const auto* index0Position = GetVec3(vertexData.positionData, i0, vertexData.positionDataStride);
        const auto* index1Position = GetVec3(vertexData.positionData, i1, vertexData.positionDataStride);
        const auto* index2Position = GetVec3(vertexData.positionData, i2, vertexData.positionDataStride);
        L10[0] = index0Position[0] - index1Position[0];
        L10[1] = index0Position[1] - index1Position[1];
        L10[2] = index0Position[2] - index1Position[2];
        L21[0] = index1Position[0] - index2Position[0];
        L21[1] = index1Position[1] - index2Position[1];
        L21[2] = index1Position[2] - index2Position[2];
        L02[0] = index2Position[0] - index0Position[0];
        L02[1] = index2Position[1] - index0Position[1];
        L02[2] = index2Position[2] - index0Position[2];
        Vec3_Normalize(L10);
        Vec3_Normalize(L21);
        Vec3_Normalize(L02);
        outExteriorAngles[0] = AngleBetweenOriginVectors(L10, L02);
        outExteriorAngles[1] = AngleBetweenOriginVectors(L21, L10);
        outExteriorAngles[2] = AngleBetweenOriginVectors(L02, L21);
    }

    void sub_10022CA0(const uint16_t index, void* sourcesDest, const tvec3& ecx0, const float exteriorAngle, const size_t stride)
    {
        auto& vec = GetVec3(sourcesDest, index, stride);
        vec[0] = ecx0[0] * exteriorAngle + vec[0];
        vec[1] = ecx0[1] * exteriorAngle + vec[1];
        vec[2] = ecx0[2] * exteriorAngle + vec[2];
    }

    void sub_10014EE0(const tvec3& src, tvec3& a2)
    {
        assert(Vec3_IsNormalized(src));

        const auto v4 = src[0] * src[0];
        float v5 = src[1] * src[1];
        float v6 = src[2] * src[2];
        int v3 = v4 > v5;
        if (*(&v4 + v3) > v6)
            v3 = 2;
        const auto srcc = -src[v3];
        a2[0] = src[0] * srcc;
        a2[1] = src[1] * srcc;
        a2[2] = src[2] * srcc;
        a2[v3] = a2[v3] + 1.0f;
        Vec3_Normalize(a2);
    }

    void CalculateTangentSpace(const VertexData& vertexData, const size_t triCount, const size_t vertexCount)
    {
        // Set tangents and binormals to 0
        for (auto vertexIndex = 0u; vertexIndex < vertexCount; vertexIndex++)
        {
            SetVec3(vertexData.tangentData, vertexIndex, vertexData.tangentDataStride, NULL_VEC3);
            SetVec3(vertexData.binormalData, vertexIndex, vertexData.binormalDataStride, NULL_VEC3);
        }

        for (auto triIndex = 0u; triIndex < triCount; triIndex++)
        {
            const auto i0 = vertexData.triData[triIndex * 3u + 0u];
            const auto i1 = vertexData.triData[triIndex * 3u + 1u];
            const auto i2 = vertexData.triData[triIndex * 3u + 2u];

            tvec3 vector, cross, exteriorAngles;
            sub_10022E80(vertexData, i0, i1, i2, vector, cross);
            GetExteriorAnglesOfTri(vertexData, exteriorAngles, i0, i1, i2);
            sub_10022CA0(i0, vertexData.tangentData, vector, exteriorAngles[0], vertexData.tangentDataStride);
            sub_10022CA0(i1, vertexData.tangentData, vector, exteriorAngles[1], vertexData.tangentDataStride);
            sub_10022CA0(i2, vertexData.tangentData, vector, exteriorAngles[2], vertexData.tangentDataStride);
            sub_10022CA0(i0, vertexData.binormalData, cross, exteriorAngles[0], vertexData.binormalDataStride);
            sub_10022CA0(i1, vertexData.binormalData, cross, exteriorAngles[1], vertexData.binormalDataStride);
            sub_10022CA0(i2, vertexData.binormalData, cross, exteriorAngles[2], vertexData.binormalDataStride);
        }

        for (auto vertexIndex = 0u; vertexIndex < vertexCount; vertexIndex++)
        {
            const auto& normal = GetVec3(vertexData.normalData, vertexIndex, vertexData.normalDataStride);
            auto& tangent = GetVec3(vertexData.tangentData, vertexIndex, vertexData.tangentDataStride);
            auto& binormal = GetVec3(vertexData.binormalData, vertexIndex, vertexData.binormalDataStride);

            const auto dot_normal_tangent = normal[0] * tangent[0] + normal[1] * tangent[1] + normal[2] * tangent[2];

            tangent[0] = normal[0] * -dot_normal_tangent + tangent[0];
            tangent[1] = normal[1] * -dot_normal_tangent + tangent[1];
            tangent[2] = normal[2] * -dot_normal_tangent + tangent[2];
            if (Vec3_Normalize(tangent) < 0.001f)
            {
                Vec3_Cross(binormal, normal, tangent);
                if (Vec3_Normalize(tangent) < 0.001)
                    sub_10014EE0(normal, tangent);
            }

            tvec3 cross;
            Vec3_Cross(normal, tangent, cross);
            const auto sourcesc = binormal[0] * cross[0] + cross[1] * binormal[1] + cross[2] * binormal[2];
            if (sourcesc >= 0.0)
            {
                binormal[0] = cross[0];
                binormal[1] = cross[1];
                binormal[2] = cross[2];
            }
            else
            {
                binormal[0] = -cross[0];
                binormal[1] = -cross[1];
                binormal[2] = -cross[2];
            }
        }
    }
} // namespace tangent_space
