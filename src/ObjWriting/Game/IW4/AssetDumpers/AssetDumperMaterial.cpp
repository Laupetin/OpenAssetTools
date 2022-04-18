#include "AssetDumperMaterial.h"

#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

#include "Game/IW4/TechsetConstantsIW4.h"

using namespace IW4;
using json = nlohmann::json;

namespace IW4
{
    const char* AssetName(const char* name)
    {
        if (name && name[0] == ',')
            return &name[1];
        return name;
    }

    json BuildComplexTableJson(complex_s* complexTable, const size_t count)
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
            {"wTerm", water->wTerm ? std::vector(water->wTerm, water->wTerm + (water->M * water->N)) : json::array()},
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

    json BuildTextureTableJson(MaterialTextureDef* textureTable, const size_t count)
    {
        auto jArray = json::array();

        if (textureTable)
        {
            for (auto index = 0u; index < count; index++)
            {
                const auto& entry = textureTable[index];
                json jEntry = {
                    {"samplerState", entry.samplerState},
                    {"semantic", entry.semantic}
                };

                const auto knownMaterialSourceName = knownMaterialSourceNames.find(entry.nameHash);
                if (knownMaterialSourceName != knownMaterialSourceNames.end())
                {
                    jEntry["name"] = knownMaterialSourceName->second;
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

    json BuildConstantTableJson(MaterialConstantDef* constantTable, const size_t count)
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

    json BuildStateBitsTableJson(GfxStateBits* stateBitsTable, const size_t count)
    {
        auto jArray = json::array();

        if (stateBitsTable)
        {
            for (auto index = 0u; index < count; index++)
            {
                const auto& entry = stateBitsTable[count];
                jArray.emplace_back(json::array({
                    entry.loadBits[0],
                    entry.loadBits[1]
                }));
            }
        }

        return jArray;
    }
}

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

    const json j = {
        {
            "info", {
                {"gameFlags", material->info.gameFlags},
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
                {"surfaceTypeBits", material->info.surfaceTypeBits},
                {"hashIndex", material->info.hashIndex}
            }
        },
        {"stateBitsEntry", std::vector(std::begin(material->stateBitsEntry), std::end(material->stateBitsEntry))},
        {"stateFlags", material->stateFlags},
        {"cameraRegion", material->cameraRegion},
        {"techniqueSet", material->techniqueSet && material->techniqueSet->name ? AssetName(material->techniqueSet->name) : nullptr},
        {"textureTable", BuildTextureTableJson(material->textureTable, material->textureCount)},
        {"constantTable", BuildConstantTableJson(material->constantTable, material->constantCount)},
        {"stateBitsTable", BuildStateBitsTableJson(material->stateBitsTable, material->stateBitsCount)}
    };

    stream << std::setw(4) << j;
}
