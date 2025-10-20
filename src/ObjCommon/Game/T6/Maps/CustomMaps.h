#pragma once

#include <vector>
#include <string>

#include "Game/T6/T6.h"
using namespace T6;

struct CustomMapVertex
{
    vec3_t pos;
    vec4_t color;
    vec2_t texCoord;
    vec3_t normal;
    vec3_t tangent;
};

enum CustomMapMaterialType
{
    MATERIAL_TYPE_COLOUR,
    MATERIAL_TYPE_TEXTURE,
    MATERIAL_TYPE_EMPTY
};

struct CustomMapMaterial
{
    CustomMapMaterialType materialType;
    std::string materialName;
};

struct CustomMapSurface
{
    CustomMapMaterial material;
    int triCount;
    int indexOfFirstVertex;
    int indexOfFirstIndex;
};

struct CustomMapWorld
{
    std::vector<CustomMapSurface> surfaces;
    std::vector<CustomMapVertex> vertices;
    std::vector<uint16_t> indices;
};

struct CustomMapBSP
{
    std::string name;
    std::string bspName;

    CustomMapWorld gfxWorld;
    CustomMapWorld colWorld;
};
