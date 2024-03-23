#pragma once

#include "Game/T6/T6.h"

#include "Json/JsonOptional.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace T6
{
    NLOHMANN_JSON_SERIALIZE_ENUM(GfxStencilOp,
                                 {
                                     {GFXS_STENCILOP_KEEP,    "keep"   },
                                     {GFXS_STENCILOP_ZERO,    "zero"   },
                                     {GFXS_STENCILOP_REPLACE, "replace"},
                                     {GFXS_STENCILOP_INCRSAT, "incrsat"},
                                     {GFXS_STENCILOP_DECRSAT, "decrsat"},
                                     {GFXS_STENCILOP_INVERT,  "invert" },
                                     {GFXS_STENCILOP_INCR,    "incr"   },
                                     {GFXS_STENCILOP_DECR,    "decr"   },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(GfxStencilFunc,
                                 {
                                     {GFXS_STENCILFUNC_NEVER,        "never"       },
                                     {GFXS_STENCILFUNC_LESS,         "less"        },
                                     {GFXS_STENCILFUNC_EQUAL,        "equal"       },
                                     {GFXS_STENCILFUNC_LESSEQUAL,    "lessequal"   },
                                     {GFXS_STENCILFUNC_GREATER,      "greater"     },
                                     {GFXS_STENCILFUNC_NOTEQUAL,     "notequal"    },
                                     {GFXS_STENCILFUNC_GREATEREQUAL, "greaterequal"},
                                     {GFXS_STENCILFUNC_ALWAYS,       "always"      },
    });

    class JsonStencil
    {
    public:
        GfxStencilOp pass;
        GfxStencilOp fail;
        GfxStencilOp zfail;
        GfxStencilFunc func;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonStencil, pass, fail, zfail, func);

    enum class JsonAlphaTest
    {
        INVALID,
        DISABLED,
        GT0,
        GE128
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        JsonAlphaTest,
        {
            {JsonAlphaTest::INVALID,  nullptr   },
            {JsonAlphaTest::DISABLED, "disabled"},
            {JsonAlphaTest::GT0,      "gt0"     },
            {JsonAlphaTest::GE128,    "ge128"   }
    });

    enum class JsonCullFace
    {
        INVALID,
        NONE,
        BACK,
        FRONT
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        JsonCullFace,
        {
            {JsonCullFace::INVALID, nullptr},
            {JsonCullFace::NONE,    "none" },
            {JsonCullFace::BACK,    "back" },
            {JsonCullFace::FRONT,   "front"}
    });

    enum class JsonDepthTest
    {
        INVALID,
        DISABLED,
        ALWAYS,
        LESS,
        EQUAL,
        LESS_EQUAL
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(JsonDepthTest,
                                 {
                                     {JsonDepthTest::INVALID,    nullptr     },
                                     {JsonDepthTest::DISABLED,   "disabled"  },
                                     {JsonDepthTest::ALWAYS,     "always"    },
                                     {JsonDepthTest::LESS,       "less"      },
                                     {JsonDepthTest::EQUAL,      "equal"     },
                                     {JsonDepthTest::LESS_EQUAL, "less_equal"}
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(GfxBlend,
                                 {
                                     {GFXS_BLEND_DISABLED,     "disabled"    },
                                     {GFXS_BLEND_ZERO,         "zero"        },
                                     {GFXS_BLEND_ONE,          "one"         },
                                     {GFXS_BLEND_SRCCOLOR,     "srccolor"    },
                                     {GFXS_BLEND_INVSRCCOLOR,  "invsrccolor" },
                                     {GFXS_BLEND_SRCALPHA,     "srcalpha"    },
                                     {GFXS_BLEND_INVSRCALPHA,  "invsrcalpha" },
                                     {GFXS_BLEND_DESTALPHA,    "destalpha"   },
                                     {GFXS_BLEND_INVDESTALPHA, "invdestalpha"},
                                     {GFXS_BLEND_DESTCOLOR,    "destcolor"   },
                                     {GFXS_BLEND_INVDESTCOLOR, "invdestcolor"},
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(GfxBlendOp,
                                 {
                                     {GFXS_BLENDOP_DISABLED,    "disabled"   },
                                     {GFXS_BLENDOP_ADD,         "add"        },
                                     {GFXS_BLENDOP_SUBTRACT,    "subtract"   },
                                     {GFXS_BLENDOP_REVSUBTRACT, "revsubtract"},
                                     {GFXS_BLENDOP_MIN,         "min"        },
                                     {GFXS_BLENDOP_MAX,         "max"        },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(GfxPolygonOffset_e,
                                 {
                                     {GFXS_POLYGON_OFFSET_0,         "offset0"        },
                                     {GFXS_POLYGON_OFFSET_1,         "offset1"        },
                                     {GFXS_POLYGON_OFFSET_2,         "offset2"        },
                                     {GFXS_POLYGON_OFFSET_SHADOWMAP, "offsetShadowmap"},
    });

    class JsonStateBitsTableEntry
    {
    public:
        GfxBlend srcBlendRgb;
        GfxBlend dstBlendRgb;
        GfxBlendOp blendOpRgb;
        JsonAlphaTest alphaTest;
        JsonCullFace cullFace;
        GfxBlend srcBlendAlpha;
        GfxBlend dstBlendAlpha;
        GfxBlendOp blendOpAlpha;
        bool colorWriteRgb;
        bool colorWriteAlpha;
        bool polymodeLine;
        bool depthWrite;
        JsonDepthTest depthTest;
        GfxPolygonOffset_e polygonOffset;
        std::optional<JsonStencil> stencilFront;
        std::optional<JsonStencil> stencilBack;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonStateBitsTableEntry,
                                       srcBlendRgb,
                                       dstBlendRgb,
                                       blendOpRgb,
                                       alphaTest,
                                       cullFace,
                                       srcBlendAlpha,
                                       dstBlendAlpha,
                                       blendOpAlpha,
                                       colorWriteRgb,
                                       colorWriteAlpha,
                                       polymodeLine,
                                       depthWrite,
                                       depthWrite,
                                       depthTest,
                                       polygonOffset,
                                       stencilFront,
                                       stencilBack);

    class JsonConstant
    {
    public:
        std::optional<std::string> name;
        std::optional<std::string> nameFragment;
        std::optional<unsigned> nameHash;
        std::vector<float> literal;
    };

    inline void to_json(nlohmann::json& out, const JsonConstant& in)
    {
        if (in.name.has_value())
        {
            out["name"] = in.name;
        }
        else
        {
            out["nameFragment"] = in.nameFragment;
            out["nameHash"] = in.nameHash;
        }

        out["literal"] = in.literal;
    }

    inline void from_json(const nlohmann::json& in, JsonConstant& out)
    {
        in.value("name", nlohmann::json()).get_to(out.name);
        in.value("nameFragment", nlohmann::json()).get_to(out.nameFragment);
        in.value("nameHash", nlohmann::json()).get_to(out.nameHash);
        in.at("literal").get_to(out.literal);
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(TextureFilter,
                                 {
                                     {TEXTURE_FILTER_DISABLED, "disabled"},
                                     {TEXTURE_FILTER_NEAREST,  "nearest" },
                                     {TEXTURE_FILTER_LINEAR,   "linear"  },
                                     {TEXTURE_FILTER_ANISO2X,  "aniso2x" },
                                     {TEXTURE_FILTER_ANISO4X,  "aniso4x" },
                                     {TEXTURE_FILTER_COMPARE,  "compare" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(SamplerStateBitsMipMap_e,
                                 {
                                     {SAMPLER_MIPMAP_ENUM_DISABLED, "disabled"},
                                     {SAMPLER_MIPMAP_ENUM_NEAREST,  "nearest" },
                                     {SAMPLER_MIPMAP_ENUM_LINEAR,   "linear"  },
    });

    class JsonSamplerState
    {
    public:
        TextureFilter filter;
        SamplerStateBitsMipMap_e mipMap;
        bool clampU;
        bool clampV;
        bool clampW;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonSamplerState, filter, mipMap, clampU, clampV, clampW);

    NLOHMANN_JSON_SERIALIZE_ENUM(TextureSemantic,
                                 {
                                     {TS_2D,            "2D"          },
                                     {TS_FUNCTION,      "function"    },
                                     {TS_COLOR_MAP,     "colorMap"    },
                                     {TS_UNUSED_1,      "unused1"     },
                                     {TS_UNUSED_2,      "unused2"     },
                                     {TS_NORMAL_MAP,    "normalMap"   },
                                     {TS_UNUSED_3,      "unused3"     },
                                     {TS_UNUSED_4,      "unused4"     },
                                     {TS_SPECULAR_MAP,  "specularMap" },
                                     {TS_UNUSED_5,      "unused5"     },
                                     {TS_OCCLUSION_MAP, "occlusionMap"},
                                     {TS_UNUSED_6,      "unused6"     },
                                     {TS_COLOR0_MAP,    "color0Map"   },
                                     {TS_COLOR1_MAP,    "color1Map"   },
                                     {TS_COLOR2_MAP,    "color2Map"   },
                                     {TS_COLOR3_MAP,    "color3Map"   },
                                     {TS_COLOR4_MAP,    "color4Map"   },
                                     {TS_COLOR5_MAP,    "color5Map"   },
                                     {TS_COLOR6_MAP,    "color6Map"   },
                                     {TS_COLOR7_MAP,    "color7Map"   },
                                     {TS_COLOR8_MAP,    "color8Map"   },
                                     {TS_COLOR9_MAP,    "color9Map"   },
                                     {TS_COLOR10_MAP,   "color10Map"  },
                                     {TS_COLOR11_MAP,   "color11Map"  },
                                     {TS_COLOR12_MAP,   "color12Map"  },
                                     {TS_COLOR13_MAP,   "color13Map"  },
                                     {TS_COLOR14_MAP,   "color14Map"  },
                                     {TS_COLOR15_MAP,   "color15Map"  },
                                     {TS_THROW_MAP,     "throwMap"    },
    });

    class JsonTexture
    {
    public:
        std::optional<std::string> name;
        std::optional<unsigned> nameHash;
        std::optional<std::string> nameStart;
        std::optional<std::string> nameEnd;
        TextureSemantic semantic;
        bool isMatureContent;
        JsonSamplerState samplerState;
        std::string image;
    };

    inline void to_json(nlohmann::json& out, const JsonTexture& in)
    {
        if (in.name.has_value())
        {
            out["name"] = in.name;
        }
        else
        {
            out["nameHash"] = in.nameHash;
            out["nameStart"] = in.nameStart;
            out["nameEnd"] = in.nameEnd;
        }

        out["semantic"] = in.semantic;
        out["isMatureContent"] = in.isMatureContent;
        out["samplerState"] = in.samplerState;
        out["image"] = in.image;
    }

    inline void from_json(const nlohmann::json& in, JsonTexture& out)
    {
        in.value("name", nlohmann::json()).get_to(out.name);
        in.value("nameHash", nlohmann::json()).get_to(out.nameHash);
        in.value("nameStart", nlohmann::json()).get_to(out.nameStart);
        in.value("nameEnd", nlohmann::json()).get_to(out.nameEnd);
        in.at("semantic").get_to(out.semantic);
        in.at("isMatureContent").get_to(out.isMatureContent);
        in.at("samplerState").get_to(out.samplerState);
        in.at("image").get_to(out.image);
    };

    class JsonTextureAtlas
    {
    public:
        uint8_t rows;
        uint8_t columns;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonTextureAtlas, rows, columns);

    NLOHMANN_JSON_SERIALIZE_ENUM(MaterialGameFlags,
                                 {
                                     {MTL_GAMEFLAG_1,            "1"           },
                                     {MTL_GAMEFLAG_2,            "2"           },
                                     {MTL_GAMEFLAG_NO_MARKS,     "NO_MARKS"    },
                                     {MTL_GAMEFLAG_NO_MARKS,     "4"           },
                                     {MTL_GAMEFLAG_8,            "8"           },
                                     {MTL_GAMEFLAG_10,           "10"          },
                                     {MTL_GAMEFLAG_20,           "20"          },
                                     {MTL_GAMEFLAG_CASTS_SHADOW, "CASTS_SHADOW"},
                                     {MTL_GAMEFLAG_CASTS_SHADOW, "40"          },
                                     {MTL_GAMEFLAG_80,           "80"          },
                                     {MTL_GAMEFLAG_100,          "100"         },
                                     {MTL_GAMEFLAG_200,          "200"         },
                                     {MTL_GAMEFLAG_400,          "400"         },
                                     {MTL_GAMEFLAG_800,          "800"         },
                                     {MTL_GAMEFLAG_1000,         "1000"        },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(GfxCameraRegionType,
                                 {
                                     {CAMERA_REGION_LIT_OPAQUE,       "litOpaque"     },
                                     {CAMERA_REGION_LIT_TRANS,        "litTrans"      },
                                     {CAMERA_REGION_LIT_QUASI_OPAQUE, "litQuasiOpaque"},
                                     {CAMERA_REGION_EMISSIVE_OPAQUE,  "emissiveOpaque"},
                                     {CAMERA_REGION_EMISSIVE_TRANS,   "emissiveTrans" },
                                     {CAMERA_REGION_EMISSIVE_FX,      "emissiveFx"    },
                                     {CAMERA_REGION_LIGHT_MAP_OPAQUE, "lightMapOpaque"},
                                     {CAMERA_REGION_DEPTH_HACK,       "depthHack"     },
                                     {CAMERA_REGION_UNUSED,           "unused"        },
                                     {CAMERA_REGION_SONAR,            "sonar"         },
                                     {CAMERA_REGION_NONE,             "none"          },
    });

    class JsonMaterial
    {
    public:
        std::vector<MaterialGameFlags> gameFlags;
        unsigned sortKey;
        std::optional<JsonTextureAtlas> textureAtlas;
        unsigned surfaceTypeBits;
        unsigned layeredSurfaceTypes;
        unsigned hashIndex;
        unsigned surfaceFlags;
        unsigned contents;
        std::vector<int8_t> stateBitsEntry;
        unsigned stateFlags;
        GfxCameraRegionType cameraRegion;
        uint8_t probeMipBits;
        std::string techniqueSet;
        std::vector<JsonTexture> textures;
        std::vector<JsonConstant> constants;
        std::vector<JsonStateBitsTableEntry> stateBits;
        std::optional<std::string> thermalMaterial;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonMaterial,
                                       gameFlags,
                                       sortKey,
                                       textureAtlas,
                                       surfaceTypeBits,
                                       layeredSurfaceTypes,
                                       hashIndex,
                                       surfaceFlags,
                                       contents,
                                       stateBitsEntry,
                                       stateFlags,
                                       cameraRegion,
                                       probeMipBits,
                                       techniqueSet,
                                       textures,
                                       constants,
                                       stateBits,
                                       thermalMaterial);
} // namespace T6
