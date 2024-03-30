#pragma once

#include <string>

struct ObjObject
{
    std::string name;
    int materialIndex;
};

struct ObjVertex
{
    float coordinates[3];

    friend bool operator==(const ObjVertex& lhs, const ObjVertex& rhs);
    friend bool operator!=(const ObjVertex& lhs, const ObjVertex& rhs);
    friend bool operator<(const ObjVertex& lhs, const ObjVertex& rhs);
};

struct ObjNormal
{
    float normal[3];

    friend bool operator==(const ObjNormal& lhs, const ObjNormal& rhs);
    friend bool operator!=(const ObjNormal& lhs, const ObjNormal& rhs);
    friend bool operator<(const ObjNormal& lhs, const ObjNormal& rhs);
};

struct ObjUv
{
    float uv[2];

    friend bool operator==(const ObjUv& lhs, const ObjUv& rhs);
    friend bool operator!=(const ObjUv& lhs, const ObjUv& rhs);
    friend bool operator<(const ObjUv& lhs, const ObjUv& rhs);
};

struct ObjFace
{
    int vertexIndex[3];
    int normalIndex[3];
    int uvIndex[3];
};

struct MtlMaterial
{
    std::string materialName;
    std::string colorMapName;
    std::string normalMapName;
    std::string specularMapName;
};
