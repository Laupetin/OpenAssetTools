#include "ObjCommon.h"

#include <cmath>
#include <limits>
#include <tuple>

bool operator==(const ObjVertex& lhs, const ObjVertex& rhs)
{
    return std::fabs(lhs.coordinates[0] - rhs.coordinates[0]) < std::numeric_limits<float>::epsilon()
           && std::fabs(lhs.coordinates[1] - rhs.coordinates[1]) < std::numeric_limits<float>::epsilon()
           && std::fabs(lhs.coordinates[2] - rhs.coordinates[2]) < std::numeric_limits<float>::epsilon();
}

bool operator!=(const ObjVertex& lhs, const ObjVertex& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const ObjVertex& lhs, const ObjVertex& rhs)
{
    return std::tie(lhs.coordinates[0], lhs.coordinates[1], lhs.coordinates[2]) < std::tie(rhs.coordinates[0], rhs.coordinates[1], rhs.coordinates[2]);
}

bool operator==(const ObjNormal& lhs, const ObjNormal& rhs)
{
    return std::fabs(lhs.normal[0] - rhs.normal[0]) < std::numeric_limits<float>::epsilon()
           && std::fabs(lhs.normal[1] - rhs.normal[1]) < std::numeric_limits<float>::epsilon()
           && std::fabs(lhs.normal[2] - rhs.normal[2]) < std::numeric_limits<float>::epsilon();
}

bool operator!=(const ObjNormal& lhs, const ObjNormal& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const ObjNormal& lhs, const ObjNormal& rhs)
{
    return std::tie(lhs.normal[0], lhs.normal[1], lhs.normal[2]) < std::tie(rhs.normal[0], rhs.normal[1], rhs.normal[2]);
}

bool operator==(const ObjUv& lhs, const ObjUv& rhs)
{
    return std::fabs(lhs.uv[0] - rhs.uv[0]) < std::numeric_limits<float>::epsilon() && std::fabs(lhs.uv[1] - rhs.uv[1]) < std::numeric_limits<float>::epsilon();
}

bool operator!=(const ObjUv& lhs, const ObjUv& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const ObjUv& lhs, const ObjUv& rhs)
{
    return std::tie(lhs.uv[0], lhs.uv[1]) < std::tie(rhs.uv[0], rhs.uv[1]);
}
