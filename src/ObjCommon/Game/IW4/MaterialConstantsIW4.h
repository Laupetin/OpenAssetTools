#pragma once

#include "Game/IW4/IW4.h"

namespace IW4
{
    inline infoParm_t infoParms[]
    {
        {"bark", 0x0, 0x100000, 0x0, 0x0},
        {"brick", 0x0, 0x200000, 0x0, 0x0},
        {"carpet", 0x0, 0x300000, 0x0, 0x0},
        {"cloth", 0x0, 0x400000, 0x0, 0x0},
        {"concrete", 0x0, 0x500000, 0x0, 0x0},
        {"dirt", 0x0, 0x600000, 0x0, 0x0},
        {"flesh", 0x0, 0x700000, 0x0, 0x0},
        {"foliage", 0x1, 0x800000, 0x2, 0x0},
        {"glass", 0x1, 0x900000, 0x10, 0x0},
        {"grass", 0x0, 0xa00000, 0x0, 0x0},
        {"gravel", 0x0, 0xb00000, 0x0, 0x0},
        {"ice", 0x0, 0xc00000, 0x0, 0x0},
        {"metal", 0x0, 0xd00000, 0x0, 0x0},
        {"mud", 0x0, 0xe00000, 0x0, 0x0},
        {"paper", 0x0, 0xf00000, 0x0, 0x0},
        {"plaster", 0x0, 0x1000000, 0x0, 0x0},
        {"rock", 0x0, 0x1100000, 0x0, 0x0},
        {"sand", 0x0, 0x1200000, 0x0, 0x0},
        {"snow", 0x0, 0x1300000, 0x0, 0x0},
        {"water", 0x1, 0x1400000, 0x20, 0x0},
        {"wood", 0x0, 0x1500000, 0x0, 0x0},
        {"asphalt", 0x0, 0x1600000, 0x0, 0x0},
        {"ceramic", 0x0, 0x1700000, 0x0, 0x0},
        {"plastic", 0x0, 0x1800000, 0x0, 0x0},
        {"rubber", 0x0, 0x1900000, 0x0, 0x0},
        {"cushion", 0x0, 0x1a00000, 0x0, 0x0},
        {"fruit", 0x0, 0x1b00000, 0x0, 0x0},
        {"paintedmetal", 0x0, 0x1c00000, 0x0, 0x0},
        {"riotshield", 0x0, 0x1d00000, 0x0, 0x0},
        {"slush", 0x0, 0x1e00000, 0x0, 0x0},
        {"opaqueglass", 0x0, 0x900000, 0x0, 0x0},
        {"clipmissile", 0x1, 0x0, 0x80, 0x0},
        {"ai_nosight", 0x1, 0x0, 0x1000, 0x0},
        {"clipshot", 0x1, 0x0, 0x2000, 0x0},
        {"playerclip", 0x1, 0x0, 0x10000, 0x0},
        {"monsterclip", 0x1, 0x0, 0x20000, 0x0},
        {"aiclipallowdeath", 0x1, 0x0, 0x4, 0x0},
        {"vehicleclip", 0x1, 0x0, 0x200, 0x0},
        {"itemclip", 0x1, 0x0, 0x400, 0x0},
        {"nodrop", 0x1, 0x0, 0x80000000, 0x0},
        {"nonsolid", 0x1, 0x4000, 0x0, 0x0},
        {"detail", 0x0, 0x0, 0x8000000, 0x0},
        {"structural", 0x0, 0x0, 0x10000000, 0x0},
        {"portal", 0x1, 0x80000000, 0x0, 0x0},
        {"canshootclip", 0x0, 0x0, 0x40, 0x0},
        {"origin", 0x1, 0x0, 0x0, 0x4},
        {"sky", 0x0, 0x4, 0x800, 0x0},
        {"nocastshadow", 0x0, 0x40000, 0x0, 0x0},
        {"physicsGeom", 0x0, 0x0, 0x0, 0x400},
        {"lightPortal", 0x0, 0x0, 0x0, 0x2000},
        {"outdoorBounds", 0x0, 0x0, 0x0, 0x8000},
        {"slick", 0x0, 0x2, 0x0, 0x0},
        {"noimpact", 0x0, 0x10, 0x0, 0x0},
        {"nomarks", 0x0, 0x20, 0x0, 0x0},
        {"nopenetrate", 0x0, 0x100, 0x0, 0x0},
        {"ladder", 0x0, 0x8, 0x0, 0x0},
        {"nodamage", 0x0, 0x1, 0x0, 0x0},
        {"mantleOn", 0x0, 0x2000000, 0x1000000, 0x0},
        {"mantleOver", 0x0, 0x4000000, 0x1000000, 0x0},
        {"stairs", 0x0, 0x200, 0x0, 0x0},
        {"soft", 0x0, 0x1000, 0x0, 0x0},
        {"nosteps", 0x0, 0x2000, 0x0, 0x0},
        {"nodraw", 0x0, 0x80, 0x0, 0x0},
        {"nolightmap", 0x0, 0x400, 0x0, 0x0},
        {"nodlight", 0x0, 0x20000, 0x0, 0x0}
    };

    inline const char* surfaceTypeNames[]
    {
        "default",
        "bark",
        "brick",
        "carpet",
        "cloth",
        "concrete",
        "dirt",
        "flesh",
        "foliage",
        "glass",
        "grass",
        "gravel",
        "ice",
        "metal",
        "mud",
        "paper",
        "plaster",
        "rock",
        "sand",
        "snow",
        "water",
        "wood",
        "asphalt",
        "ceramic",
        "plastic",
        "rubber",
        "cushion",
        "fruit",
        "paintedmetal",
        "riotshield",
        "slush"
    };
    static_assert(std::extent_v<decltype(surfaceTypeNames)> == SURF_TYPE_NUM);


    enum class BlendFunc_e
    {
        UNKNOWN,
        CUSTOM,
        REPLACE,
        BLEND,
        ADD,
        MULTIPLY,
        SCREEN_ADD,

        COUNT
    };

    constexpr auto GDT_BLEND_FUNC_UNKNOWN = "";
    constexpr auto GDT_BLEND_FUNC_CUSTOM = "Custom";
    constexpr auto GDT_BLEND_FUNC_REPLACE = "Replace*";
    constexpr auto GDT_BLEND_FUNC_BLEND = "Blend";
    constexpr auto GDT_BLEND_FUNC_ADD = "Add";
    constexpr auto GDT_BLEND_FUNC_MULTIPLY = "Multiply";
    constexpr auto GDT_BLEND_FUNC_SCREEN_ADD = "Screen Add";

    inline const char* GdtBlendFuncNames[]
    {
        GDT_BLEND_FUNC_UNKNOWN,
        GDT_BLEND_FUNC_CUSTOM,
        GDT_BLEND_FUNC_REPLACE,
        GDT_BLEND_FUNC_BLEND,
        GDT_BLEND_FUNC_ADD,
        GDT_BLEND_FUNC_MULTIPLY,
        GDT_BLEND_FUNC_SCREEN_ADD
    };
    static_assert(std::extent_v<decltype(GdtBlendFuncNames)> == static_cast<size_t>(BlendFunc_e::COUNT));

    enum class BlendOp_e
    {
        UNKNOWN,
        DISABLE,
        ADD,
        SUBTRACT,
        REV_SUBTRACT,
        MIN,
        MAX,

        COUNT
    };

    constexpr auto GDT_BLEND_OP_UNKNOWN = "";
    constexpr auto GDT_BLEND_OP_DISABLE = "Disable";
    constexpr auto GDT_BLEND_OP_ADD = "Add*";
    constexpr auto GDT_BLEND_OP_SUBTRACT = "Subtract";
    constexpr auto GDT_BLEND_OP_REV_SUBTRACT = "RevSubtract";
    constexpr auto GDT_BLEND_OP_MIN = "Min";
    constexpr auto GDT_BLEND_OP_MAX = "Max";

    inline const char* GdtBlendOpNames[]
    {
        GDT_BLEND_OP_UNKNOWN,
        GDT_BLEND_OP_DISABLE,
        GDT_BLEND_OP_ADD,
        GDT_BLEND_OP_SUBTRACT,
        GDT_BLEND_OP_REV_SUBTRACT,
        GDT_BLEND_OP_MIN,
        GDT_BLEND_OP_MAX
    };
    static_assert(std::extent_v<decltype(GdtBlendOpNames)> == static_cast<size_t>(BlendOp_e::COUNT));

    enum class CustomBlendFunc_e
    {
        UNKNOWN,
        DISABLED,
        ZERO,
        ONE,
        SRC_COLOR,
        INV_SRC_COLOR,
        SRC_ALPHA,
        INV_SRC_ALPHA,
        DST_ALPHA,
        INV_DST_ALPHA,
        DEST_COLOR,
        INV_DST_COLOR,

        COUNT
    };

    inline const char* GdtCustomBlendFuncNames[]
    {
        "",
        "Disable",
        "Zero",
        "One*",
        "SrcColor",
        "InvSrcColor",
        "SrcAlpha",
        "InvSrcAlpha",
        "DestAlpha",
        "InvDestAlpha",
        "DestColor",
        "InvDestColor"
    };
    static_assert(std::extent_v<decltype(GdtCustomBlendFuncNames)> == static_cast<size_t>(CustomBlendFunc_e::COUNT));

    enum class AlphaTest_e
    {
        UNKNOWN,
        ALWAYS,
        GT0,
        LT128,
        GE128,

        COUNT
    };

    constexpr auto GDT_ALPHA_TEST_UNKNOWN = "";
    constexpr auto GDT_ALPHA_TEST_ALWAYS = "Always*";
    constexpr auto GDT_ALPHA_TEST_GT0 = "GT0";
    constexpr auto GDT_ALPHA_TEST_LT128 = "LT128";
    constexpr auto GDT_ALPHA_TEST_GE128 = "GE128";

    inline const char* GdtAlphaTestNames[]
    {
        GDT_ALPHA_TEST_UNKNOWN,
        GDT_ALPHA_TEST_ALWAYS,
        GDT_ALPHA_TEST_GT0,
        GDT_ALPHA_TEST_LT128,
        GDT_ALPHA_TEST_GE128
    };
    static_assert(std::extent_v<decltype(GdtAlphaTestNames)> == static_cast<size_t>(AlphaTest_e::COUNT));

    enum class DepthTest_e
    {
        UNKNOWN,
        LESS_EQUAL,
        LESS,
        EQUAL,
        ALWAYS,
        DISABLE,

        COUNT
    };

    inline const char* GdtDepthTestNames[]
    {
        "",
        "LessEqual*",
        "Less",
        "Equal",
        "Always",
        "Disable"
    };
    static_assert(std::extent_v<decltype(GdtDepthTestNames)> == static_cast<size_t>(DepthTest_e::COUNT));

    enum class StateBitsEnabledStatus_e
    {
        UNKNOWN,
        ENABLED,
        DISABLED,

        COUNT
    };

    inline const char* GdtStateBitsEnabledStatusNames[]
    {
        "",
        "Enable",
        "Disable"
    };
    static_assert(std::extent_v<decltype(GdtStateBitsEnabledStatusNames)> == static_cast<size_t>(StateBitsEnabledStatus_e::COUNT));

    inline const char* GdtStateBitsOnOffStatusNames[]
    {
        "",
        "On",
        "Off"
    };
    static_assert(std::extent_v<decltype(GdtStateBitsOnOffStatusNames)> == static_cast<size_t>(StateBitsEnabledStatus_e::COUNT));

    enum class CullFace_e
    {
        UNKNOWN,
        NONE,
        BACK,
        FRONT,

        COUNT
    };

    inline const char* GdtCullFaceNames[]
    {
        "",
        "None",
        "Back*",
        "Front"
    };
    static_assert(std::extent_v<decltype(GdtCullFaceNames)> == static_cast<size_t>(CullFace_e::COUNT));

    enum class PolygonOffset_e
    {
        UNKNOWN,
        OFFSET_0,
        OFFSET_1,
        OFFSET_2,
        OFFSET_SHADOW_MAP,

        COUNT
    };

    inline const char* GdtPolygonOffsetNames[]
    {
        "",
        "0",
        "1",
        "2",
        "shadowMap"
    };
    static_assert(std::extent_v<decltype(GdtPolygonOffsetNames)> == static_cast<size_t>(PolygonOffset_e::COUNT));

    enum class StencilFunc_e
    {
        UNKNOWN,
        NEVER,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_EQUAL,
        ALWAYS,

        COUNT
    };

    inline const char* GdtStencilFuncNames[]
    {
        "",
        "Never",
        "Less",
        "Equal",
        "LessEqual",
        "Greater",
        "NotEqual",
        "GreaterEqual",
        "Always"
    };
    static_assert(std::extent_v<decltype(GdtStencilFuncNames)> == static_cast<size_t>(StencilFunc_e::COUNT));

    enum class StencilOp_e
    {
        UNKNOWN,
        KEEP,
        ZERO,
        REPLACE,
        INCR_SAT,
        DECR_SAT,
        INVERT,
        INCR,
        DECR,

        COUNT
    };

    inline const char* GdtStencilOpNames[]
    {
        "",
        "Keep",
        "Zero",
        "Replace",
        "IncrSat",
        "DecrSat",
        "Invert",
        "Incr",
        "Decr"
    };
    static_assert(std::extent_v<decltype(GdtStencilOpNames)> == static_cast<size_t>(StencilOp_e::COUNT));

    enum GdtMaterialType
    {
        MATERIAL_TYPE_UNKNOWN,
        MATERIAL_TYPE_2D,
        MATERIAL_TYPE_CUSTOM,
        MATERIAL_TYPE_DISTORTION,
        MATERIAL_TYPE_EFFECT,
        MATERIAL_TYPE_IMPACT_MARK,
        MATERIAL_TYPE_MODEL_AMBIENT,
        MATERIAL_TYPE_MODEL_PHONG,
        MATERIAL_TYPE_MODEL_UNLIT,
        MATERIAL_TYPE_OBJECTIVE,
        MATERIAL_TYPE_PARTICLE_CLOUD,
        MATERIAL_TYPE_SKY,
        MATERIAL_TYPE_TOOLS,
        MATERIAL_TYPE_UNLIT,
        MATERIAL_TYPE_WATER,
        MATERIAL_TYPE_WORLD_PHONG,
        MATERIAL_TYPE_WORLD_UNLIT,

        MATERIAL_TYPE_COUNT
    };

    constexpr auto GDT_MATERIAL_TYPE_UNKNOWN = "<unknown>";
    constexpr auto GDT_MATERIAL_TYPE_2D = "2d";
    constexpr auto GDT_MATERIAL_TYPE_CUSTOM = "custom";
    constexpr auto GDT_MATERIAL_TYPE_DISTORTION = "distortion";
    constexpr auto GDT_MATERIAL_TYPE_EFFECT = "effect";
    constexpr auto GDT_MATERIAL_TYPE_IMPACT_MARK = "impact mark";
    constexpr auto GDT_MATERIAL_TYPE_MODEL_AMBIENT = "model ambient";
    constexpr auto GDT_MATERIAL_TYPE_MODEL_PHONG = "model phong";
    constexpr auto GDT_MATERIAL_TYPE_MODEL_UNLIT = "model unlit";
    constexpr auto GDT_MATERIAL_TYPE_OBJECTIVE = "objective";
    constexpr auto GDT_MATERIAL_TYPE_PARTICLE_CLOUD = "particle cloud";
    constexpr auto GDT_MATERIAL_TYPE_SKY = "sky";
    constexpr auto GDT_MATERIAL_TYPE_TOOLS = "tools";
    constexpr auto GDT_MATERIAL_TYPE_UNLIT = "unlit";
    constexpr auto GDT_MATERIAL_TYPE_WATER = "water";
    constexpr auto GDT_MATERIAL_TYPE_WORLD_PHONG = "world phong";
    constexpr auto GDT_MATERIAL_TYPE_WORLD_UNLIT = "world unlit";

    inline const char* GdtMaterialTypeNames[]
    {
        GDT_MATERIAL_TYPE_UNKNOWN,
        GDT_MATERIAL_TYPE_2D,
        GDT_MATERIAL_TYPE_CUSTOM,
        GDT_MATERIAL_TYPE_DISTORTION,
        GDT_MATERIAL_TYPE_EFFECT,
        GDT_MATERIAL_TYPE_IMPACT_MARK,
        GDT_MATERIAL_TYPE_MODEL_AMBIENT,
        GDT_MATERIAL_TYPE_MODEL_PHONG,
        GDT_MATERIAL_TYPE_MODEL_UNLIT,
        GDT_MATERIAL_TYPE_OBJECTIVE,
        GDT_MATERIAL_TYPE_PARTICLE_CLOUD,
        GDT_MATERIAL_TYPE_SKY,
        GDT_MATERIAL_TYPE_TOOLS,
        GDT_MATERIAL_TYPE_UNLIT,
        GDT_MATERIAL_TYPE_WATER,
        GDT_MATERIAL_TYPE_WORLD_PHONG,
        GDT_MATERIAL_TYPE_WORLD_UNLIT
    };
    static_assert(std::extent_v<decltype(GdtMaterialTypeNames)> == static_cast<size_t>(MATERIAL_TYPE_COUNT));

    constexpr auto GDT_SORTKEY_DEFAULT = "<default>";
    constexpr auto GDT_SORTKEY_OPAQUE_AMBIENT = "opaque ambient";
    constexpr auto GDT_SORTKEY_OPAQUE = "opaque";
    constexpr auto GDT_SORTKEY_SKY = "sky";
    constexpr auto GDT_SORTKEY_SKYBOX = "skybox";
    constexpr auto GDT_SORTKEY_DECAL_BOTTOM_1 = "decal - bottom 1";
    constexpr auto GDT_SORTKEY_DECAL_BOTTOM_2 = "decal - bottom 2";
    constexpr auto GDT_SORTKEY_DECAL_BOTTOM_3 = "decal - bottom 3";
    constexpr auto GDT_SORTKEY_DECAL_STATIC = "decal - static decal";
    constexpr auto GDT_SORTKEY_DECAL_MIDDLE_1 = "decal - middle 1";
    constexpr auto GDT_SORTKEY_DECAL_MIDDLE_2 = "decal - middle 2";
    constexpr auto GDT_SORTKEY_DECAL_MIDDLE_3 = "decal - middle 3";
    constexpr auto GDT_SORTKEY_DECAL_WEAPON_IMPACT = "decal - weapon impact";
    constexpr auto GDT_SORTKEY_WINDOW_INSIDE = "window inside";
    constexpr auto GDT_SORTKEY_WINDOW_OUTSIDE = "window outside";
    constexpr auto GDT_SORTKEY_DISTORTION = "distortion";
    constexpr auto GDT_SORTKEY_BLEND_ADDITIVE = "blend / additive";
    constexpr auto GDT_SORTKEY_EFFECT_AUTO_SORT = "effect - auto sort";
    constexpr auto GDT_SORTKEY_AFTER_EFFECT_BOTTOM = "after effects - bottom";
    constexpr auto GDT_SORTKEY_AFTER_EFFECT_MIDDLE = "after effects - middle";
    constexpr auto GDT_SORTKEY_AFTER_EFFECT_TOP = "after effects - top";
    constexpr auto GDT_SORTKEY_VIEWMODEL_EFFECT = "viewmodel effect";

    inline const char* SortKeyNames[]
    {
        GDT_SORTKEY_OPAQUE_AMBIENT, // 0
        GDT_SORTKEY_OPAQUE, // 1
        GDT_SORTKEY_SKY, // 2
        GDT_SORTKEY_SKYBOX, // 3
        "?", // 4
        "?", // 5
        GDT_SORTKEY_DECAL_BOTTOM_1, // 6
        GDT_SORTKEY_DECAL_BOTTOM_2, // 7
        GDT_SORTKEY_DECAL_BOTTOM_3, // 8
        GDT_SORTKEY_DECAL_STATIC, // 9
        GDT_SORTKEY_DECAL_MIDDLE_1, // 10
        GDT_SORTKEY_DECAL_MIDDLE_2, // 11
        GDT_SORTKEY_DECAL_MIDDLE_3, // 12
        GDT_SORTKEY_DECAL_WEAPON_IMPACT, // 13
        "?", // 14
        "?", // 15
        "?", // 16
        "?", // 17
        "?", // 18
        "?", // 19
        "?", // 20
        "?", // 21
        "-", // 22
        "-", // 23
        GDT_SORTKEY_WINDOW_INSIDE, // 24
        GDT_SORTKEY_WINDOW_OUTSIDE, // 25
        "?", // 26
        "?", // 27
        "?", // 28
        "?", // 29
        "?", // 30
        "?", // 31
        "?", // 32
        "?", // 33
        "?", // 34
        "?", // 35
        "?", // 36
        "?", // 37
        "?", // 38
        "?", // 39
        "?", // 40
        "?", // 41
        "?", // 42
        GDT_SORTKEY_DISTORTION, // 43
        "?", // 44
        "?", // 45
        "-", // 46
        GDT_SORTKEY_BLEND_ADDITIVE, // 47
        GDT_SORTKEY_EFFECT_AUTO_SORT, // 48
        GDT_SORTKEY_AFTER_EFFECT_BOTTOM, // 49
        GDT_SORTKEY_AFTER_EFFECT_MIDDLE, // 50
        GDT_SORTKEY_AFTER_EFFECT_TOP, // 51
        "-", // 52
        GDT_SORTKEY_VIEWMODEL_EFFECT, // 53
    };
    static_assert(std::extent_v<decltype(SortKeyNames)> == static_cast<size_t>(SORTKEY_MAX));

    enum GdtCustomMaterialTypes
    {
        CUSTOM_MATERIAL_TYPE_NONE,
        // Uses custom techset with generic options
        CUSTOM_MATERIAL_TYPE_CUSTOM,
        CUSTOM_MATERIAL_TYPE_PHONG_FLAG,
        CUSTOM_MATERIAL_TYPE_GRAIN_OVERLAY,
        CUSTOM_MATERIAL_TYPE_EFFECT_EYE_OFFSET,
        CUSTOM_MATERIAL_TYPE_REFLEX_SIGHT,
        CUSTOM_MATERIAL_TYPE_SHADOW_CLEAR,
        CUSTOM_MATERIAL_TYPE_SHADOW_OVERLAY,

        // Not part of IW3
        CUSTOM_MATERIAL_TYPE_SPLATTER,

        CUSTOM_MATERIAL_TYPE_COUNT
    };

    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_NONE = "";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_CUSTOM = "mtl_custom";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_PHONG_FLAG = "mtl_phong_flag";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_GRAIN_OVERLAY = "mtl_grain_overlay";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_EFFECT_EYE_OFFSET = "mtl_effect_eyeoffset";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_REFLEXSIGHT = "mtl_reflexsight";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_SHADOWCLEAR = "mtl_shadowclear";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_SHADOWOVERLAY = "mtl_shadowoverlay";
    constexpr auto GDT_CUSTOM_MATERIAL_TYPE_SPLATTER = "mtl_splatter";

    inline const char* GdtCustomMaterialTypeNames[]
    {
        GDT_CUSTOM_MATERIAL_TYPE_NONE,
        GDT_CUSTOM_MATERIAL_TYPE_CUSTOM,
        GDT_CUSTOM_MATERIAL_TYPE_PHONG_FLAG,
        GDT_CUSTOM_MATERIAL_TYPE_GRAIN_OVERLAY,
        GDT_CUSTOM_MATERIAL_TYPE_EFFECT_EYE_OFFSET,
        GDT_CUSTOM_MATERIAL_TYPE_REFLEXSIGHT,
        GDT_CUSTOM_MATERIAL_TYPE_SHADOWCLEAR,
        GDT_CUSTOM_MATERIAL_TYPE_SHADOWOVERLAY,
        GDT_CUSTOM_MATERIAL_TYPE_SPLATTER
    };
    static_assert(std::extent_v<decltype(GdtCustomMaterialTypeNames)> == static_cast<size_t>(CUSTOM_MATERIAL_TYPE_COUNT));
}
