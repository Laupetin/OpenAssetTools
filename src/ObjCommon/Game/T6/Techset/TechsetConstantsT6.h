#pragma once

#include "Game/T6/T6.h"
#include "Techset/CommonTechnique.h"

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

    static techset::CommonStreamRoutingSourceInfo streamRoutingSources[]{
        {
         .name = "position",
         .abbreviation = "p",
         .optional = false,
         },
        {
         .name = "color",
         .abbreviation = "c",
         .optional = false,
         },
        {
         .name = "texcoord[0]",
         .abbreviation = "t0",
         .optional = false,
         },
        {
         .name = "normal",
         .abbreviation = "n",
         .optional = false,
         },
        {
         .name = "tangent",
         .abbreviation = "t",
         .optional = false,
         },
        {
         .name = "texcoord[1]",
         .abbreviation = "t1",
         .optional = false,
         },
        {
         .name = "texcoord[2]",
         .abbreviation = "t2",
         .optional = true,
         },
        {
         .name = "texcoord[3]",
         .abbreviation = "t3",
         .optional = true,
         },
        {
         .name = "normalTransform[0]",
         .abbreviation = "n0",
         .optional = true,
         },
        {
         .name = "normalTransform[1]",
         .abbreviation = "n1",
         .optional = true,
         },
        {
         .name = "blendWeight",
         .abbreviation = "b",
         .optional = true,
         },
    };
    static_assert(std::extent_v<decltype(streamRoutingSources)> == STREAM_SRC_COUNT);

    static techset::CommonStreamRoutingDestinationInfo streamRoutingDestinations[]{
        {
         .name = "position",
         .abbreviation = "p",
         },
        {
         .name = "normal",
         .abbreviation = "n",
         },
        {
         .name = "color[0]",
         .abbreviation = "c0",
         },
        {
         .name = "color[1]",
         .abbreviation = "c1",
         },
        {
         .name = "depth",
         .abbreviation = "d",
         },
        {
         .name = "texcoord[0]",
         .abbreviation = "t0",
         },
        {
         .name = "texcoord[1]",
         .abbreviation = "t1",
         },
        {
         .name = "texcoord[2]",
         .abbreviation = "t2",
         },
        {
         .name = "texcoord[3]",
         .abbreviation = "t3",
         },
        {
         .name = "texcoord[4]",
         .abbreviation = "t4",
         },
        {
         .name = "texcoord[5]",
         .abbreviation = "t5",
         },
        {
         .name = "texcoord[6]",
         .abbreviation = "t6",
         },
        {
         .name = "texcoord[7]",
         .abbreviation = "t7",
         },
        {
         .name = "texcoord[8]",
         .abbreviation = "t8",
         },
        {
         .name = "texcoord[9]",
         .abbreviation = "t9",
         },
        {
         .name = "texcoord[10]",
         .abbreviation = "t10",
         },
        {
         .name = "texcoord[11]",
         .abbreviation = "t11",
         },
        {
         .name = "texcoord[12]",
         .abbreviation = "t12",
         },
        {
         .name = "texcoord[13]",
         .abbreviation = "t13",
         },
        {
         .name = "blendWeight",
         .abbreviation = "b",
         },
    };
    static_assert(std::extent_v<decltype(streamRoutingDestinations)> == STREAM_DST_COUNT);

    inline MaterialTypeInfo g_materialTypeInfo[]{
        {"",     ""    },
        {"m/",   "m_"  },
        {"mc/",  "mc_" },
        {"mlv/", "mlv_"},
        {"wc/",  "wc_" },
        {"wpc/", "wpc_"},
        {"wq/",  "wq_" },
        {"wqc/", "wqc_"},
    };
    static_assert(std::extent_v<decltype(g_materialTypeInfo)> == MTL_TYPE_COUNT);
} // namespace T6
