#include "AssetDumperMaterial.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/MaterialConstantsT6.h"
#include "Game/T6/TechsetConstantsT6.h"

#include <cassert>
#include <nlohmann/json.hpp>
#include <sstream>

using namespace T6;
using namespace nlohmann;

namespace T6::material
{
#define KNOWN_HASH(strValue)                                                                                                                                   \
    {                                                                                                                                                          \
        Common::R_HashString(strValue, 0), strValue                                                                                                            \
    }

    std::unordered_map<unsigned, std::string> knownHashes{
        KNOWN_HASH("colorMap"),
        KNOWN_HASH("normalMap"),
        KNOWN_HASH("specularMap"),
    };

    class JsonDumper
    {
    public:
        explicit JsonDumper(std::ostream& stream)
            : m_stream(stream)
        {
        }

        void Dump(const Material* material) const
        {
            json jRoot;

            jRoot["_type"] = "material";
            jRoot["_version"] = 1;

            MaterialToJson(jRoot, material);

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        template<size_t SIZE> static void JsonEnumEntry(json& j, const char* key, const unsigned value, const char* (&enumValues)[SIZE])
        {
            static_assert(SIZE > 0);
            JsonEnumEntry(j, key, value, enumValues, SIZE);
        }

        static void JsonEnumEntry(json& j, const char* key, const unsigned value, const char** enumValues, const size_t enumValueCount)
        {
            assert(value < enumValueCount);

            if (value < enumValueCount)
                j[key] = enumValues[value];
            else
                j[key] = nullptr;
        }

        static void MaterialToJson(json& jRoot, const Material* material)
        {
            MaterialInfoToJson(jRoot, material);
            StateBitsEntryToJson(jRoot, material);
            jRoot["stateFlags"] = material->stateFlags;
            JsonEnumEntry(jRoot, "cameraRegion", material->cameraRegion, cameraRegionNames);
            jRoot["probeMipBits"] = material->probeMipBits;
            TechniqueSetToJson(jRoot, material->techniqueSet);
            TextureTableToJson(jRoot, material);
            ConstantTableToJson(jRoot, material);
            StateBitsTableToJson(jRoot, material);
            ThermalMaterialToJson(jRoot, material->thermalMaterial);
        }

        static void MaterialInfoToJson(json& jRoot, const Material* material)
        {
            const auto& info = material->info;

            json jGameFlags = json::array();
            for (auto bitIndex = 0u; bitIndex < sizeof(info.gameFlags) * 8u; bitIndex++)
            {
                if (info.gameFlags & (1 << bitIndex))
                {
                    if (bitIndex >= std::extent_v<decltype(gameFlagNames)>)
                    {
                        std::ostringstream ss;
                        ss << std::hex << (1 << bitIndex);
                        jGameFlags.push_back(ss.str());
                    }
                    else
                        jGameFlags.push_back(gameFlagNames[bitIndex]);
                }
            }
            jRoot["gameFlags"] = std::move(jGameFlags);

            jRoot["sortKey"] = info.sortKey;

            json jTextureAtlas;
            jTextureAtlas["rows"] = info.textureAtlasRowCount;
            jTextureAtlas["columns"] = info.textureAtlasColumnCount;
            jRoot["textureAtlas"] = std::move(jTextureAtlas);

            jRoot["surfaceTypeBits"] = info.surfaceTypeBits;
            jRoot["layeredSurfaceTypes"] = info.layeredSurfaceTypes;
            jRoot["hashIndex"] = info.hashIndex;
            jRoot["surfaceFlags"] = info.surfaceFlags;
            jRoot["contents"] = info.contents;
        }

        static void StateBitsEntryToJson(json& jRoot, const Material* material)
        {
            json jStateBitsEntry = json::array();

            for (const char c : material->stateBitsEntry)
                jStateBitsEntry.push_back(static_cast<int>(c));

            jRoot["stateBitsEntry"] = std::move(jStateBitsEntry);
        }

        static void TechniqueSetToJson(json& jRoot, const MaterialTechniqueSet* techniqueSet)
        {
            if (techniqueSet && techniqueSet->name)
                jRoot["techniqueSet"] = AssetName(techniqueSet->name);
            else
                jRoot["techniqueSet"] = nullptr;
        }

        static void TextureTableToJson(json& jRoot, const Material* material)
        {
            json jTextures = json::array();
            if (material->textureTable)
            {
                for (auto textureIndex = 0u; textureIndex < material->textureCount; textureIndex++)
                {
                    json jTexture;
                    TextureDefToJson(jTexture, &material->textureTable[textureIndex]);
                    jTextures.emplace_back(std::move(jTexture));
                }
            }
            jRoot["textures"] = std::move(jTextures);
        }

        static void TextureDefToJson(json& jTexture, const MaterialTextureDef* textureDef)
        {
            const auto knownHash = knownHashes.find(textureDef->nameHash);
            if (knownHash == knownHashes.end())
            {
                jTexture["nameHash"] = textureDef->nameHash;
                jTexture["nameStart"] = std::string(1u, textureDef->nameStart);
                jTexture["nameEnd"] = std::string(1u, textureDef->nameEnd);
            }
            else
            {
                jTexture["name"] = knownHash->second;
            }

            JsonEnumEntry(jTexture, "semantic", textureDef->semantic, textureSemanticNames);
            jTexture["isMatureContent"] = textureDef->isMatureContent;

            json jSamplerState;
            SamplerStateToJson(jSamplerState, textureDef->samplerState);
            jTexture["samplerState"] = std::move(jSamplerState);

            if (textureDef->image && textureDef->image->name)
                jTexture["image"] = AssetName(textureDef->image->name);
            else
                jTexture["image"] = nullptr;
        }

        static void SamplerStateToJson(json& jSamplerState, const MaterialTextureDefSamplerState& samplerState)
        {
            JsonEnumEntry(jSamplerState, "filter", samplerState.filter, textureFilterNames);
            JsonEnumEntry(jSamplerState, "mipMap", samplerState.mipMap, samplerStateMipMapNames);
            jSamplerState["clampU"] = samplerState.clampU ? true : false;
            jSamplerState["clampV"] = samplerState.clampV ? true : false;
            jSamplerState["clampW"] = samplerState.clampW ? true : false;
        }

        static void ConstantTableToJson(json& jRoot, const Material* material)
        {
            json jConstants = json::array();
            if (material->constantTable)
            {
                for (auto constantIndex = 0u; constantIndex < material->constantCount; constantIndex++)
                {
                    json jConstant;
                    ConstantDefToJson(jConstant, &material->constantTable[constantIndex]);
                    jConstants.emplace_back(std::move(jConstant));
                }
            }
            jRoot["constants"] = std::move(jConstants);
        }

        static void ConstantDefToJson(json& jConstant, const MaterialConstantDef* textureDef)
        {
            const auto fragmentLength = strnlen(textureDef->name, std::extent_v<decltype(MaterialConstantDef::name)>);
            const std::string nameFragment(textureDef->name, fragmentLength);

            if (fragmentLength < std::extent_v<decltype(MaterialConstantDef::name)> || Common::R_HashString(nameFragment.c_str(), 0) == textureDef->nameHash)
            {
                jConstant["name"] = nameFragment;
            }
            else
            {
                jConstant["nameHash"] = textureDef->nameHash;
                jConstant["nameFragment"] = nameFragment;
            }

            json jLiteral;
            jLiteral.push_back(textureDef->literal.v[0]);
            jLiteral.push_back(textureDef->literal.v[1]);
            jLiteral.push_back(textureDef->literal.v[2]);
            jLiteral.push_back(textureDef->literal.v[3]);
            jConstant["literal"] = std::move(jLiteral);
        }

        static void StateBitsTableToJson(json& jRoot, const Material* material)
        {
            json jStateBits = json::array();
            if (material->stateBitsTable)
            {
                for (auto stateBitsEntryIndex = 0u; stateBitsEntryIndex < material->stateBitsCount; stateBitsEntryIndex++)
                {
                    json jStateBitsEntry;
                    StateBitsEntryToJson(jStateBitsEntry, &material->stateBitsTable[stateBitsEntryIndex].loadBits.structured);
                    jStateBits.emplace_back(std::move(jStateBitsEntry));
                }
            }
            jRoot["stateBits"] = std::move(jStateBits);
        }

        static void StateBitsEntryToJson(json& jStateBitsEntry, const GfxStateBitsLoadBitsStructured* structured)
        {
            JsonEnumEntry(jStateBitsEntry, "srcBlendRgb", std::min(structured->srcBlendRgb, std::extent_v<decltype(blendNames)> - 1u), blendNames);
            JsonEnumEntry(jStateBitsEntry, "dstBlendRgb", std::min(structured->dstBlendRgb, std::extent_v<decltype(blendNames)> - 1u), blendNames);
            JsonEnumEntry(jStateBitsEntry, "blendOpRgb", std::min(structured->blendOpRgb, std::extent_v<decltype(blendOpNames)> - 1u), blendOpNames);

            assert(structured->alphaTestDisabled || structured->alphaTest == GFXS_ALPHA_TEST_GT_0 || structured->alphaTest == GFXS_ALPHA_TEST_GE_128);
            if (structured->alphaTestDisabled)
                jStateBitsEntry["alphaTest"] = "disabled";
            else if (structured->alphaTest == GFXS_ALPHA_TEST_GT_0)
                jStateBitsEntry["alphaTest"] = "gt0";
            else if (structured->alphaTest == GFXS_ALPHA_TEST_GE_128)
                jStateBitsEntry["alphaTest"] = "ge128";

            assert(structured->cullFace == GFXS0_CULL_NONE || structured->cullFace == GFXS0_CULL_BACK || structured->cullFace == GFXS0_CULL_FRONT);
            if (structured->cullFace == GFXS0_CULL_NONE)
                jStateBitsEntry["cullFace"] = "none";
            else if (structured->cullFace == GFXS0_CULL_BACK)
                jStateBitsEntry["cullFace"] = "back";
            else if (structured->cullFace == GFXS0_CULL_FRONT)
                jStateBitsEntry["cullFace"] = "front";

            JsonEnumEntry(jStateBitsEntry, "srcBlendRgb", std::min(structured->srcBlendAlpha, std::extent_v<decltype(blendNames)> - 1u), blendNames);
            JsonEnumEntry(jStateBitsEntry, "dstBlendRgb", std::min(structured->dstBlendAlpha, std::extent_v<decltype(blendNames)> - 1u), blendNames);
            JsonEnumEntry(jStateBitsEntry, "blendOpRgb", std::min(structured->blendOpAlpha, std::extent_v<decltype(blendOpNames)> - 1u), blendOpNames);
            jStateBitsEntry["colorWriteRgb"] = static_cast<bool>(structured->colorWriteRgb);
            jStateBitsEntry["colorWriteAlpha"] = static_cast<bool>(structured->colorWriteAlpha);
            jStateBitsEntry["polymodeLine"] = static_cast<bool>(structured->polymodeLine);
            jStateBitsEntry["depthWrite"] = static_cast<bool>(structured->depthWrite);

            assert(structured->depthTestDisabled || structured->depthTest == GFXS_DEPTHTEST_ALWAYS || structured->depthTest == GFXS_DEPTHTEST_LESS
                   || structured->depthTest == GFXS_DEPTHTEST_EQUAL || structured->depthTest == GFXS_DEPTHTEST_LESSEQUAL);
            if (structured->depthTestDisabled)
                jStateBitsEntry["depthTest"] = "disabled";
            else if (structured->depthTest == GFXS_DEPTHTEST_ALWAYS)
                jStateBitsEntry["depthTest"] = "always";
            else if (structured->depthTest == GFXS_DEPTHTEST_LESS)
                jStateBitsEntry["depthTest"] = "less";
            else if (structured->depthTest == GFXS_DEPTHTEST_EQUAL)
                jStateBitsEntry["depthTest"] = "equal";
            else if (structured->depthTest == GFXS_DEPTHTEST_LESSEQUAL)
                jStateBitsEntry["depthTest"] = "lessequal";

            JsonEnumEntry(
                jStateBitsEntry, "polygonOffset", std::min(structured->polygonOffset, std::extent_v<decltype(polygonOffsetNames)> - 1u), polygonOffsetNames);

            if (structured->stencilFrontEnabled)
            {
                json jStencilFront;
                StencilToJson(
                    jStencilFront, structured->stencilFrontPass, structured->stencilFrontFail, structured->stencilFrontZFail, structured->stencilFrontFunc);
                jStateBitsEntry["stencilFront"] = std::move(jStencilFront);
            }
            else
                jStateBitsEntry["stencilFront"] = json();

            if (structured->stencilBackEnabled)
            {
                json jStencilBack;
                StencilToJson(
                    jStencilBack, structured->stencilBackPass, structured->stencilBackFail, structured->stencilBackZFail, structured->stencilBackFunc);
                jStateBitsEntry["stencilBack"] = std::move(jStencilBack);
            }
            else
                jStateBitsEntry["stencilBack"] = json();
        }

        static void StencilToJson(json& jStencil, const unsigned pass, const unsigned fail, const unsigned zFail, const unsigned func)
        {
            JsonEnumEntry(jStencil, "pass", std::min(pass, std::extent_v<decltype(stencilOpNames)> - 1u), stencilOpNames);
            JsonEnumEntry(jStencil, "fail", std::min(fail, std::extent_v<decltype(stencilOpNames)> - 1u), stencilOpNames);
            JsonEnumEntry(jStencil, "zfail", std::min(zFail, std::extent_v<decltype(stencilOpNames)> - 1u), stencilOpNames);
            JsonEnumEntry(jStencil, "func", std::min(func, std::extent_v<decltype(stencilFuncNames)> - 1u), stencilFuncNames);
        }

        static void ThermalMaterialToJson(json& jRoot, const Material* thermalMaterial)
        {
            if (thermalMaterial && thermalMaterial->info.name)
                jRoot["thermalMaterial"] = AssetName(thermalMaterial->info.name);
            else
                jRoot["thermalMaterial"] = nullptr;
        }

        static const char* AssetName(const char* input)
        {
            if (input && input[0] == ',')
                return &input[1];

            return input;
        }

        std::ostream& m_stream;
    };
} // namespace T6::material

std::string AssetDumperMaterial::GetFileNameForAsset(const std::string& assetName)
{
    std::string sanitizedFileName(assetName);
    if (sanitizedFileName[0] == '*')
    {
        std::replace(sanitizedFileName.begin(), sanitizedFileName.end(), '*', '_');
        const auto parenthesisPos = sanitizedFileName.find('(');
        if (parenthesisPos != std::string::npos)
            sanitizedFileName.erase(parenthesisPos);
        sanitizedFileName = "generated/" + sanitizedFileName;
    }

    std::ostringstream ss;
    ss << "materials/" << sanitizedFileName << ".json";

    return ss.str();
}

bool AssetDumperMaterial::ShouldDump(XAssetInfo<Material>* asset)
{
    return true;
}

void AssetDumperMaterial::DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset)
{
    const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset->m_name));

    if (!assetFile)
        return;

    const material::JsonDumper dumper(*assetFile);
    dumper.Dump(asset->Asset());
}
