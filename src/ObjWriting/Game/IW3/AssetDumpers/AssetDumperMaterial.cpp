#include "AssetDumperMaterial.h"

#include "Game/IW3/MaterialConstantsIW3.h"
#include "Game/IW3/TechsetConstantsIW3.h"

#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>

// #define FLAGS_DEBUG 1

using namespace IW3;
using json = nlohmann::json;

namespace IW3
{
    const char* AssetName(const char* name)
    {
        if (name && name[0] == ',')
            return &name[1];
        return name;
    }

    template<size_t S> json ArrayEntry(const char* (&a)[S], const size_t index)
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
        static const char* samplerFilterNames[]{"none", "nearest", "linear", "aniso2x", "aniso4x"};
        static const char* samplerMipmapNames[]{"disabled", "nearest", "linear"};

        return json{
            {"filter", ArrayEntry(samplerFilterNames, (samplerState & SAMPLER_FILTER_MASK) >> SAMPLER_FILTER_SHIFT)},
            {"mipmap", ArrayEntry(samplerMipmapNames, (samplerState & SAMPLER_MIPMAP_MASK) >> SAMPLER_MIPMAP_SHIFT)},
            {"clampU", (samplerState & SAMPLER_CLAMP_U) ? true : false},
            {"clampV", (samplerState & SAMPLER_CLAMP_V) ? true : false},
            {"clampW", (samplerState & SAMPLER_CLAMP_W) ? true : false},
        };
    }

    json BuildTextureTableJson(const MaterialTextureDef* textureTable, const size_t count)
    {
        static const char* semanticNames[]{
            "2d", "function", "colorMap", "unused1", "unused2", "normalMap", "unused3", "unused4", "specularMap", "unused5", "unused6", "waterMap"};

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

                const auto knownMaterialSourceName = knownMaterialSourceNames.find(entry.nameHash);
                if (knownMaterialSourceName != knownMaterialSourceNames.end())
                {
                    jEntry["name"] = knownMaterialSourceName->second;
                }
                else
                {
                    jEntry.merge_patch({
                        {"nameHash",  entry.nameHash },
                        {"nameStart", entry.nameStart},
                        {"nameEnd",   entry.nameEnd  },
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
                        const auto knownMaterialSourceName = knownMaterialSourceNames.find(entry.nameHash);
                        if (knownMaterialSourceName != knownMaterialSourceNames.end())
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
        static const char* blendNames[]{
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
        static const char* blendOpNames[]{"disabled", "add", "subtract", "revSubtract", "min", "max"};
        static const char* depthTestNames[]{
            "always",
            "less",
            "equal",
            "lessEqual",
        };
        static const char* polygonOffsetNames[]{
            "0",
            "1",
            "2",
            "shadowMap",
        };
        static const char* stencilOpNames[]{"keep", "zero", "replace", "incrSat", "decrSat", "invert", "incr", "decr"};

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
                if (entry.loadBits[0] & GFXS0_ATEST_GT_0)
                    alphaTest = "gt0";
                else if (entry.loadBits[0] & GFXS0_ATEST_LT_128)
                    alphaTest = "lt128";
                else if (entry.loadBits[0] & GFXS0_ATEST_GE_128)
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

    json BuildSurfaceTypeBitsJson(const unsigned surfaceTypeBits)
    {
        if (!surfaceTypeBits)
            return json(surfaceTypeNames[SURF_TYPE_DEFAULT]);

        static constexpr auto NON_SURFACE_TYPE_BITS =
            ~(std::numeric_limits<unsigned>::max() >> ((sizeof(unsigned) * 8) - (static_cast<unsigned>(SURF_TYPE_NUM) - 1)));
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
            return json(surfaceTypeNames[SURF_TYPE_DEFAULT]);

        return json(ss.str());
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
} // namespace IW3

bool AssetDumperMaterial::ShouldDump(XAssetInfo<Material>* asset)
{
    return true;
}

void AssetDumperMaterial::DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset)
{
    auto* material = asset->Asset();

    std::ostringstream ss;
    ss << "materials/" << asset->m_name << ".json";
    const auto assetFile = context.OpenAssetFile(ss.str());

    if (!assetFile)
        return;

    auto& stream = *assetFile;

    static const char* cameraRegionNames[]{"lit", "decal", "emissive", "none"};
    static std::unordered_map<size_t, std::string> sortKeyNames{
        {0,  "distortion"             },
        {1,  "opaque water"           },
        {2,  "boat hull"              },
        {3,  "opaque ambient"         },
        {4,  "opaque"                 },
        {5,  "sky"                    },
        {6,  "skybox - sun / moon"    },
        {7,  "skybox - clouds"        },
        {8,  "skybox - horizon"       },
        {9,  "decal - bottom 1"       },
        {10, "decal - bottom 2"       },
        {11, "decal - bottom 3"       },
        {12, "decal - static decal"   },
        {13, "decal - middle 1"       },
        {14, "decal - middle 2"       },
        {15, "decal - middle 3"       },
        {24, "decal - weapon impact"  },
        {29, "decal - top 1"          },
        {30, "decal - top 2"          },
        {31, "decal - top 3"          },
        {32, "multiplicative"         },
        {33, "banner / curtain"       },
        {34, "hair"                   },
        {35, "underwater"             },
        {36, "transparent water"      },
        {37, "corona"                 },
        {38, "window inside"          },
        {39, "window outside"         },
        {40, "before effects - bottom"},
        {41, "before effects - middle"},
        {42, "before effects - top"   },
        {43, "blend / additive"       },
        {48, "effect - auto sort"     },
        {56, "after effects - bottom" },
        {57, "after effects - middle" },
        {58, "after effects - top"    },
        {59, "viewmodel effect"       },
    };

    const auto foundSortKeyName = sortKeyNames.find(material->info.sortKey);
    assert(foundSortKeyName != sortKeyNames.end());

    const json j = {
        {"info",
         {
#if defined(FLAGS_DEBUG) && FLAGS_DEBUG == 1
             {"gameFlags", BuildCharFlagsJson("gameFlag", material->info.gameFlags)}, // TODO: Find out what gameflags mean
#else
             {"gameFlags", material->info.gameFlags}, // TODO: Find out what gameflags mean
#endif
             {"sortKey", foundSortKeyName != sortKeyNames.end() ? foundSortKeyName->second : std::to_string(material->info.sortKey)},
             {"textureAtlasRowCount", material->info.textureAtlasRowCount},
             {"textureAtlasColumnCount", material->info.textureAtlasColumnCount},
             {"drawSurf",
              {{"objectId", static_cast<unsigned>(material->info.drawSurf.fields.objectId)},
               {"reflectionProbeIndex", static_cast<unsigned>(material->info.drawSurf.fields.reflectionProbeIndex)},
               {"customIndex", static_cast<unsigned>(material->info.drawSurf.fields.customIndex)},
               {"materialSortedIndex", static_cast<unsigned>(material->info.drawSurf.fields.materialSortedIndex)},
               {"prepass", static_cast<unsigned>(material->info.drawSurf.fields.prepass)},
               {"useHeroLighting", static_cast<unsigned>(material->info.drawSurf.fields.primaryLightIndex)},
               {"surfType", static_cast<unsigned>(material->info.drawSurf.fields.surfType)},
               {"primarySortKey", static_cast<unsigned>(material->info.drawSurf.fields.primarySortKey)}}},
             {"surfaceTypeBits", BuildSurfaceTypeBitsJson(material->info.surfaceTypeBits)},
             {"hashIndex", material->info.hashIndex}}},
        {"stateBitsEntry", std::vector(std::begin(material->stateBitsEntry), std::end(material->stateBitsEntry))},
#if defined(FLAGS_DEBUG) && FLAGS_DEBUG == 1
        {"stateFlags", BuildCharFlagsJson("stateFlag", material->stateFlags)},
#else
        {"stateFlags", material->stateFlags},
#endif
        {"cameraRegion", ArrayEntry(cameraRegionNames, material->cameraRegion)},
        {"techniqueSet", material->techniqueSet && material->techniqueSet->name ? AssetName(material->techniqueSet->name) : nullptr},
        {"textureTable", BuildTextureTableJson(material->textureTable, material->textureCount)},
        {"constantTable", BuildConstantTableJson(material->constantTable, material->constantCount)},
        {"stateBitsTable", BuildStateBitsTableJson(material->stateBitsTable, material->stateBitsCount)}
    };

    stream << std::setw(4) << j;
}
