#pragma once

#include <type_traits>

#include "Game/IW4/IW4.h"

namespace IW4
{
    inline const char* techniqueTypeNames[]
    {
        "depth prepass",
        "build floatz",
        "build shadowmap depth",
        "build shadowmap color",
        "unlit",
        "emissive",
        "emissive dfog",
        "emissive shadow",
        "emissive shadow dfog",
        "lit",
        "lit dfog",
        "lit sun",
        "lit sun dfog",
        "lit sun shadow",
        "lit sun shadow dfog",
        "lit spot",
        "lit spot dfog",
        "lit spot shadow",
        "lit spot shadow dfog",
        "lit omni",
        "lit omni dfog",
        "lit omni shadow",
        "lit omni shadow dfog",
        "lit instanced",
        "lit instanced dfog",
        "lit instanced sun",
        "lit instanced sun dfog",
        "lit instanced sun shadow",
        "lit instanced sun shadow dfog",
        "lit instanced spot",
        "lit instanced spot dfog",
        "lit instanced spot shadow",
        "lit instanced spot shadow dfog",
        "lit instanced omni",
        "lit instanced omni dfog",
        "lit instanced omni shadow",
        "lit instanced omni shadow dfog",
        "light spot",
        "light omni",
        "light spot shadow",
        "fakelight normal",
        "fakelight view",
        "sunlight preview",
        "case texture",
        "solid wireframe",
        "shaded wireframe",
        "debug bumpmap",
        "debug bumpmap instanced",
    };

    static_assert(std::extent_v<decltype(techniqueTypeNames)> == TECHNIQUE_COUNT);
}