#pragma once

#include "Game/IW3/CommonIW3.h"
#include "Game/IW3/IW3.h"

#include <type_traits>
#include <unordered_map>

namespace IW3
{
    inline const char* techniqueTypeNames[]{
        "depth prepass",
        "build floatz",
        "build shadowmap depth",
        "build shadowmap color",
        "unlit",
        "emissive",
        "emissive shadow",
        "lit",
        "lit sun",
        "lit sun shadow",
        "lit spot",
        "lit spot shadow",
        "lit omni",
        "lit omni shadow",
        "lit instanced",
        "lit instanced sun",
        "lit instanced sun shadow",
        "lit instanced spot",
        "lit instanced spot shadow",
        "lit instanced omni",
        "lit instanced omni shadow",
        "light spot",
        "light omni",
        "light spot shadow",
        "fakelight normal",
        "fakelight view",
        "sunlight preview",
        "case texture",
        "solid wireframe",
        "shaded wireframe",
        "shadowcookie caster",
        "shadowcookie receiver",
        "debug bumpmap",
        "debug bumpmap instanced",
    };
    static_assert(std::extent_v<decltype(techniqueTypeNames)> == TECHNIQUE_COUNT);

    static const char* materialStreamDestinationNames[]{
        "position",
        "normal",
        "color[0]",
        "color[1]",
        "texcoord[0]",
        "texcoord[1]",
        "texcoord[2]",
        "texcoord[3]",
        "texcoord[4]",
        "texcoord[5]",
        "texcoord[6]",
        "texcoord[7]",
    };
    static_assert(std::extent_v<decltype(materialStreamDestinationNames)> == STREAM_DST_COUNT);

    static const char* materialStreamSourceNames[]{
        "position",
        "color",
        "texcoord[0]",
        "normal",
        "tangent",
        "texcoord[1]",
        "texcoord[2]",
        "normalTransform[0]",
        "normalTransform[1]",
    };
    static_assert(std::extent_v<decltype(materialStreamSourceNames)> == STREAM_SRC_COUNT);

    static constexpr std::pair<uint32_t, const char*> KnownMaterialSource(const char* name)
    {
        return std::make_pair(Common::R_HashString(name, 0u), name);
    }

    inline std::unordered_map knownMaterialSourceNames{
        KnownMaterialSource("colorMap"),        KnownMaterialSource("colorMap0"),       KnownMaterialSource("colorMap1"),
        KnownMaterialSource("colorMap2"),       KnownMaterialSource("colorMap3"),       KnownMaterialSource("colorMap4"),
        KnownMaterialSource("colorMap5"),       KnownMaterialSource("colorMap6"),       KnownMaterialSource("colorMap7"),
        KnownMaterialSource("normalMap"),       KnownMaterialSource("normalMap0"),      KnownMaterialSource("normalMap1"),
        KnownMaterialSource("normalMap2"),      KnownMaterialSource("normalMap3"),      KnownMaterialSource("normalMap4"),
        KnownMaterialSource("normalMap5"),      KnownMaterialSource("normalMap6"),      KnownMaterialSource("normalMap7"),
        KnownMaterialSource("specularMap"),     KnownMaterialSource("specularMap0"),    KnownMaterialSource("specularMap1"),
        KnownMaterialSource("specularMap2"),    KnownMaterialSource("specularMap3"),    KnownMaterialSource("specularMap4"),
        KnownMaterialSource("specularMap5"),    KnownMaterialSource("specularMap6"),    KnownMaterialSource("specularMap7"),
        KnownMaterialSource("detailMap"),       KnownMaterialSource("detailMap0"),      KnownMaterialSource("detailMap1"),
        KnownMaterialSource("detailMap2"),      KnownMaterialSource("detailMap3"),      KnownMaterialSource("detailMap4"),
        KnownMaterialSource("detailMap5"),      KnownMaterialSource("detailMap6"),      KnownMaterialSource("detailMap7"),
        KnownMaterialSource("attenuationMap"),  KnownMaterialSource("attenuationMap0"), KnownMaterialSource("attenuationMap1"),
        KnownMaterialSource("attenuationMap2"), KnownMaterialSource("attenuationMap3"), KnownMaterialSource("attenuationMap4"),
        KnownMaterialSource("attenuationMap5"), KnownMaterialSource("attenuationMap6"), KnownMaterialSource("attenuationMap7"),
        KnownMaterialSource("distortionScale"), KnownMaterialSource("eyeOffsetParms"),  KnownMaterialSource("falloffBeginColor"),
        KnownMaterialSource("falloffEndColor"),
    };
} // namespace IW3
