#include "XModelCommon.h"

#include <cmath>
#include <limits>
#include <tuple>

void XModelMaterial::ApplyDefaults()
{
    // Phong = Color, Bump, Spec, CosinePower
    // Blinn = Color, Bump, Spec, Eccentricity
    // Lambert = Color, Bump
    materialTypeName = "Phong";
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    color[3] = 1;
    transparency[0] = 0;
    transparency[1] = 0;
    transparency[2] = 0;
    transparency[3] = 1;
    ambientColor[0] = 0;
    ambientColor[1] = 0;
    ambientColor[2] = 0;
    ambientColor[3] = 1;
    incandescence[0] = 0;
    incandescence[1] = 0;
    incandescence[2] = 0;
    incandescence[3] = 1;
    coeffs[0] = 0.8f;
    coeffs[1] = 0;
    glow.x = 0;
    glow.y = 0;
    refractive.x = 6;
    refractive.y = 1;
    specularColor[0] = -1;
    specularColor[1] = -1;
    specularColor[2] = -1;
    specularColor[3] = 1;
    reflectiveColor[0] = -1;
    reflectiveColor[1] = -1;
    reflectiveColor[2] = -1;
    reflectiveColor[3] = 1;
    reflective.x = -1;
    reflective.y = -1;
    blinn[0] = -1;
    blinn[1] = -1;
    phong = -1;
}

bool operator==(const VertexMergerPos& lhs, const VertexMergerPos& rhs)
{
    const auto coordinatesMatch = std::fabs(lhs.x - rhs.x) < std::numeric_limits<float>::epsilon()
                                  && std::fabs(lhs.y - rhs.y) < std::numeric_limits<float>::epsilon()
                                  && std::fabs(lhs.z - rhs.z) < std::numeric_limits<float>::epsilon();

    if (!coordinatesMatch || lhs.weightCount != rhs.weightCount)
        return false;

    for (auto weightIndex = 0u; weightIndex < lhs.weightCount; weightIndex++)
    {
        if (lhs.weights[weightIndex].boneIndex != rhs.weights[weightIndex].boneIndex
            || std::fabs(lhs.weights[weightIndex].weight - rhs.weights[weightIndex].weight) >= std::numeric_limits<float>::epsilon())
        {
            return false;
        }
    }

    return true;
}

bool operator!=(const VertexMergerPos& lhs, VertexMergerPos& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const VertexMergerPos& lhs, const VertexMergerPos& rhs)
{
    const auto t0 = std::tie(lhs.x, lhs.y, lhs.z, rhs.weightCount);
    const auto t1 = std::tie(rhs.x, rhs.y, rhs.z, rhs.weightCount);
    if (t0 < t1)
        return true;

    if (!(t0 == t1))
        return false;

    for (auto weightIndex = 0u; weightIndex < lhs.weightCount; weightIndex++)
    {
        const auto& lhsWeight = lhs.weights[weightIndex];
        const auto& rhsWeight = rhs.weights[weightIndex];

        const auto t2 = std::tie(lhsWeight.boneIndex, lhsWeight.weight);
        const auto t3 = std::tie(rhsWeight.boneIndex, rhsWeight.weight);
        if (t2 < t3)
            return true;

        if (!(t2 == t3))
            return false;
    }

    return false;
}
