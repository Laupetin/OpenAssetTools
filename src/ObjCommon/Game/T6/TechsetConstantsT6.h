#pragma once

#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"

namespace T6
{
    inline const char* techniqueTypeNames[]{
        "depth prepass",
        "build shadowmap depth",
        "unlit",
        "emissive",
        "lit",
        "lit sun",
        "lit sun shadow",
        "lit spot",
        "lit spot shadow",
        "lit spot square",
        "lit spot square shadow",
        "lit spot round",
        "lit spot round shadow",
        "lit omni",
        "lit omni shadow",
        "lit dlight glight",
        "lit sun dlight glight",
        "lit sun shadow dlight glight",
        "lit spot dlight glight",
        "lit spot shadow dlight glight",
        "lit spot square dlight glight",
        "lit spot square shadow dlight glight",
        "lit spot round dlight glight",
        "lit spot round shadow dlight glight",
        "lit omni dlight glight",
        "lit omni shadow dlight glight",
        "light spot",
        "light omni",
        "fakelight normal",
        "fakelight view",
        "sunlight preview",
        "case texture",
        "solid wireframe",
        "shaded wireframe",
        "debug bumpmap",
        "debug performance",
    };
    static_assert(std::extent_v<decltype(techniqueTypeNames)> == TECHNIQUE_COUNT);

    static const char* materialStreamDestinationNames[]{
        "position",    "normal",       "color[0]",     "color[1]",     "depth",        "texcoord[0]", "texcoord[1]",
        "texcoord[2]", "texcoord[3]",  "texcoord[4]",  "texcoord[5]",  "texcoord[6]",  "texcoord[7]", "texcoord[8]",
        "texcoord[9]", "texcoord[10]", "texcoord[11]", "texcoord[12]", "texcoord[13]", "blendWeight",
    };
    static_assert(std::extent_v<decltype(materialStreamDestinationNames)> == STREAM_DST_COUNT);

    static const char* materialStreamDestinationAbbreviation[]{
        "p", "n", "c0", "c1", "d", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "t10", "t11", "t12", "t13", "b",
    };
    static_assert(std::extent_v<decltype(materialStreamDestinationAbbreviation)> == STREAM_DST_COUNT);

    static const char* materialStreamSourceNames[]{
        "position",
        "color",
        "texcoord[0]",
        "normal",
        "tangent",
        "texcoord[1]",
        "texcoord[2]",
        "texcoord[3]",
        "normalTransform[0]",
        "normalTransform[1]",
        "blendWeight",
    };
    static_assert(std::extent_v<decltype(materialStreamSourceNames)> == STREAM_SRC_COUNT);

    static const char* materialStreamSourceAbbreviation[]{
        "p",
        "c",
        "t0",
        "n",
        "t",
        "t1",
        "t2",
        "t3",
        "n0",
        "n1",
        "b",
    };
    static_assert(std::extent_v<decltype(materialStreamSourceAbbreviation)> == STREAM_SRC_COUNT);

    inline MaterialTypeInfo g_materialTypeInfo[]{
        {"",    ""   },
        {"m/",  "m_" },
        {"mc/", "mc_"},
        {"?",   "?"  },
        {"wc/", "wc_"},
        {"?",   "?"  },
        {"?",   "?"  },
        {"?",   "?"  },
    }; // TODO: Fill this
    static_assert(std::extent_v<decltype(g_materialTypeInfo)> == MTL_TYPE_COUNT);
} // namespace T6
