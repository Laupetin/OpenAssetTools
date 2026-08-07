#include "XModelHighMipVolumeT5.h"

#include "Game/T5/CommonT5.h"

#include <algorithm>
#include <cassert>
#include <cmath>

using namespace T5;

namespace
{
    unsigned XModelGetSurfaces(const XModel& model, XSurface*& surfaces, const unsigned lodIndex)
    {
        assert(lodIndex < std::extent_v<decltype(XModel::lodInfo)>);

        const auto& lod = model.lodInfo[lodIndex];

        assert(lod.surfIndex < model.numsurfs);
        assert(lod.numsurfs + lod.surfIndex <= model.numsurfs);

        surfaces = &model.surfs[lod.surfIndex];
        return lod.numsurfs;
    }

    Material** XModelGetSkins(const XModel& model, const unsigned lodIndex)
    {
        assert(lodIndex < std::extent_v<decltype(XModel::lodInfo)>);

        return &model.materialHandles[model.lodInfo[lodIndex].surfIndex];
    }

    void ClearBounds(vec3_t& mins, vec3_t& maxs)
    {
        mins.x = std::numeric_limits<float>::max();
        mins.y = std::numeric_limits<float>::max();
        mins.z = std::numeric_limits<float>::max();

        maxs.x = -std::numeric_limits<float>::max();
        maxs.y = -std::numeric_limits<float>::max();
        maxs.z = -std::numeric_limits<float>::max();
    }

    bool R_StreamGetMaterialTextureSize(const Material& material, const unsigned texIndex, vec2_t& texSize, unsigned& filterState)
    {
        assert(texIndex < material.textureCount);
        const auto& texDef = material.textureTable[texIndex];

        if (texDef.semantic == TS_WATER_MAP || !texDef.u.image->streaming)
            return false;

        assert(texDef.u.image);

        const auto* image = texDef.u.image;

        texSize.x = static_cast<float>(image->width);
        texSize.y = static_cast<float>(image->height);
        filterState = texDef.samplerState.filter;

        return true;
    }

    float PointToLineDistSq2D(const vec2_t& point, const vec2_t& start, const vec2_t& end)
    {
        const auto dx = end.x - start.x;
        const auto dy = end.y - start.y;

        const auto px = point.x - start.x;
        const auto py = point.y - start.y;

        const auto segDot = dx * dx + dy * dy;

        if (segDot == 0.0f)
            return 0.0f;

        const auto proj = -(px * dx + py * dy) / segDot;

        const auto projX = dx * proj + px;
        const auto projY = dy * proj + py;

        return projX * projX + projY * projY;
    }

    float R_CalculateTriangleArea2D(const vec2_t (&texCoord)[3])
    {
        if (texCoord[0].x == texCoord[1].x && texCoord[0].y == texCoord[1].y)
            return 0.0;
        if (texCoord[0].x == texCoord[2].x && texCoord[0].y == texCoord[2].y)
            return 0.0;
        if (texCoord[1].x == texCoord[2].x && texCoord[1].y == texCoord[2].y)
            return 0.0;

        const auto v7 = texCoord[2].x - texCoord[1].x;
        const auto v8 = texCoord[2].y - texCoord[1].y;
        const auto v6 = PointToLineDistSq2D(texCoord[0], texCoord[1], texCoord[2]) * (v7 * v7 + v8 * v8);

        return std::sqrtf(v6) * 0.5f;
    }

    float PointToLineDistSq(const vec3_t& point, const vec3_t& start, const vec3_t& end)
    {
        const vec3_t seg{
            .x = end.x - start.x,
            .y = end.y - start.y,
            .z = end.z - start.z,
        };
        const vec3_t ptToLine{
            .x = point.x - start.x,
            .y = point.y - start.y,
            .z = point.z - start.z,
        };

        const auto segDot = seg.x * seg.x + seg.y * seg.y + seg.z * seg.z;
        assert(segDot);

        const auto ptToLineDot = ptToLine.x * seg.x + ptToLine.y * seg.y + ptToLine.z * seg.z;

        return (-(ptToLineDot / segDot) * seg.z + ptToLine.z) * (-(ptToLineDot / segDot) * seg.z + ptToLine.z)
               + (-(ptToLineDot / segDot) * seg.y + ptToLine.y) * (-(ptToLineDot / segDot) * seg.y + ptToLine.y)
               + (-(ptToLineDot / segDot) * seg.x + ptToLine.x) * (-(ptToLineDot / segDot) * seg.x + ptToLine.x);
    }

    float Vec3LengthSq(const vec3_t& v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    float Vec3DistanceSq(const vec3_t& p1, const vec3_t& p2)
    {
        const vec3_t v{
            .x = p2.x - p1.x,
            .y = p2.y - p1.y,
            .z = p2.z - p1.z,
        };

        return Vec3LengthSq(v);
    }

    float R_CalculateTriangleArea3D(const vec3_t (&coord)[3])
    {
        if (coord[0].x == coord[1].x && coord[0].y == coord[1].y && coord[0].z == coord[1].z)
            return 0.0;
        if (coord[0].x == coord[2].x && coord[0].y == coord[2].y && coord[0].z == coord[2].z)
            return 0.0;
        if (coord[1].x == coord[2].x && coord[1].y == coord[2].y && coord[1].z == coord[2].z)
            return 0.0;

        const auto v3 = PointToLineDistSq(coord[0], coord[1], coord[2]);
        const auto v7 = Vec3DistanceSq(coord[1], coord[2]) * v3;

        return sqrtf(v7) * 0.5f;
    }

    void AddPointToBounds(const vec3_t& v, vec3_t& mins, vec3_t& maxs)
    {
        mins.x = std::min(mins.x, v.x);
        maxs.x = std::max(v.x, maxs.x);
        mins.y = std::min(mins.y, v.y);
        maxs.y = std::max(v.y, maxs.y);
        mins.z = std::min(mins.z, v.z);
        maxs.z = std::max(v.z, maxs.z);
    }

    void Vec3Cross(const float (&v0)[3], const float (&v1)[3], vec3_t& cross)
    {
        cross.x = v0[1] * v1[2] - v0[2] * v1[1];
        cross.y = v0[2] * v1[0] - v0[0] * v1[2];
        cross.z = v0[0] * v1[1] - v0[1] * v1[0];
    }

    float Vec3Normalize(vec3_t& v)
    {
        const auto length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

        float ilength;
        if (length > 0.0f)
            ilength = (1.0f / length);
        else
            ilength = 1.0f;

        v.x = v.x * ilength;
        v.y = v.y * ilength;
        v.z = v.z * ilength;

        return length;
    }

    void MatrixInverse(const float (&in)[3][3], float (&out)[3][3])
    {
        const auto det = (in[2][2] * in[1][1] - in[2][1] * in[1][2]) * in[0][0] - (in[2][2] * in[0][1] - in[2][1] * in[0][2]) * in[1][0]
                         + (in[1][2] * in[0][1] - in[1][1] * in[0][2]) * in[2][0];

        assert(det);
        const auto deta = 1.0f / det;
        out[0][0] = (in[2][2] * in[1][1] - in[2][1] * in[1][2]) * deta;
        out[0][1] = -(in[2][2] * in[0][1] - in[2][1] * in[0][2]) * deta;
        out[0][2] = (in[1][2] * in[0][1] - in[1][1] * in[0][2]) * deta;
        out[1][0] = -(in[2][2] * in[1][0] - in[2][0] * in[1][2]) * deta;
        out[1][1] = (in[2][2] * in[0][0] - in[2][0] * in[0][2]) * deta;
        out[1][2] = -(in[1][2] * in[0][0] - in[1][0] * in[0][2]) * deta;
        out[2][0] = (in[2][1] * in[1][0] - in[2][0] * in[1][1]) * deta;
        out[2][1] = -(in[2][1] * in[0][0] - in[2][0] * in[0][1]) * deta;
        out[2][2] = (in[1][1] * in[0][0] - in[1][0] * in[0][1]) * deta;
    }

    bool R_CalculateTriangleTextureGradient(const vec3_t (&pos)[3], const vec2_t (&texCoord)[3], vec2_t (&outTexGradient)[3], vec3_t& outNormal)
    {
        float inputMatrix[3][3];
        inputMatrix[0][0] = pos[1].x - pos[0].x;
        inputMatrix[0][1] = pos[1].y - pos[0].y;
        inputMatrix[0][2] = pos[1].z - pos[0].z;
        inputMatrix[1][0] = pos[2].x - pos[0].x;
        inputMatrix[1][1] = pos[2].y - pos[0].y;
        inputMatrix[1][2] = pos[2].z - pos[0].z;

        Vec3Cross(inputMatrix[1], inputMatrix[0], outNormal);
        if (outNormal.x * outNormal.x + outNormal.y * outNormal.y + outNormal.z * outNormal.z == 0.0f)
            return false;

        Vec3Normalize(outNormal);
        inputMatrix[2][0] = outNormal.x;
        inputMatrix[2][1] = outNormal.y;
        inputMatrix[2][2] = outNormal.z;

        float outputValues[2][3];
        outputValues[0][0] = texCoord[1].x - texCoord[0].x;
        outputValues[0][1] = texCoord[1].y - texCoord[0].y;
        outputValues[1][0] = texCoord[2].x - texCoord[0].x;
        outputValues[1][1] = texCoord[2].y - texCoord[0].y;

        float inputMatrixInv[3][3];
        MatrixInverse(inputMatrix, inputMatrixInv);
        for (auto xyzDim = 0u; xyzDim < 3; ++xyzDim)
        {
            for (auto texCoordDim = 0u; texCoordDim < 2; ++texCoordDim)
            {
                outTexGradient[xyzDim].v[texCoordDim] =
                    inputMatrixInv[xyzDim][0] * outputValues[0][texCoordDim] + inputMatrixInv[xyzDim][1] * outputValues[1][texCoordDim];
            }
        }

        return true;
    }

    float R_CalculateTexelDensityFromGradient(const vec2_t (&texGradient)[3], const vec2_t& textureSize)
    {
        vec2_t texGradientLengthSq;

        texGradientLengthSq.x =
            (texGradient[0].x * texGradient[0].x + texGradient[0].y * texGradient[0].y + texGradient[1].x * texGradient[1].x) * textureSize.x * textureSize.x;
        texGradientLengthSq.y =
            (texGradient[1].y * texGradient[1].y + texGradient[2].x * texGradient[2].x + texGradient[2].y * texGradient[2].y) * textureSize.y * textureSize.y;

        if (texGradientLengthSq.y >= 0.000001f)
        {
            if (texGradientLengthSq.x >= 0.000001f)
            {
                if (texGradientLengthSq.y <= texGradientLengthSq.x)
                    return 1.0f / sqrtf(texGradientLengthSq.y);

                return 1.0f / sqrtf(texGradientLengthSq.x);
            }

            return 1.0f / sqrtf(texGradientLengthSq.y);
        }

        return 1.0f / sqrtf(texGradientLengthSq.x);
    }

    bool R_CalculateTriangleTopMipAabb(
        const vec3_t (&pos)[3], const vec2_t (&texCoord)[3], const vec2_t& textureSize, const unsigned char filterState, float* outRadius)
    {
        vec3_t normal;
        vec3_t viewSphereRadii;
        vec3_t viewSphereCenters[3];
        vec2_t texGradient[3];

        float MAX_RADIUS = sqrtf(5.0e11f) * 0.99000001f;

        if (!R_CalculateTriangleTextureGradient(pos, texCoord, texGradient, normal))
            return false;

        if (texGradient[0].x * texGradient[0].x + texGradient[1].x * texGradient[1].x + texGradient[2].x * texGradient[2].x <= 0.0
            && texGradient[0].y * texGradient[0].y + texGradient[1].y * texGradient[1].y + texGradient[2].y * texGradient[2].y <= 0.0)
        {
            return false;
        }

        viewSphereRadii.x = R_CalculateTexelDensityFromGradient(texGradient, textureSize) * 800.0f;
        unsigned filterState_FilterMasked = filterState & 7;

        if (filterState_FilterMasked == 3 || filterState_FilterMasked == 4)
        {
            if (filterState_FilterMasked == 4)
                viewSphereRadii.x = viewSphereRadii.x * 4.0f;
            else
                viewSphereRadii.x = viewSphereRadii.x * 2.0f;

            for (auto triCorner = 0u; triCorner < 3u; ++triCorner)
            {
                viewSphereCenters[triCorner] = pos[triCorner];
            }
        }
        else
        {
            if ((filterState & 0x18) == 8)
                viewSphereRadii.x = viewSphereRadii.x * 0.5f;

            for (auto triCorner = 0; triCorner < 3; ++triCorner)
            {
                viewSphereCenters[triCorner].x = viewSphereRadii.x * normal.x + pos[triCorner].x;
                viewSphereCenters[triCorner].y = viewSphereRadii.x * normal.y + pos[triCorner].y;
                viewSphereCenters[triCorner].z = viewSphereRadii.x * normal.z + pos[triCorner].z;
            }
        }

        viewSphereRadii.x = std::min(viewSphereRadii.x, MAX_RADIUS);
        viewSphereRadii.y = viewSphereRadii.x;
        viewSphereRadii.z = viewSphereRadii.x;
        *outRadius = viewSphereRadii.x;

        return true;
    }

    float BoxMaxDimension(const vec3_t& mins, const vec3_t& maxs)
    {
        return std::max({maxs.x - mins.x, maxs.y - mins.y, maxs.z - mins.z});
    }

    void Vec3Lerp(const vec3_t& start, const vec3_t& end, const float fraction, vec3_t& endPos)
    {
        endPos.x = (end.x - start.x) * fraction + start.x;
        endPos.y = (end.y - start.y) * fraction + start.y;
        endPos.z = (end.z - start.z) * fraction + start.z;
    }

    float Vec3Length(const vec3_t& v)
    {
        return sqrtf(v.z * v.z + v.y * v.y + v.x * v.x);
    }

    float Vec3Distance(const vec3_t& v1, const vec3_t& v2)
    {
        vec3_t dir;

        dir.x = v2.x - v1.x;
        dir.y = v2.y - v1.y;
        dir.z = v2.z - v1.z;

        return Vec3Length(dir);
    }

    bool Material_IncludesImage(const Material& mat, const GfxImage* image)
    {
        for (int i = 0u; i < mat.textureCount; ++i)
        {
            if (mat.textureTable[i].u.image == image)
                return true;
        }

        return false;
    }

    bool Material_FirstIncludesSecond(const Material* mat1, const Material* mat2)
    {
        if (mat1 == mat2)
            return true;

        for (auto i = 0u; i < mat2->textureCount; ++i)
        {
            if (!Material_IncludesImage(*mat1, mat2->textureTable[i].u.image))
                return false;
        }

        return true;
    }

    int VecNCompareCustomEpsilon(const float* v0, const float* v1, const float epsilon, const int coordCount)
    {
        for (int i = 0; i < coordCount; ++i)
        {
            if ((v0[i] - v1[i]) * (v0[i] - v1[i]) > epsilon * epsilon)
                return 0;
        }

        return 1;
    }
} // namespace

namespace xmodel
{
    XModelHighMipBounds* GenerateHighMipVolumeT5(const XModel& model, MemoryManager& memory)
    {
        auto* result = memory.Alloc<XModelHighMipBounds>(model.numsurfs);

        unsigned surfCountPrevLods = 0;
        for (auto lod = 0u; lod < std::extent_v<decltype(XModel::lodInfo)>; ++lod)
        {
            XSurface* surfaces;
            const auto surfCount = XModelGetSurfaces(model, surfaces, lod);
            const auto* materials = XModelGetSkins(model, lod);

            for (auto surfIter = 0u; surfIter < surfCount; ++surfIter)
            {
                assert(surfIter + surfCountPrevLods < model.numsurfs);

                XModelHighMipBounds* bounds = &result[surfIter + surfCountPrevLods];

                const auto& surface = surfaces[surfIter];
                const auto& material = *materials[surfIter];

                vec3_t surfBoundBoxMins;
                vec3_t surfBoundBoxMaxs;
                ClearBounds(surfBoundBoxMins, surfBoundBoxMaxs);

                unsigned maxFilterState = TEXTURE_FILTER_NEAREST;
                float radiusForModel = 0.0f;
                for (auto texIter = 0u; texIter < material.textureCount; ++texIter)
                {
                    float cumulativeCoverage = 0.0f;
                    float cumulativeArea = 0.0f;
                    float radiusForTexture = 0.0f;

                    vec2_t texSize;
                    unsigned filterState;
                    if (R_StreamGetMaterialTextureSize(material, texIter, texSize, filterState))
                    {
                        maxFilterState = std::max(filterState, maxFilterState);

                        for (auto triangleIndex = 0u; triangleIndex < surface.triCount; ++triangleIndex)
                        {
                            vec3_t triPos[3];
                            vec2_t triTexCoord[3];
                            for (auto triCorner = 0u; triCorner < 3u; ++triCorner)
                            {
                                const auto vertIndex = surface.triIndices[triangleIndex].i[triCorner];
                                const auto& vert = surface.verts0[vertIndex];

                                triPos[triCorner] = vert.xyz;
                                Common::Vec2UnpackTexCoords(vert.texCoord, triTexCoord[triCorner].v);
                            }

                            const auto coverage = R_CalculateTriangleArea2D(triTexCoord);
                            const auto area = R_CalculateTriangleArea3D(triPos);

                            if (coverage > 0.0 && area > 0.0)
                            {
                                AddPointToBounds(triPos[0], surfBoundBoxMins, surfBoundBoxMaxs);
                                AddPointToBounds(triPos[1], surfBoundBoxMins, surfBoundBoxMaxs);
                                AddPointToBounds(triPos[2], surfBoundBoxMins, surfBoundBoxMaxs);
                                cumulativeCoverage = cumulativeCoverage + coverage;
                                cumulativeArea = cumulativeArea + area;
                            }

                            ++triangleIndex;
                        }

                        const auto uvMax = sqrtf(cumulativeCoverage);
                        const auto width = sqrtf(cumulativeArea);

                        vec3_t fakePos[3]{
                            {.x = 0.0f,  .y = 0.0f,  .z = 0.0f},
                            {.x = 0.0f,  .y = width, .z = 0.0f},
                            {.x = width, .y = 0.0f,  .z = 0.0f},
                        };

                        vec2_t fakeTexCoord[3]{
                            {.x = 0.0f,  .y = 0.0f },
                            {.x = 0.0f,  .y = uvMax},
                            {.x = uvMax, .y = 0.0f },
                        };

                        if (R_CalculateTriangleTopMipAabb(fakePos, fakeTexCoord, texSize, 2, &radiusForTexture))
                        {
                            radiusForModel = std::max(radiusForModel, radiusForTexture);
                        }
                    }
                }

                radiusForModel = BoxMaxDimension(surfBoundBoxMins, surfBoundBoxMaxs) * 0.5f + radiusForModel;
                bounds->himipRadiusSq = radiusForModel * radiusForModel;
                Vec3Lerp(surfBoundBoxMins, surfBoundBoxMaxs, 0.5f, bounds->center);
            }

            surfCountPrevLods += surfCount;
        }

        for (auto surfI = 1u; surfI < model.numsurfs; ++surfI)
        {
            for (auto surfJ = 0u; surfJ < surfI; ++surfJ)
            {
                auto* v2 = &result[surfI];

                if (Material_FirstIncludesSecond(model.materialHandles[surfJ], model.materialHandles[surfI]))
                {
                    XModelHighMipBounds* boundsJ = &result[surfJ];
                    const auto dist = Vec3Distance(boundsJ->center, v2->center);
                    const auto radius = sqrtf(v2->himipRadiusSq);
                    const auto radiusJ = sqrtf(boundsJ->himipRadiusSq);

                    if (radiusJ >= dist + radius)
                    {
                        v2->himipRadiusSq = 0.0f;
                        break;
                    }

                    if (radius >= dist + radiusJ)
                    {
                        boundsJ->himipRadiusSq = v2->himipRadiusSq;
                        boundsJ->center.x = v2->center.x;
                        boundsJ->center.y = v2->center.y;
                        boundsJ->center.z = v2->center.z;
                        v2->himipRadiusSq = 0.0f;
                        break;
                    }

                    const float newRadius = (radius + dist + radiusJ) / 2.0f;

                    float var14C;
                    if (radius - radiusJ < 0.0f)
                        var14C = radiusJ;
                    else
                        var14C = radius;
                    if (newRadius / var14C <= 1.05f)
                    {
                        boundsJ->himipRadiusSq = newRadius * newRadius;
                        if (!VecNCompareCustomEpsilon(v2->center.v, boundsJ->center.v, 0.001f, std::extent_v<decltype(vec3_t::v)>))
                        {
                            Vec3Lerp(v2->center, boundsJ->center, (newRadius - radius) / (newRadius - radius + newRadius - radiusJ), boundsJ->center);
                        }

                        v2->himipRadiusSq = 0.0f;
                        break;
                    }
                }
            }
        }

        return result;
    }
} // namespace xmodel
