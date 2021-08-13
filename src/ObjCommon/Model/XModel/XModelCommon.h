#pragma once

#include <string>

#include "Math/Quaternion.h"

struct XModelObject
{
    std::string name;
};

struct XModelBone
{
    std::string name;
    int parentIndex;
    float scale[3];
    float offset[3];
    Quaternion32 rotation;
};

struct XModelVertex
{
    float coordinates[3];
    float normal[3];
    float color[4];
    float uv[2];
};

struct XModelFace
{
    int vertexIndex[3];
    int objectIndex;
    int materialIndex;
};

struct XModelMaterial
{
    std::string name;
    std::string materialTypeName;
    float color[4];
    float transparency[4];
    float ambientColor[4];
    float incandescence[4];
    float coeffs[2];
    struct {
        float x;
        int y;
    } glow;
    struct
    {
        int x;
        float y;
    } refractive;
    float specularColor[4];
    float reflectiveColor[4];
    struct
    {
        int x;
        float y;
    } reflective;
    float blinn[2];
    float phong;
    std::string colorMapName;

    void ApplyDefaults();
};