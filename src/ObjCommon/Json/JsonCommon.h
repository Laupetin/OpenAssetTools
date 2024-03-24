#pragma once

#include <nlohmann/json.hpp>

class JsonVec2
{
public:
    float x;
    float y;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonVec2, x, y);

class JsonVec3
{
public:
    float x;
    float y;
    float z;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonVec3, x, y, z);

class JsonVec4
{
public:
    float x;
    float y;
    float z;
    float w;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonVec4, x, y, z, w);
