#include "AssetDumperMaterial.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <nlohmann/json.hpp>

#include "Utils/ClassUtils.h"
#include "Game/IW4/MaterialConstantsIW4.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Game/IW4/TechsetConstantsIW4.h"
#include "Math/Vector.h"

#define DUMP_AS_JSON 1
//#define DUMP_AS_GDT 1
//#define FLAGS_DEBUG 1

using namespace IW4;
using json = nlohmann::json;
using namespace std::string_literals;

namespace IW4
{
    const char* AssetName(const char* name)
    {
        if (name && name[0] == ',')
            return &name[1];
        return name;
    }

    template <size_t S>
    json ArrayEntry(const char* (&a)[S], const size_t index)
    {
        assert(index < S);
        if (index < S)
            return a[index];

        return json{};
    }

    json BuildComplexTableJson(const complex_s* complexTable, const size_t count)
    {
        auto jArray = json::array();

        if (complexTable)
        {
            for (auto index = 0u; index < count; index++)
            {
                const auto& entry = complexTable[index];
                jArray.emplace_back(json{
                    {"real", entry.real},
                    {"imag", entry.imag}
                });
            }
        }

        return jArray;
    }

    json BuildWaterJson(water_t* water)
    {
        if (!water)
            return json{};

        return json{
            {"floatTime", water->writable.floatTime},
            {"H0", BuildComplexTableJson(water->H0, water->M * water->N)},
            {"wTerm", water->wTerm ? json{std::vector(water->wTerm, water->wTerm + (water->M * water->N))} : json::array()},
            {"M", water->M},
            {"N", water->N},
            {"Lx", water->Lx},
            {"Lz", water->Lz},
            {"windvel", water->windvel},
            {"winddir", std::vector(std::begin(water->winddir), std::end(water->winddir))},
            {"amplitude", water->amplitude},
            {"codeConstant", std::vector(std::begin(water->codeConstant), std::end(water->codeConstant))},
            {"image", water->image && water->image->name ? AssetName(water->image->name) : nullptr}
        };
    }

    json BuildSamplerStateJson(unsigned char samplerState)
    {
        static const char* samplerFilterNames[]
        {
            "none",
            "nearest",
            "linear",
            "aniso2x",
            "aniso4x"
        };
        static const char* samplerMipmapNames[]
        {
            "disabled",
            "nearest",
            "linear"
        };

        return json{
            {"filter", ArrayEntry(samplerFilterNames, (samplerState & SAMPLER_FILTER_MASK) >> SAMPLER_FILTER_SHIFT)},
            {"mipmap", ArrayEntry(samplerMipmapNames, (samplerState & SAMPLER_MIPMAP_MASK) >> SAMPLER_MIPMAP_SHIFT)},
            {"clampU", (samplerState & SAMPLER_CLAMP_U) ? true : false},
            {"clampV", (samplerState & SAMPLER_CLAMP_V) ? true : false},
            {"clampW", (samplerState & SAMPLER_CLAMP_W) ? true : false},
        };
    }

    json BuildTextureTableJson(MaterialTextureDef* textureTable, const size_t count)
    {
        static const char* semanticNames[]
        {
            "2d",
            "function",
            "colorMap",
            "detailMap",
            "unused2",
            "normalMap",
            "unused3",
            "unused4",
            "specularMap",
            "unused5",
            "unused6",
            "waterMap"
        };

        auto jArray = json::array();

        if (textureTable)
        {
            for (auto index = 0u; index < count; index++)
            {
                const auto& entry = textureTable[index];

                json jEntry = {
                    {"samplerState", BuildSamplerStateJson(entry.samplerState)},
                    {"semantic", ArrayEntry(semanticNames, entry.semantic)}
                };

                const auto knownMaterialSourceName = knownTextureMaps.find(entry.nameHash);
                if (knownMaterialSourceName != knownTextureMaps.end())
                {
                    jEntry["name"] = knownMaterialSourceName->second.m_name;
                }
                else
                {
                    jEntry.merge_patch({
                        {"nameHash", entry.nameHash},
                        {"nameStart", entry.nameStart},
                        {"nameEnd", entry.nameEnd},
                    });
                }

                if (entry.semantic == TS_WATER_MAP)
                {
                    jEntry["water"] = BuildWaterJson(entry.u.water);
                }
                else
                {
                    jEntry["image"] = entry.u.image && entry.u.image->name ? AssetName(entry.u.image->name) : nullptr;
                }

                jArray.emplace_back(std::move(jEntry));
            }
        }

        return jArray;
    }

    json BuildConstantTableJson(const MaterialConstantDef* constantTable, const size_t count)
    {
        auto jArray = json::array();

        if (constantTable)
        {
            for (auto index = 0u; index < count; index++)
            {
                const auto& entry = constantTable[index];
                json jEntry = {
                    {"literal", std::vector(std::begin(entry.literal), std::end(entry.literal))}
                };

                const auto nameLen = strnlen(entry.name, std::extent_v<decltype(MaterialConstantDef::name)>);
                if (nameLen == std::extent_v<decltype(MaterialConstantDef::name)>)
                {
                    std::string fullLengthName(entry.name, std::extent_v<decltype(MaterialConstantDef::name)>);
                    const auto fullLengthHash = Common::R_HashString(fullLengthName.c_str(), 0);

                    if (fullLengthHash == entry.nameHash)
                    {
                        jEntry["name"] = fullLengthName;
                    }
                    else
                    {
                        const auto knownMaterialSourceName = knownConstantNames.find(entry.nameHash);
                        if (knownMaterialSourceName != knownConstantNames.end())
                        {
                            jEntry["name"] = knownMaterialSourceName->second;
                        }
                        else
                        {
                            jEntry.merge_patch({
                                {"nameHash", entry.nameHash},
                                {"namePart", fullLengthName}
                            });
                        }
                    }
                }
                else
                {
                    jEntry["name"] = std::string(entry.name, nameLen);
                }

                jArray.emplace_back(std::move(jEntry));
            }
        }

        return jArray;
    }

    json BuildStateBitsTableJson(const GfxStateBits* stateBitsTable, const size_t count)
    {
        static const char* blendNames[]
        {
            "disabled",
            "zero",
            "one",
            "srcColor",
            "invSrcColor",
            "srcAlpha",
            "invSrcAlpha",
            "destAlpha",
            "invDestAlpha",
            "destColor",
            "invDestColor",
        };
        static const char* blendOpNames[]
        {
            "disabled",
            "add",
            "subtract",
            "revSubtract",
            "min",
            "max"
        };
        static const char* depthTestNames[]
        {
            "always",
            "less",
            "equal",
            "lessEqual",
        };
        static const char* polygonOffsetNames[]
        {
            "0",
            "1",
            "2",
            "shadowMap",
        };
        static const char* stencilOpNames[]
        {
            "keep",
            "zero",
            "replace",
            "incrSat",
            "decrSat",
            "invert",
            "incr",
            "decr"
        };

        auto jArray = json::array();

        if (stateBitsTable)
        {
            for (auto index = 0u; index < count; index++)
            {
                const auto& entry = stateBitsTable[index];

                const auto srcBlendRgb = (entry.loadBits[0] & GFXS0_SRCBLEND_RGB_MASK) >> GFXS0_SRCBLEND_RGB_SHIFT;
                const auto dstBlendRgb = (entry.loadBits[0] & GFXS0_DSTBLEND_RGB_MASK) >> GFXS0_DSTBLEND_RGB_SHIFT;
                const auto blendOpRgb = (entry.loadBits[0] & GFXS0_BLENDOP_RGB_MASK) >> GFXS0_BLENDOP_RGB_SHIFT;
                const auto srcBlendAlpha = (entry.loadBits[0] & GFXS0_SRCBLEND_ALPHA_MASK) >> GFXS0_SRCBLEND_ALPHA_SHIFT;
                const auto dstBlendAlpha = (entry.loadBits[0] & GFXS0_DSTBLEND_ALPHA_MASK) >> GFXS0_DSTBLEND_ALPHA_SHIFT;
                const auto blendOpAlpha = (entry.loadBits[0] & GFXS0_BLENDOP_ALPHA_MASK) >> GFXS0_BLENDOP_ALPHA_SHIFT;
                const auto depthTest = (entry.loadBits[1] & GFXS1_DEPTHTEST_MASK) >> GFXS1_DEPTHTEST_SHIFT;
                const auto polygonOffset = (entry.loadBits[1] & GFXS1_POLYGON_OFFSET_MASK) >> GFXS1_POLYGON_OFFSET_SHIFT;

                const auto* alphaTest = "disable";
                if ((entry.loadBits[0] & GFXS0_ATEST_MASK) == GFXS0_ATEST_GT_0)
                    alphaTest = "gt0";
                else if ((entry.loadBits[0] & GFXS0_ATEST_MASK) == GFXS0_ATEST_LT_128)
                    alphaTest = "lt128";
                else if ((entry.loadBits[0] & GFXS0_ATEST_MASK) == GFXS0_ATEST_GE_128)
                    alphaTest = "ge128";
                else
                    assert(entry.loadBits[0] & GFXS0_ATEST_DISABLE);

                const auto* cullFace = "none";
                if ((entry.loadBits[0] & GFXS0_CULL_MASK) == GFXS0_CULL_BACK)
                    cullFace = "back";
                else if ((entry.loadBits[0] & GFXS0_CULL_MASK) == GFXS0_CULL_FRONT)
                    cullFace = "front";
                else
                    assert((entry.loadBits[0] & GFXS0_CULL_MASK) == GFXS0_CULL_NONE);

                jArray.emplace_back(json{
                    {"srcBlendRgb", ArrayEntry(blendNames, srcBlendRgb)},
                    {"dstBlendRgb", ArrayEntry(blendNames, dstBlendRgb)},
                    {"blendOpRgb", ArrayEntry(blendOpNames, blendOpRgb)},
                    {"alphaTest", alphaTest},
                    {"cullFace", cullFace},
                    {"srcBlendAlpha", ArrayEntry(blendNames, srcBlendAlpha)},
                    {"dstBlendAlpha", ArrayEntry(blendNames, dstBlendAlpha)},
                    {"blendOpAlpha", ArrayEntry(blendOpNames, blendOpAlpha)},
                    {"colorWriteRgb", (entry.loadBits[0] & GFXS0_COLORWRITE_RGB) ? true : false},
                    {"colorWriteAlpha", (entry.loadBits[0] & GFXS0_COLORWRITE_ALPHA) ? true : false},
                    {"gammaWrite", (entry.loadBits[0] & GFXS0_GAMMAWRITE) ? true : false},
                    {"polymodeLine", (entry.loadBits[0] & GFXS0_POLYMODE_LINE) ? true : false},

                    {"depthWrite", (entry.loadBits[1] & GFXS1_DEPTHWRITE) ? true : false},
                    {"depthTest", (entry.loadBits[1] & GFXS1_DEPTHTEST_DISABLE) ? json("disable") : ArrayEntry(depthTestNames, depthTest)},
                    {"polygonOffset", ArrayEntry(polygonOffsetNames, polygonOffset)},
                    {"stencilFrontEnabled", (entry.loadBits[1] & GFXS1_STENCIL_FRONT_ENABLE) ? true : false},
                    {"stencilBackEnabled", (entry.loadBits[1] & GFXS1_STENCIL_BACK_ENABLE) ? true : false},
                    {"stencilFrontPass", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_FRONT_PASS_SHIFT) & GFXS_STENCILOP_MASK)},
                    {"stencilFrontFail", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_FRONT_FAIL_SHIFT) & GFXS_STENCILOP_MASK)},
                    {"stencilFrontZFail", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_FRONT_ZFAIL_SHIFT) & GFXS_STENCILOP_MASK)},
                    {"stencilFrontFunc", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_FRONT_FUNC_SHIFT) & GFXS_STENCILOP_MASK)},
                    {"stencilBackPass", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_BACK_PASS_SHIFT) & GFXS_STENCILOP_MASK)},
                    {"stencilBackFail", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_BACK_FAIL_SHIFT) & GFXS_STENCILOP_MASK)},
                    {"stencilBackZFail", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_BACK_ZFAIL_SHIFT) & GFXS_STENCILOP_MASK)},
                    {"stencilBackFunc", ArrayEntry(stencilOpNames, (entry.loadBits[1] >> GFXS1_STENCIL_BACK_FUNC_SHIFT) & GFXS_STENCILOP_MASK)},
                });
            }
        }

        return jArray;
    }

    json BuildCharFlagsJson(const std::string& prefix, const unsigned char gameFlags)
    {
        std::vector<std::string> values;

        for (auto i = 0u; i < (sizeof(gameFlags) * 8u); i++)
        {
            if (gameFlags & (1 << i))
            {
                std::ostringstream ss;
                ss << prefix << " 0x" << std::hex << (1 << i);
                values.emplace_back(ss.str());
            }
        }

        return json(values);
    }

    std::string CreateSurfaceTypeString(const unsigned surfaceTypeBits)
    {
        if (!surfaceTypeBits)
            return "<none>";

        static constexpr auto NON_SURFACE_TYPE_BITS = ~(std::numeric_limits<unsigned>::max() >> ((sizeof(unsigned) * 8) - (static_cast<unsigned>(SURF_TYPE_NUM) - 1)));
        assert((surfaceTypeBits & NON_SURFACE_TYPE_BITS) == 0);

        std::ostringstream ss;
        auto firstSurfaceType = true;
        for (auto surfaceTypeIndex = static_cast<unsigned>(SURF_TYPE_BARK); surfaceTypeIndex < SURF_TYPE_NUM; surfaceTypeIndex++)
        {
            if ((surfaceTypeBits & (1 << (surfaceTypeIndex - 1))) == 0)
                continue;

            if (firstSurfaceType)
                firstSurfaceType = false;
            else
                ss << ",";
            ss << surfaceTypeNames[surfaceTypeIndex];
        }

        if (firstSurfaceType)
            return "<none>";

        return ss.str();
    }

    void DumpMaterialAsJson(Material* material, std::ostream& stream)
    {
        static const char* cameraRegionNames[]
        {
            "litOpaque",
            "litTrans",
            "emissive",
            "depthHack",
            "none"
        };

        const json j = {
            {
                "info", {
#if defined(FLAGS_DEBUG) && FLAGS_DEBUG == 1
                    {"gameFlags", BuildCharFlagsJson("gameFlag", material->info.gameFlags)}, // TODO: Find out what gameflags mean
#else
                    {"gameFlags", material->info.gameFlags}, // TODO: Find out what gameflags mean
#endif
                    {"sortKey", material->info.sortKey},
                    {"textureAtlasRowCount", material->info.textureAtlasRowCount},
                    {"textureAtlasColumnCount", material->info.textureAtlasColumnCount},
                    {
                        "drawSurf", {
                            {"objectId", static_cast<unsigned>(material->info.drawSurf.fields.objectId)},
                            {"reflectionProbeIndex", static_cast<unsigned>(material->info.drawSurf.fields.reflectionProbeIndex)},
                            {"hasGfxEntIndex", static_cast<unsigned>(material->info.drawSurf.fields.hasGfxEntIndex)},
                            {"customIndex", static_cast<unsigned>(material->info.drawSurf.fields.customIndex)},
                            {"materialSortedIndex", static_cast<unsigned>(material->info.drawSurf.fields.materialSortedIndex)},
                            {"prepass", static_cast<unsigned>(material->info.drawSurf.fields.prepass)},
                            {"useHeroLighting", static_cast<unsigned>(material->info.drawSurf.fields.useHeroLighting)},
                            {"sceneLightIndex", static_cast<unsigned>(material->info.drawSurf.fields.sceneLightIndex)},
                            {"surfType", static_cast<unsigned>(material->info.drawSurf.fields.surfType)},
                            {"primarySortKey", static_cast<unsigned>(material->info.drawSurf.fields.primarySortKey)}
                        }
                    },
                    {"surfaceTypeBits", CreateSurfaceTypeString(material->info.surfaceTypeBits)},
                    {"hashIndex", material->info.hashIndex}
                }
            },
            {"stateBitsEntry", std::vector(std::begin(material->stateBitsEntry), std::end(material->stateBitsEntry))},
#if defined(FLAGS_DEBUG) && FLAGS_DEBUG == 1
            {"stateFlags", BuildCharFlagsJson("stateFlag", material->stateFlags)},
#else
            {"stateFlags", material->stateFlags},
#endif
            {"cameraRegion", ArrayEntry(cameraRegionNames, material->cameraRegion)},
            {"techniqueSet", material->techniqueSet && material->techniqueSet->name ? AssetName(material->techniqueSet->name) : json{}},
            {"textureTable", BuildTextureTableJson(material->textureTable, material->textureCount)},
            {"constantTable", BuildConstantTableJson(material->constantTable, material->constantCount)},
            {"stateBitsTable", BuildStateBitsTableJson(material->stateBitsTable, material->stateBitsCount)}
        };

        stream << std::setw(4) << j;
    }

    class TechsetInfo
    {
    public:
        std::string m_techset_name;
        std::string m_techset_base_name;
        std::string m_techset_prefix;
        GdtMaterialType m_gdt_material_type = MATERIAL_TYPE_UNKNOWN;
        GdtCustomMaterialTypes m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_NONE;
        std::string m_gdt_custom_string;
        MaterialType m_engine_material_type = MTL_TYPE_DEFAULT;
        std::string m_sort_key_name;
        bool m_no_cast_shadow = false;
        bool m_no_receive_dynamic_shadow = false;
        bool m_no_fog = false;
        bool m_tex_scroll = false;
        bool m_uv_anim = false;
        bool m_has_color_map = false;
        bool m_has_detail_map = false;
        bool m_has_normal_map = false;
        bool m_has_detail_normal_map = false;
        bool m_has_specular_map = false;
        bool m_zfeather = false;
        bool m_use_spot_light = false;
        bool m_falloff = false;
        bool m_dist_falloff = false;
        bool m_outdoor_only = false;

        // TODO: Find out what p0 in techset name actually means, seems like it only does stuff for techsets using a specular texture though
        // TODO: Find out what o0 in techset name actually means, seems like it gives the colormap a blue/whiteish tint and is almost exclusively used on snow-related materials
        // TODO: Find out what _lin in techset name actually means
        bool m_specular_p_flag = false;
        bool m_color_o_flag = false;
        bool m_effect_lin_flag = false;
    };

    class StateBitsInfo
    {
    public:
        BlendFunc_e m_blend_func = BlendFunc_e::UNKNOWN;
        BlendOp_e m_custom_blend_op_rgb = BlendOp_e::UNKNOWN;
        BlendOp_e m_custom_blend_op_alpha = BlendOp_e::UNKNOWN;
        CustomBlendFunc_e m_custom_src_blend_func = CustomBlendFunc_e::UNKNOWN;
        CustomBlendFunc_e m_custom_dst_blend_func = CustomBlendFunc_e::UNKNOWN;
        CustomBlendFunc_e m_custom_src_blend_func_alpha = CustomBlendFunc_e::UNKNOWN;
        CustomBlendFunc_e m_custom_dst_blend_func_alpha = CustomBlendFunc_e::UNKNOWN;
        AlphaTest_e m_alpha_test = AlphaTest_e::UNKNOWN;
        DepthTest_e m_depth_test = DepthTest_e::UNKNOWN;
        StateBitsEnabledStatus_e m_depth_write = StateBitsEnabledStatus_e::UNKNOWN;
        CullFace_e m_cull_face = CullFace_e::UNKNOWN;
        PolygonOffset_e m_polygon_offset = PolygonOffset_e::UNKNOWN;
        StateBitsEnabledStatus_e m_color_write_rgb = StateBitsEnabledStatus_e::UNKNOWN;
        StateBitsEnabledStatus_e m_color_write_alpha = StateBitsEnabledStatus_e::UNKNOWN;
        StateBitsEnabledStatus_e m_gamma_write = StateBitsEnabledStatus_e::UNKNOWN;
        StencilMode_e m_stencil_mode = StencilMode_e::UNKNOWN;
        StencilFunc_e m_stencil_front_func = StencilFunc_e::UNKNOWN;
        StencilOp_e m_stencil_front_fail = StencilOp_e::UNKNOWN;
        StencilOp_e m_stencil_front_zfail = StencilOp_e::UNKNOWN;
        StencilOp_e m_stencil_front_pass = StencilOp_e::UNKNOWN;
        StencilFunc_e m_stencil_back_func = StencilFunc_e::UNKNOWN;
        StencilOp_e m_stencil_back_fail = StencilOp_e::UNKNOWN;
        StencilOp_e m_stencil_back_zfail = StencilOp_e::UNKNOWN;
        StencilOp_e m_stencil_back_pass = StencilOp_e::UNKNOWN;
    };

    class ConstantsInfo
    {
    public:
        Vector4f m_color_tint = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        float m_env_map_min = 0.2f;
        float m_env_map_max = 1.0f;
        float m_env_map_exponent = 2.5f;
        float m_zfeather_depth = 40.0f;
        float m_eye_offset_depth = 0.0f;
        float m_falloff_begin_angle = 35.0f;
        float m_falloff_end_angle = 65.0f;
        float m_dist_falloff_begin_distance = 200.0f;
        float m_dist_falloff_end_distance = 10.0f;
        Vector4f m_falloff_begin_color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        Vector4f m_falloff_end_color = Vector4f(0.5f, 0.5f, 0.5f, 0.5f);
        Vector2f m_detail_scale = Vector2f(8.0f, 8.0f);
        Vector2f m_distortion_scale = Vector2f(0.5f, 0.5f);
        Vector4f m_color_obj_min = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
        Vector4f m_color_obj_max = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        Vector4f m_water_color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

        // Speed in which the wave animation is played
        float m_flag_speed = 1.0f;
        // Offset of the wave animation
        float m_flag_phase = 0.0f;

        float m_uv_scroll_x = 0.0f;
        float m_uv_scroll_y = 0.0f;
        float m_uv_rotate = 0.0f;
    };

    class MaterialGdtDumper
    {
        TechsetInfo m_techset_info;
        StateBitsInfo m_state_bits_info;
        ConstantsInfo m_constants_info;

        const Material* m_material;
        GdtEntry m_entry;

        void SetValue(const std::string& key, const char* value)
        {
            m_entry.m_properties.emplace(std::make_pair(key, value));
        }

        void SetValue(const std::string& key, std::string value)
        {
            m_entry.m_properties.emplace(std::make_pair(key, std::move(value)));
        }

        void SetValue(const std::string& key, const Vector4f& v)
        {
            std::ostringstream ss;
            ss << v.x() << " " << v.y() << " " << v.z() << " " << v.w();
            m_entry.m_properties.emplace(std::make_pair(key, ss.str()));
        }

        void SetValue(const std::string& key, const bool value)
        {
            m_entry.m_properties.emplace(std::make_pair(key, value ? "1" : "0"));
        }

        template <typename T,
                  typename = typename std::enable_if_t<std::is_arithmetic_v<T>, T>>
        void SetValue(const std::string& key, T value)
        {
            m_entry.m_properties.emplace(std::make_pair(key, std::to_string(value)));
        }

        void SetCommonValues()
        {
            SetValue("textureAtlasRowCount", m_material->info.textureAtlasRowCount);
            SetValue("textureAtlasColumnCount", m_material->info.textureAtlasColumnCount);
            SetValue("surfaceType", CreateSurfaceTypeString(m_material->info.surfaceTypeBits));
        }

        _NODISCARD bool MaterialCouldPossiblyUseCustomTemplate() const
        {
            if (m_material->constantCount > 0)
                return false;

            if (m_material->textureTable)
            {
                static constexpr auto COLOR_MAP_HASH = Common::R_HashString("colorMap", 0u);
                static constexpr auto DETAIL_MAP_HASH = Common::R_HashString("detailMap", 0u);

                for (auto i = 0u; i < m_material->textureCount; i++)
                {
                    const auto nameHash = m_material->textureTable[i].nameHash;
                    if (nameHash != COLOR_MAP_HASH && nameHash != DETAIL_MAP_HASH)
                        return false;
                }
            }

            return true;
        }

        static std::vector<std::string> GetTechsetNameParts(const std::string& basename)
        {
            std::vector<std::string> result;

            auto partStartPosition = 0u;
            auto currentPosition = 0u;
            for (const auto& c : basename)
            {
                if (c == '_')
                {
                    result.emplace_back(basename, partStartPosition, currentPosition - partStartPosition);
                    partStartPosition = currentPosition + 1;
                }
                currentPosition++;
            }

            if (partStartPosition < basename.size())
                result.emplace_back(basename, partStartPosition);

            return result;
        }

        void ExamineCommonUnlitTechsetInfo()
        {
            const auto nameParts = GetTechsetNameParts(m_techset_info.m_techset_base_name);
            bool inCustomName = false;
            bool customNameStart = true;
            std::ostringstream customNameStream;

            for (const auto& namePart : nameParts)
            {
                if (inCustomName)
                {
                    if (customNameStart)
                        customNameStart = false;
                    else
                        customNameStream << "_";
                    customNameStream << namePart;
                    continue;
                }

                // Anything after a custom part is part of its custom name
                if (namePart == "custom")
                {
                    inCustomName = true;
                    continue;
                }

                if (namePart == "falloff")
                    m_techset_info.m_falloff = true;
                else if (namePart == "distfalloff")
                    m_techset_info.m_dist_falloff = true;
                else if (namePart == "zfeather")
                    m_techset_info.m_zfeather = true;
                else if (namePart == "nofog")
                    m_techset_info.m_no_fog = true;
                else if (namePart == "nocast")
                    m_techset_info.m_no_cast_shadow = true;
                else if (namePart == "spot")
                    m_techset_info.m_use_spot_light = true;
                else if (namePart == "lin")
                    m_techset_info.m_effect_lin_flag = true;
                else if (namePart == "outdoor")
                    m_techset_info.m_outdoor_only = true;
                else if (namePart == "ua")
                    m_techset_info.m_uv_anim = true;
                else
                {
                    if (namePart != "add" && namePart != "replace" && namePart != "blend" && namePart != "eyeoffset" && namePart != "screen" && namePart != "effect" && namePart != "unlit"
                        && namePart != "multiply" && namePart != "sm")
                    {
                        assert(false);
                    }
                }
            }

            if (inCustomName)
            {
                m_techset_info.m_gdt_custom_string = customNameStream.str();
            }
        }

        void ExamineLitTechsetInfo()
        {
            if (!m_techset_info.m_techset_prefix.empty() && m_techset_info.m_techset_prefix[0] == 'm')
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_MODEL_PHONG;
            else
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_WORLD_PHONG;

            const auto nameParts = GetTechsetNameParts(m_techset_info.m_techset_base_name);
            bool inCustomName = false;
            bool customNameStart = true;
            std::ostringstream customNameStream;

            m_techset_info.m_no_receive_dynamic_shadow = true;
            for (const auto& namePart : nameParts)
            {
                if (namePart == "l")
                    continue;

                if (inCustomName)
                {
                    if (customNameStart)
                        customNameStart = false;
                    else
                        customNameStream << "_";
                    customNameStream << namePart;
                    continue;
                }

                // Anything after a custom part is part of its custom name
                if (namePart == "custom")
                {
                    inCustomName = true;
                    continue;
                }

                if (namePart == "scroll")
                    m_techset_info.m_tex_scroll = true;
                else if (namePart == "ua")
                    m_techset_info.m_uv_anim = true;
                else if (namePart == "nocast")
                    m_techset_info.m_no_cast_shadow = true;
                else if (namePart == "nofog")
                    m_techset_info.m_no_fog = true;
                else if (namePart == "sm" || namePart == "hsm")
                    m_techset_info.m_no_receive_dynamic_shadow = false;
                else if (namePart == "flag")
                {
                    m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                    m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_PHONG_FLAG;
                }
                else if (namePart.size() >= 2 && namePart[1] == '0')
                {
                    for (auto i = 0u; i < namePart.size(); i += 2)
                    {
                        switch (namePart[i])
                        {
                        case 'r':
                            m_state_bits_info.m_blend_func = BlendFunc_e::REPLACE;
                            m_state_bits_info.m_alpha_test = AlphaTest_e::ALWAYS;
                            break;
                        case 'a':
                            m_state_bits_info.m_blend_func = BlendFunc_e::ADD;
                            break;
                        case 'b':
                            m_state_bits_info.m_blend_func = BlendFunc_e::BLEND;
                            break;
                        case 't':
                            m_state_bits_info.m_blend_func = BlendFunc_e::REPLACE;
                            m_state_bits_info.m_alpha_test = AlphaTest_e::GE128;
                            break;
                        case 'c':
                            m_techset_info.m_has_color_map = true;
                            break;
                        case 'd':
                            m_techset_info.m_has_detail_map = true;
                            break;
                        case 'n':
                            m_techset_info.m_has_normal_map = true;
                            break;
                        case 'q':
                            m_techset_info.m_has_detail_normal_map = true;
                            break;
                        case 's':
                            m_techset_info.m_has_specular_map = true;
                            break;
                        case 'p':
                            m_techset_info.m_specular_p_flag = true;
                            break;
                        case 'o':
                            m_techset_info.m_color_o_flag = true;
                            break;
                        default:
                            assert(false);
                            break;
                        }
                    }
                }
                else
                    assert(false);
            }

            if (inCustomName)
            {
                m_techset_info.m_gdt_custom_string = customNameStream.str();
            }
        }

        void ExamineUnlitTechsetInfo()
        {
            if (!m_techset_info.m_techset_prefix.empty())
            {
                if (m_techset_info.m_techset_prefix[0] == 'm')
                    m_techset_info.m_gdt_material_type = MATERIAL_TYPE_MODEL_UNLIT;
                else
                    m_techset_info.m_gdt_material_type = MATERIAL_TYPE_WORLD_UNLIT;
            }
            else
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_UNLIT;

            ExamineCommonUnlitTechsetInfo();
        }

        void ExamineTechsetInfo()
        {
            if (!m_material->techniqueSet || !m_material->techniqueSet->name)
                return;

            m_techset_info.m_techset_name = AssetName(m_material->techniqueSet->name);
            m_techset_info.m_techset_base_name = m_techset_info.m_techset_name;

            for (auto materialType = MTL_TYPE_DEFAULT + 1; materialType < MTL_TYPE_COUNT; materialType++)
            {
                const std::string_view techsetPrefix(g_materialTypeInfo[materialType].techniqueSetPrefix);
                if (m_techset_info.m_techset_name.rfind(techsetPrefix, 0) == 0)
                {
                    m_techset_info.m_techset_base_name = m_techset_info.m_techset_name.substr(techsetPrefix.size());
                    m_techset_info.m_techset_prefix = std::string(techsetPrefix);
                    break;
                }
            }

            if (m_material->info.sortKey < SORTKEY_MAX && SortKeyNames[m_material->info.sortKey])
            {
                m_techset_info.m_sort_key_name = SortKeyNames[m_material->info.sortKey];
            }
            else
            {
                m_techset_info.m_sort_key_name = std::to_string(m_material->info.sortKey);
            }

            if (m_techset_info.m_techset_base_name == "2d")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_2D;
            }
            else if (m_techset_info.m_techset_base_name == "tools")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_TOOLS;
            }
            else if (m_techset_info.m_techset_base_name == "objective")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_OBJECTIVE;
            }
            else if (m_techset_info.m_techset_base_name == "sky")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_SKY;
            }
            else if (m_techset_info.m_techset_base_name == "water")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_WATER;
            }
            else if (m_techset_info.m_techset_base_name.rfind("ambient_", 0) == 0)
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_MODEL_AMBIENT;
            }
            else if (m_techset_info.m_techset_base_name.rfind("distortion_", 0) == 0)
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_DISTORTION;
            }
            else if (m_techset_info.m_techset_base_name.rfind("particle_cloud", 0) == 0)
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_PARTICLE_CLOUD;
            }
            else if (m_techset_info.m_techset_base_name == "grain_overlay")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_GRAIN_OVERLAY;
            }
            else if (m_techset_info.m_techset_base_name == "effect_add_eyeoffset")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_EFFECT_EYE_OFFSET;
            }
            else if (m_techset_info.m_techset_base_name == "reflexsight")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_REFLEX_SIGHT;
            }
            else if (m_techset_info.m_techset_base_name == "shadowclear")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_SHADOW_CLEAR;
            }
            else if (m_techset_info.m_techset_base_name == "shadowoverlay")
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_SHADOW_OVERLAY;
            }
            else if (m_techset_info.m_techset_base_name.rfind("splatter", 0) == 0)
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_SPLATTER;
            }
            else if (m_techset_info.m_techset_base_name.rfind("effect", 0) == 0)
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_EFFECT;
                ExamineCommonUnlitTechsetInfo();
            }
            else if (m_techset_info.m_techset_base_name.rfind("l_", 0) == 0)
            {
                ExamineLitTechsetInfo();
            }
            else if (m_techset_info.m_techset_base_name.rfind("unlit", 0) == 0)
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_UNLIT;
                ExamineUnlitTechsetInfo();
            }
            else if (MaterialCouldPossiblyUseCustomTemplate())
            {
                m_techset_info.m_gdt_material_type = MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_material_type = CUSTOM_MATERIAL_TYPE_CUSTOM;
                m_techset_info.m_gdt_custom_string = m_techset_info.m_techset_base_name;
            }
            else
            {
                std::cout << "Could not determine material type for material \"" << m_material->info.name << "\"\n";
            }
        }

        struct BlendFuncParameters
        {
            BlendFunc_e m_blend_func;
            BlendOp_e m_blend_op_rgb;
            CustomBlendFunc_e m_src_blend_func;
            CustomBlendFunc_e m_dst_blend_func;
            BlendOp_e m_blend_op_alpha;
            CustomBlendFunc_e m_src_blend_func_alpha;
            CustomBlendFunc_e m_dst_blend_func_alpha;
        };

        static inline BlendFuncParameters knownBlendFuncs[]
        {
            // Only considering passthrough statemap
            {BlendFunc_e::ADD, BlendOp_e::ADD, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ONE, BlendOp_e::DISABLE, CustomBlendFunc_e::UNKNOWN, CustomBlendFunc_e::UNKNOWN},
            {BlendFunc_e::BLEND, BlendOp_e::ADD, CustomBlendFunc_e::SRC_ALPHA, CustomBlendFunc_e::INV_SRC_ALPHA, BlendOp_e::DISABLE, CustomBlendFunc_e::UNKNOWN, CustomBlendFunc_e::UNKNOWN},
            {BlendFunc_e::MULTIPLY, BlendOp_e::ADD, CustomBlendFunc_e::ZERO, CustomBlendFunc_e::SRC_COLOR, BlendOp_e::DISABLE, CustomBlendFunc_e::UNKNOWN, CustomBlendFunc_e::UNKNOWN},
            {BlendFunc_e::REPLACE, BlendOp_e::DISABLE, CustomBlendFunc_e::UNKNOWN, CustomBlendFunc_e::UNKNOWN, BlendOp_e::DISABLE, CustomBlendFunc_e::UNKNOWN, CustomBlendFunc_e::UNKNOWN},
            {BlendFunc_e::SCREEN_ADD, BlendOp_e::ADD, CustomBlendFunc_e::INV_DST_COLOR, CustomBlendFunc_e::ONE, BlendOp_e::DISABLE, CustomBlendFunc_e::UNKNOWN, CustomBlendFunc_e::UNKNOWN},

            // TODO: Enable when using statemaps
            // Considering default statemap
            //{BlendFunc_e::ADD, BlendOp_e::ADD, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ONE, BlendOp_e::ADD, CustomBlendFunc_e::INV_DST_ALPHA, CustomBlendFunc_e::ONE},
            //{BlendFunc_e::BLEND, BlendOp_e::ADD, CustomBlendFunc_e::SRC_ALPHA, CustomBlendFunc_e::INV_SRC_ALPHA, BlendOp_e::ADD, CustomBlendFunc_e::INV_DST_ALPHA, CustomBlendFunc_e::ONE},
            //{BlendFunc_e::MULTIPLY, BlendOp_e::ADD, CustomBlendFunc_e::ZERO, CustomBlendFunc_e::SRC_COLOR, BlendOp_e::ADD, CustomBlendFunc_e::INV_DST_ALPHA, CustomBlendFunc_e::ONE},
            //// REPLACE matches passthrough statemap
            //{BlendFunc_e::SCREEN_ADD, BlendOp_e::ADD, CustomBlendFunc_e::INV_DST_COLOR, CustomBlendFunc_e::ONE, BlendOp_e::ADD, CustomBlendFunc_e::INV_DST_ALPHA, CustomBlendFunc_e::ONE},
        };

        template <typename T>
        bool KnownBlendFuncParameterMatches(const T materialValue, const T blendFuncValue)
        {
            if (blendFuncValue == T::UNKNOWN)
                return true;
            if (materialValue == T::UNKNOWN)
                return false;

            return static_cast<unsigned>(materialValue) == static_cast<unsigned>(blendFuncValue);
        }

        void ExamineBlendFunc()
        {
            if (m_state_bits_info.m_blend_func != BlendFunc_e::UNKNOWN)
                return;

            for (const auto& knownBlendFunc : knownBlendFuncs)
            {
                if (KnownBlendFuncParameterMatches(m_state_bits_info.m_custom_blend_op_rgb, knownBlendFunc.m_blend_op_rgb)
                    && KnownBlendFuncParameterMatches(m_state_bits_info.m_custom_src_blend_func, knownBlendFunc.m_src_blend_func)
                    && KnownBlendFuncParameterMatches(m_state_bits_info.m_custom_dst_blend_func, knownBlendFunc.m_dst_blend_func)
                    && KnownBlendFuncParameterMatches(m_state_bits_info.m_custom_blend_op_alpha, knownBlendFunc.m_blend_op_alpha)
                    && KnownBlendFuncParameterMatches(m_state_bits_info.m_custom_src_blend_func_alpha, knownBlendFunc.m_src_blend_func_alpha)
                    && KnownBlendFuncParameterMatches(m_state_bits_info.m_custom_dst_blend_func_alpha, knownBlendFunc.m_dst_blend_func_alpha)
                )
                {
                    m_state_bits_info.m_blend_func = knownBlendFunc.m_blend_func;
                    return;
                }
            }

            m_state_bits_info.m_blend_func = BlendFunc_e::CUSTOM;
        }

        template <typename T>
        T StateBitsToEnum(const unsigned input, const size_t mask, const size_t shift)
        {
            const unsigned value = (input & mask) >> shift;
            return value >= (static_cast<unsigned>(T::COUNT) - 1) ? T::UNKNOWN : static_cast<T>(value + 1);
        }

        void ExamineStateBitsInfo()
        {
            if (!m_material->stateBitsTable || m_material->stateBitsCount == 0)
                return;

            // This assumes the statemap of these techniques is passthrough which it is most likely not
            // This should still not produce any wrong values
            GfxStateBits stateBits{};
            if (m_material->stateBitsEntry[TECHNIQUE_LIT] < m_material->stateBitsCount)
                stateBits = m_material->stateBitsTable[m_material->stateBitsEntry[TECHNIQUE_LIT]];
            else if (m_material->stateBitsEntry[TECHNIQUE_EMISSIVE] < m_material->stateBitsCount)
                stateBits = m_material->stateBitsTable[m_material->stateBitsEntry[TECHNIQUE_EMISSIVE]];
            else if (m_material->stateBitsEntry[TECHNIQUE_UNLIT] < m_material->stateBitsCount)
                stateBits = m_material->stateBitsTable[m_material->stateBitsEntry[TECHNIQUE_UNLIT]];
            else if (m_material->stateBitsEntry[TECHNIQUE_DEPTH_PREPASS] < m_material->stateBitsCount)
                stateBits = m_material->stateBitsTable[m_material->stateBitsEntry[TECHNIQUE_DEPTH_PREPASS]];
            else
            {
                assert(false);
                return;
            }

            if (m_state_bits_info.m_custom_blend_op_rgb == BlendOp_e::UNKNOWN)
                m_state_bits_info.m_custom_blend_op_rgb = StateBitsToEnum<BlendOp_e>(stateBits.loadBits[0], GFXS0_BLENDOP_RGB_MASK, GFXS0_BLENDOP_RGB_SHIFT);

            if (m_state_bits_info.m_custom_blend_op_alpha == BlendOp_e::UNKNOWN)
                m_state_bits_info.m_custom_blend_op_alpha = StateBitsToEnum<BlendOp_e>(stateBits.loadBits[0], GFXS0_BLENDOP_ALPHA_MASK, GFXS0_BLENDOP_ALPHA_SHIFT);

            if (m_state_bits_info.m_custom_src_blend_func == CustomBlendFunc_e::UNKNOWN)
                m_state_bits_info.m_custom_src_blend_func = StateBitsToEnum<CustomBlendFunc_e>(stateBits.loadBits[0], GFXS0_SRCBLEND_RGB_MASK, GFXS0_SRCBLEND_RGB_SHIFT);

            if (m_state_bits_info.m_custom_dst_blend_func == CustomBlendFunc_e::UNKNOWN)
                m_state_bits_info.m_custom_dst_blend_func = StateBitsToEnum<CustomBlendFunc_e>(stateBits.loadBits[0], GFXS0_DSTBLEND_RGB_MASK, GFXS0_DSTBLEND_RGB_SHIFT);

            if (m_state_bits_info.m_custom_src_blend_func_alpha == CustomBlendFunc_e::UNKNOWN)
                m_state_bits_info.m_custom_src_blend_func_alpha = StateBitsToEnum<CustomBlendFunc_e>(stateBits.loadBits[0], GFXS0_SRCBLEND_ALPHA_MASK, GFXS0_SRCBLEND_ALPHA_SHIFT);

            if (m_state_bits_info.m_custom_dst_blend_func_alpha == CustomBlendFunc_e::UNKNOWN)
                m_state_bits_info.m_custom_dst_blend_func_alpha = StateBitsToEnum<CustomBlendFunc_e>(stateBits.loadBits[0], GFXS0_DSTBLEND_ALPHA_MASK, GFXS0_DSTBLEND_ALPHA_SHIFT);

            if (m_state_bits_info.m_alpha_test == AlphaTest_e::UNKNOWN)
            {
                if (stateBits.loadBits[0] & GFXS0_ATEST_DISABLE)
                    m_state_bits_info.m_alpha_test = AlphaTest_e::ALWAYS;
                else if ((stateBits.loadBits[0] & GFXS0_ATEST_MASK) == GFXS0_ATEST_GE_128)
                    m_state_bits_info.m_alpha_test = AlphaTest_e::GE128;
                else if ((stateBits.loadBits[0] & GFXS0_ATEST_MASK) == GFXS0_ATEST_GT_0)
                    m_state_bits_info.m_alpha_test = AlphaTest_e::GT0;
                else if ((stateBits.loadBits[0] & GFXS0_ATEST_MASK) == GFXS0_ATEST_LT_128)
                    m_state_bits_info.m_alpha_test = AlphaTest_e::LT128;
                else
                    assert(false);
            }

            if (m_state_bits_info.m_depth_test == DepthTest_e::UNKNOWN)
            {
                if (stateBits.loadBits[1] & GFXS1_DEPTHTEST_DISABLE)
                    m_state_bits_info.m_depth_test = DepthTest_e::DISABLE;
                else if (stateBits.loadBits[1] & GFXS1_DEPTHTEST_LESSEQUAL)
                    m_state_bits_info.m_depth_test = DepthTest_e::LESS_EQUAL;
                else if (stateBits.loadBits[1] & GFXS1_DEPTHTEST_LESS)
                    m_state_bits_info.m_depth_test = DepthTest_e::LESS;
                else if (stateBits.loadBits[1] & GFXS1_DEPTHTEST_EQUAL)
                    m_state_bits_info.m_depth_test = DepthTest_e::EQUAL;
                else
                    m_state_bits_info.m_depth_test = DepthTest_e::ALWAYS;
            }

            if (m_state_bits_info.m_depth_write == StateBitsEnabledStatus_e::UNKNOWN)
                m_state_bits_info.m_depth_write = (stateBits.loadBits[1] & GFXS1_DEPTHWRITE) ? StateBitsEnabledStatus_e::ENABLED : StateBitsEnabledStatus_e::DISABLED;

            if (m_state_bits_info.m_cull_face == CullFace_e::UNKNOWN)
            {
                if (stateBits.loadBits[0] & GFXS0_CULL_NONE)
                    m_state_bits_info.m_cull_face = CullFace_e::NONE;
                else if (stateBits.loadBits[0] & GFXS0_CULL_BACK)
                    m_state_bits_info.m_cull_face = CullFace_e::BACK;
                else if (stateBits.loadBits[0] & GFXS0_CULL_FRONT)
                    m_state_bits_info.m_cull_face = CullFace_e::FRONT;
                else
                    assert(false);
            }

            if (m_state_bits_info.m_polygon_offset == PolygonOffset_e::UNKNOWN)
                m_state_bits_info.m_polygon_offset = StateBitsToEnum<PolygonOffset_e>(stateBits.loadBits[1], GFXS1_POLYGON_OFFSET_MASK, GFXS1_POLYGON_OFFSET_SHIFT);

            if (m_state_bits_info.m_color_write_rgb == StateBitsEnabledStatus_e::UNKNOWN)
                m_state_bits_info.m_color_write_rgb = (stateBits.loadBits[0] & GFXS0_COLORWRITE_RGB) ? StateBitsEnabledStatus_e::ENABLED : StateBitsEnabledStatus_e::DISABLED;

            if (m_state_bits_info.m_color_write_alpha == StateBitsEnabledStatus_e::UNKNOWN)
                m_state_bits_info.m_color_write_alpha = (stateBits.loadBits[0] & GFXS0_COLORWRITE_ALPHA) ? StateBitsEnabledStatus_e::ENABLED : StateBitsEnabledStatus_e::DISABLED;

            if (m_state_bits_info.m_gamma_write == StateBitsEnabledStatus_e::UNKNOWN)
                m_state_bits_info.m_gamma_write = (stateBits.loadBits[0] & GFXS0_GAMMAWRITE) ? StateBitsEnabledStatus_e::ENABLED : StateBitsEnabledStatus_e::DISABLED;

            if (m_state_bits_info.m_stencil_mode == StencilMode_e::UNKNOWN)
            {
                if ((stateBits.loadBits[1] & GFXS1_STENCIL_BACK_ENABLE) == 0 && (stateBits.loadBits[1] & GFXS1_STENCIL_FRONT_ENABLE) == 0)
                {
                    m_state_bits_info.m_stencil_mode = StencilMode_e::DISABLED;
                }
                else if (stateBits.loadBits[1] & GFXS1_STENCIL_BACK_ENABLE)
                {
                    assert(stateBits.loadBits[1] & GFXS1_STENCIL_FRONT_ENABLE);
                    m_state_bits_info.m_stencil_mode = StencilMode_e::TWO_SIDED;
                }
                else
                {
                    assert(stateBits.loadBits[1] & GFXS1_STENCIL_FRONT_ENABLE);
                    m_state_bits_info.m_stencil_mode = StencilMode_e::ONE_SIDED;
                }
            }

            if (m_state_bits_info.m_stencil_front_func == StencilFunc_e::UNKNOWN)
                m_state_bits_info.m_stencil_front_func = StateBitsToEnum<StencilFunc_e>(stateBits.loadBits[1], GFXS1_STENCIL_FRONT_FUNC_MASK, GFXS1_STENCIL_FRONT_FUNC_SHIFT);

            if (m_state_bits_info.m_stencil_front_pass == StencilOp_e::UNKNOWN)
                m_state_bits_info.m_stencil_front_pass = StateBitsToEnum<StencilOp_e>(stateBits.loadBits[1], GFXS1_STENCIL_FRONT_PASS_MASK, GFXS1_STENCIL_FRONT_PASS_SHIFT);

            if (m_state_bits_info.m_stencil_front_fail == StencilOp_e::UNKNOWN)
                m_state_bits_info.m_stencil_front_fail = StateBitsToEnum<StencilOp_e>(stateBits.loadBits[1], GFXS1_STENCIL_FRONT_FAIL_MASK, GFXS1_STENCIL_FRONT_FAIL_SHIFT);

            if (m_state_bits_info.m_stencil_front_zfail == StencilOp_e::UNKNOWN)
                m_state_bits_info.m_stencil_front_zfail = StateBitsToEnum<StencilOp_e>(stateBits.loadBits[1], GFXS1_STENCIL_FRONT_ZFAIL_MASK, GFXS1_STENCIL_FRONT_ZFAIL_SHIFT);

            if (m_state_bits_info.m_stencil_back_func == StencilFunc_e::UNKNOWN)
                m_state_bits_info.m_stencil_back_func = StateBitsToEnum<StencilFunc_e>(stateBits.loadBits[1], GFXS1_STENCIL_BACK_FUNC_MASK, GFXS1_STENCIL_BACK_FUNC_SHIFT);

            if (m_state_bits_info.m_stencil_back_pass == StencilOp_e::UNKNOWN)
                m_state_bits_info.m_stencil_back_pass = StateBitsToEnum<StencilOp_e>(stateBits.loadBits[1], GFXS1_STENCIL_BACK_PASS_MASK, GFXS1_STENCIL_BACK_PASS_SHIFT);

            if (m_state_bits_info.m_stencil_back_fail == StencilOp_e::UNKNOWN)
                m_state_bits_info.m_stencil_back_fail = StateBitsToEnum<StencilOp_e>(stateBits.loadBits[1], GFXS1_STENCIL_BACK_FAIL_MASK, GFXS1_STENCIL_BACK_FAIL_SHIFT);

            if (m_state_bits_info.m_stencil_back_zfail == StencilOp_e::UNKNOWN)
                m_state_bits_info.m_stencil_back_zfail = StateBitsToEnum<StencilOp_e>(stateBits.loadBits[1], GFXS1_STENCIL_BACK_ZFAIL_MASK, GFXS1_STENCIL_BACK_ZFAIL_SHIFT);

            ExamineBlendFunc();
        }

        _NODISCARD int FindConstant(const std::string& constantName) const
        {
            const auto constantHash = Common::R_HashString(constantName.c_str(), 0u);

            if (m_material->constantTable)
            {
                for (auto i = 0; i < m_material->constantCount; i++)
                {
                    if (m_material->constantTable[i].nameHash == constantHash)
                        return i;
                }
            }

            return -1;
        }

        _NODISCARD int FindTexture(const std::string& textureTypeName) const
        {
            const auto textureTypeHash = Common::R_HashString(textureTypeName.c_str(), 0u);

            if (m_material->textureTable)
            {
                for (auto i = 0; i < m_material->textureCount; i++)
                {
                    if (m_material->textureTable[i].nameHash == textureTypeHash)
                        return i;
                }
            }

            return -1;
        }

        void ExamineConstants()
        {
            if (!m_material->constantTable)
                return;

            for (auto i = 0u; i < m_material->constantCount; i++)
            {
                const auto& constant = m_material->constantTable[i];

                if (constant.nameHash == Common::R_HashString("colorTint"))
                {
                    m_constants_info.m_color_tint = Vector4f(constant.literal);
                }
                else if (constant.nameHash == Common::R_HashString("envMapParms"))
                {
                    // TODO: Calculate actual values
                    m_constants_info.m_env_map_min = 0;
                    m_constants_info.m_env_map_max = 0;
                    m_constants_info.m_env_map_exponent = 0;
                }
                else if (constant.nameHash == Common::R_HashString("featherParms"))
                {
                    m_constants_info.m_zfeather_depth = constant.literal[1];
                }
                else if (constant.nameHash == Common::R_HashString("falloffBeginColor"))
                {
                    m_constants_info.m_falloff_begin_color = Vector4f(constant.literal);
                }
                else if (constant.nameHash == Common::R_HashString("falloffEndColor"))
                {
                    m_constants_info.m_falloff_end_color = Vector4f(constant.literal);
                }
                else if (constant.nameHash == Common::R_HashString("eyeOffsetParms"))
                {
                    m_constants_info.m_eye_offset_depth = constant.literal[0];
                }
                else if (constant.nameHash == Common::R_HashString("detailScale"))
                {
                    const auto materialType = m_techset_info.m_gdt_material_type;
                    const auto colorMapIndex = FindTexture("colorMap");
                    const auto detailMapIndex = FindTexture("detailMap");
                    const auto hasColorMap = colorMapIndex >= 0 && m_material->textureTable[colorMapIndex].semantic != TS_WATER_MAP && m_material->textureTable[colorMapIndex].u.image;
                    const auto hasDetailMap = detailMapIndex >= 0 && m_material->textureTable[detailMapIndex].semantic != TS_WATER_MAP && m_material->textureTable[detailMapIndex].u.image;

                    if ((materialType == MATERIAL_TYPE_MODEL_PHONG || materialType == MATERIAL_TYPE_WORLD_PHONG)
                        && hasColorMap && hasDetailMap)
                    {
                        const auto colorMapTexture = m_material->textureTable[colorMapIndex].u.image;
                        const auto detailMapTexture = m_material->textureTable[detailMapIndex].u.image;

                        if (colorMapTexture->width != 0 && colorMapTexture->height != 0
                            && detailMapTexture->width != 0 && detailMapTexture->height != 0)
                        {
                            const auto detailScaleFactorX = static_cast<float>(colorMapTexture->width) / static_cast<float>(detailMapTexture->width);
                            const auto detailScaleFactorY = static_cast<float>(colorMapTexture->height) / static_cast<float>(detailMapTexture->height);
                            m_constants_info.m_detail_scale = Vector2f(constant.literal[0] / detailScaleFactorX, constant.literal[1] / detailScaleFactorY);
                        }
                        else
                            m_constants_info.m_detail_scale = Vector2f(constant.literal[0], constant.literal[1]);
                    }
                    else
                    {
                        m_constants_info.m_detail_scale = Vector2f(constant.literal[0], constant.literal[1]);
                    }
                }
                else if (constant.nameHash == Common::R_HashString("flagParms"))
                {
                    m_constants_info.m_flag_speed = constant.literal[0];
                    m_constants_info.m_flag_phase = constant.literal[1];
                }
                else if (constant.nameHash == Common::R_HashString("falloffParms"))
                {
                    // TODO: Calculate actual values
                    m_constants_info.m_falloff_begin_angle = 0.0f;
                    m_constants_info.m_falloff_end_angle = 0.0f;
                    m_constants_info.m_dist_falloff_begin_distance = 0.0f;
                    m_constants_info.m_dist_falloff_end_distance = 0.0f;
                }
                else if (constant.nameHash == Common::R_HashString("distortionScale"))
                {
                    m_constants_info.m_distortion_scale = Vector2f(constant.literal[0], constant.literal[1]);
                }
                else if (constant.nameHash == Common::R_HashString("uvAnimParms"))
                {
                    m_constants_info.m_uv_scroll_x = constant.literal[0];
                    m_constants_info.m_uv_scroll_y = constant.literal[1];
                    m_constants_info.m_uv_rotate = constant.literal[2];
                }
                else if (constant.nameHash == Common::R_HashString("colorObjMin"))
                {
                    m_constants_info.m_color_obj_min = Vector4f(constant.literal);
                }
                else if (constant.nameHash == Common::R_HashString("colorObjMax"))
                {
                    m_constants_info.m_color_obj_max = Vector4f(constant.literal);
                }
                else if (constant.nameHash == Common::R_HashString("waterColor"))
                {
                    m_constants_info.m_water_color = Vector4f(constant.literal);
                }
                else
                {
                    std::string constantNamePart(constant.name, strnlen(constant.name, std::extent_v<decltype(constant.name)>));
                    std::cout << "Unknown constant: " << constantNamePart << "\n";
                }
            }
        }

        void SetMaterialTypeValues()
        {
            ExamineTechsetInfo();
            ExamineStateBitsInfo();
            ExamineConstants();

            SetValue("materialType", GdtMaterialTypeNames[static_cast<size_t>(m_techset_info.m_gdt_material_type)]);
            SetValue("customTemplate", GdtCustomMaterialTypeNames[static_cast<size_t>(m_techset_info.m_gdt_custom_material_type)]);
            SetValue("customString", m_techset_info.m_gdt_custom_string);
            SetValue("sort", m_techset_info.m_sort_key_name);
            SetValue("noCastShadow", m_techset_info.m_no_cast_shadow);
            SetValue("noReceiveDynamicShadow", m_techset_info.m_no_receive_dynamic_shadow);
            SetValue("noFog", m_techset_info.m_no_fog);
            SetValue("texScroll", m_techset_info.m_tex_scroll);
            SetValue("uvAnim", m_techset_info.m_uv_anim);
            SetValue("zFeather", m_techset_info.m_zfeather);
            SetValue("zFeatherDepth", m_constants_info.m_zfeather_depth);
            SetValue("useSpotLight", m_techset_info.m_use_spot_light);
            SetValue("falloff", m_techset_info.m_falloff);
            SetValue("distFalloff", m_techset_info.m_dist_falloff);
            SetValue("outdoorOnly", m_techset_info.m_outdoor_only);
            SetValue("eyeOffsetDepth", m_constants_info.m_eye_offset_depth);


            // TODO: These are not good names, change when known what they do
            SetValue("specularP", m_techset_info.m_specular_p_flag);
            SetValue("colorO", m_techset_info.m_color_o_flag);
            SetValue("effectLinFlag", m_techset_info.m_effect_lin_flag);

            SetValue("blendFunc", GdtBlendFuncNames[static_cast<size_t>(m_state_bits_info.m_blend_func)]);
            SetValue("customBlendOpRgb", GdtBlendOpNames[static_cast<size_t>(m_state_bits_info.m_custom_blend_op_rgb)]);
            SetValue("customBlendOpAlpha", GdtBlendOpNames[static_cast<size_t>(m_state_bits_info.m_custom_blend_op_alpha)]);
            SetValue("srcCustomBlendFunc", GdtCustomBlendFuncNames[static_cast<size_t>(m_state_bits_info.m_custom_src_blend_func)]);
            SetValue("destCustomBlendFunc", GdtCustomBlendFuncNames[static_cast<size_t>(m_state_bits_info.m_custom_dst_blend_func)]);
            SetValue("srcCustomBlendFuncAlpha", GdtCustomBlendFuncNames[static_cast<size_t>(m_state_bits_info.m_custom_src_blend_func_alpha)]);
            SetValue("destCustomBlendFuncAlpha", GdtCustomBlendFuncNames[static_cast<size_t>(m_state_bits_info.m_custom_dst_blend_func_alpha)]);
            SetValue("alphaTest", GdtAlphaTestNames[static_cast<size_t>(m_state_bits_info.m_alpha_test)]);
            SetValue("depthTest", GdtDepthTestNames[static_cast<size_t>(m_state_bits_info.m_depth_test)]);
            SetValue("depthWrite", GdtStateBitsOnOffStatusNames[static_cast<size_t>(m_state_bits_info.m_depth_write)]);
            SetValue("cullFace", GdtCullFaceNames[static_cast<size_t>(m_state_bits_info.m_cull_face)]);
            SetValue("polygonOffset", GdtPolygonOffsetNames[static_cast<size_t>(m_state_bits_info.m_polygon_offset)]);
            SetValue("colorWriteRed", GdtStateBitsEnabledStatusNames[static_cast<size_t>(m_state_bits_info.m_color_write_rgb)]);
            SetValue("colorWriteGreen", GdtStateBitsEnabledStatusNames[static_cast<size_t>(m_state_bits_info.m_color_write_rgb)]);
            SetValue("colorWriteBlue", GdtStateBitsEnabledStatusNames[static_cast<size_t>(m_state_bits_info.m_color_write_rgb)]);
            SetValue("colorWriteAlpha", GdtStateBitsEnabledStatusNames[static_cast<size_t>(m_state_bits_info.m_color_write_alpha)]);
            SetValue("gammaWrite", GdtStateBitsOnOffStatusNames[static_cast<size_t>(m_state_bits_info.m_gamma_write)]);
            SetValue("stencil", GdtStencilModeNames[static_cast<size_t>(m_state_bits_info.m_stencil_mode)]);
            SetValue("stencilFunc1", GdtStencilFuncNames[static_cast<size_t>(m_state_bits_info.m_stencil_front_func)]);
            SetValue("stencilOpPass1", GdtStencilOpNames[static_cast<size_t>(m_state_bits_info.m_stencil_front_pass)]);
            SetValue("stencilOpFail1", GdtStencilOpNames[static_cast<size_t>(m_state_bits_info.m_stencil_front_fail)]);
            SetValue("stencilOpZFail1", GdtStencilOpNames[static_cast<size_t>(m_state_bits_info.m_stencil_front_zfail)]);
            SetValue("stencilFunc2", GdtStencilFuncNames[static_cast<size_t>(m_state_bits_info.m_stencil_back_func)]);
            SetValue("stencilOpPass2", GdtStencilOpNames[static_cast<size_t>(m_state_bits_info.m_stencil_back_pass)]);
            SetValue("stencilOpFail2", GdtStencilOpNames[static_cast<size_t>(m_state_bits_info.m_stencil_back_fail)]);
            SetValue("stencilOpZFail2", GdtStencilOpNames[static_cast<size_t>(m_state_bits_info.m_stencil_back_zfail)]);

            SetValue("colorTint", m_constants_info.m_color_tint);
            SetValue("envMapMin", m_constants_info.m_env_map_min);
            SetValue("envMapMax", m_constants_info.m_env_map_max);
            SetValue("envMapExponent", m_constants_info.m_env_map_exponent);
            SetValue("zFeatherDepth", m_constants_info.m_zfeather_depth);
            SetValue("eyeOffsetDepth", m_constants_info.m_eye_offset_depth);
            SetValue("falloffBeginColor", m_constants_info.m_falloff_begin_color);
            SetValue("falloffEndColor", m_constants_info.m_falloff_end_color);
            SetValue("detailScaleX", m_constants_info.m_detail_scale.x());
            SetValue("detailScaleY", m_constants_info.m_detail_scale.y());
            SetValue("distortionScaleX", m_constants_info.m_distortion_scale.x());
            SetValue("distortionScaleY", m_constants_info.m_distortion_scale.y());
            SetValue("colorObjMin", m_constants_info.m_color_obj_min);
            SetValue("colorObjMax", m_constants_info.m_color_obj_max);
            SetValue("waterColor", m_constants_info.m_water_color);
            SetValue("flagSpeed", m_constants_info.m_flag_speed);
            SetValue("flagPhase", m_constants_info.m_flag_phase);
        }

        void SetTextureTableValues()
        {
            if (m_material->textureTable == nullptr || m_material->textureCount <= 0)
                return;

            for (auto i = 0u; i < m_material->textureCount; i++)
            {
                const auto& entry = m_material->textureTable[i];
                const auto knownMaterialSourceName = knownTextureMaps.find(entry.nameHash);
                if (knownMaterialSourceName == knownTextureMaps.end())
                {
                    assert(false);
                    std::cout << "Unknown material texture source name hash: 0x" << std::hex << entry.nameHash << " (" << entry.nameStart << "..." << entry.nameEnd << ")\n";
                    continue;
                }

                const char* imageName;
                if (entry.semantic != TS_WATER_MAP)
                {
                    if (!entry.u.image || !entry.u.image->name)
                        continue;
                    imageName = AssetName(entry.u.image->name);
                }
                else
                {
                    if (!entry.u.water || !entry.u.water->image || !entry.u.water->image->name)
                        continue;
                    imageName = AssetName(entry.u.water->image->name);
                }

                TileMode_e tileMode;
                if (entry.samplerState & SAMPLER_CLAMP_U && entry.samplerState & SAMPLER_CLAMP_V && entry.samplerState & SAMPLER_CLAMP_W)
                    tileMode = TileMode_e::TILE_BOTH;
                else if (entry.samplerState & SAMPLER_CLAMP_U)
                    tileMode = TileMode_e::TILE_VERTICAL;
                else if (entry.samplerState & SAMPLER_CLAMP_V)
                    tileMode = TileMode_e::TILE_HORIZONTAL;
                else
                    tileMode = TileMode_e::NO_TILE;

                auto filter = GdtFilter_e::UNKNOWN;
                if ((entry.samplerState & SAMPLER_FILTER_MASK) == SAMPLER_FILTER_ANISO2X)
                {
                    if (entry.samplerState & SAMPLER_MIPMAP_NEAREST)
                        filter = GdtFilter_e::MIP_2X_BILINEAR;
                    else if (entry.samplerState & SAMPLER_MIPMAP_LINEAR)
                        filter = GdtFilter_e::MIP_2X_TRILINEAR;
                }
                else if ((entry.samplerState & SAMPLER_FILTER_MASK) == SAMPLER_FILTER_ANISO4X)
                {
                    if (entry.samplerState & SAMPLER_MIPMAP_NEAREST)
                        filter = GdtFilter_e::MIP_4X_BILINEAR;
                    else if (entry.samplerState & SAMPLER_MIPMAP_LINEAR)
                        filter = GdtFilter_e::MIP_4X_TRILINEAR;
                }
                else if ((entry.samplerState & SAMPLER_FILTER_MASK) == SAMPLER_FILTER_NEAREST)
                {
                    assert((entry.samplerState & SAMPLER_MIPMAP_MASK) == SAMPLER_MIPMAP_DISABLED);
                    filter = GdtFilter_e::NOMIP_NEAREST;
                }
                else if ((entry.samplerState & SAMPLER_FILTER_MASK) == SAMPLER_FILTER_LINEAR)
                {
                    assert((entry.samplerState & SAMPLER_MIPMAP_MASK) == SAMPLER_MIPMAP_DISABLED);
                    filter = GdtFilter_e::NOMIP_BILINEAR;
                }

                assert(filter != GdtFilter_e::UNKNOWN);
                if (filter == GdtFilter_e::UNKNOWN)
                {
                    std::cout << "Unknown filter/mipmap combination: " << entry.samplerState << "\n";
                    continue;
                }

                SetValue(knownMaterialSourceName->second.m_name, imageName);
                SetValue("tile"s + knownMaterialSourceName->second.m_additional_property_suffix, GdtTileModeNames[static_cast<size_t>(tileMode)]);
                SetValue("filter"s + knownMaterialSourceName->second.m_additional_property_suffix, GdtSamplerFilterNames[static_cast<size_t>(filter)]);
            }
        }

    public:
        explicit MaterialGdtDumper(const Material* material)
            : m_material(material)
        {
        }

        GdtEntry& CreateGdtEntry()
        {
            m_entry = GdtEntry();
            m_entry.m_gdf_name = ObjConstants::GDF_FILENAME_MATERIAL;
            m_entry.m_name = m_material->info.name;

            SetCommonValues();
            SetMaterialTypeValues();
            SetTextureTableValues();

            return m_entry;
        }
    };
}

bool AssetDumperMaterial::ShouldDump(XAssetInfo<Material>* asset)
{
    return true;
}

void AssetDumperMaterial::DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset)
{
    auto* material = asset->Asset();

#if defined(DUMP_AS_JSON) && DUMP_AS_JSON == 1
    {
        std::ostringstream ss;
        ss << "materials/" << asset->m_name << ".json";
        const auto assetFile = context.OpenAssetFile(ss.str());
        if (!assetFile)
            return;
        auto& stream = *assetFile;
        DumpMaterialAsJson(material, stream);
    }
#endif

#if defined(DUMP_AS_GDT) && DUMP_AS_GDT == 1
    {
        std::ostringstream ss;
        ss << "materials/" << asset->m_name << ".gdt";
        const auto assetFile = context.OpenAssetFile(ss.str());
        if (!assetFile)
            return;
        auto& stream = *assetFile;
        MaterialGdtDumper dumper(material);
        Gdt gdt(GdtVersion("IW4", 1));
        gdt.m_entries.emplace_back(std::make_unique<GdtEntry>(std::move(dumper.CreateGdtEntry())));
        GdtOutputStream::WriteGdt(gdt, stream);
    }
#endif

    if (context.m_gdt)
    {
        MaterialGdtDumper dumper(material);
        context.m_gdt->WriteEntry(dumper.CreateGdtEntry());
    }
}
