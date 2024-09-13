#pragma once

#include "Json/JsonExtension.h"

#pragma warning(push, 0)
#include <nlohmann/json.hpp>
#pragma warning(pop)

class JsonVec2
{
public:
    float x;
    float y;
};

NLOHMANN_DEFINE_TYPE_EXTENSION(JsonVec2, x, y);

class JsonVec3
{
public:
    float x;
    float y;
    float z;
};

NLOHMANN_DEFINE_TYPE_EXTENSION(JsonVec3, x, y, z);

class JsonVec4
{
public:
    float x;
    float y;
    float z;
    float w;
};

NLOHMANN_DEFINE_TYPE_EXTENSION(JsonVec4, x, y, z, w);
