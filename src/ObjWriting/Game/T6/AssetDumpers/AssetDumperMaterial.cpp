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

            m_stream << std::setw(4) << jRoot << std::endl;
        }

    private:
        template<size_t SIZE> static void JsonEnumEntry(json& j, const char* key, unsigned value, const char* (&enumValues)[SIZE])
        {
            static_assert(SIZE > 0);
            JsonEnumEntry(j, key, value, enumValues, SIZE);
        }

        static void JsonEnumEntry(json& j, const char* key, unsigned value, const char** enumValues, size_t enumValueCount)
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
            // TODO
        }

        static void StateBitsEntryToJson(json& jRoot, const Material* material)
        {
            json jStateBitsEntry;

            for (auto i = 0u; i < std::extent_v<decltype(Material::stateBitsEntry)>; i++)
            {
                jStateBitsEntry.push_back(static_cast<int>(material->stateBitsEntry[i]));
            }

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
            json jTextures;
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
            jTexture["nameHash"] = textureDef->nameHash;
            jTexture["nameStart"] = std::string(1u, textureDef->nameStart);
            jTexture["nameEnd"] = std::string(1u, textureDef->nameEnd);
            jTexture["samplerState"] = textureDef->samplerState;
            JsonEnumEntry(jTexture, "semantic", textureDef->semantic, textureSemanticNames);
            jTexture["isMatureContent"] = textureDef->isMatureContent;

            if (textureDef->image && textureDef->image->name)
                jTexture["image"] = AssetName(textureDef->image->name);
            else
                jTexture["image"] = nullptr;
        }

        static void ConstantTableToJson(json& jRoot, const Material* material)
        {
            json jConstants;
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

        static void ConstantDefToJson(json& jConstant, const MaterialConstantDef* textureDef) {}

        static void StateBitsTableToJson(json& jRoot, const Material* material)
        {
            json jStateBits;
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

        static void StateBitsEntryToJson(json& jStateBitsEntry, const GfxStateBitsLoadBitsStructured* structured) {}

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
    std::ostringstream ss;
    ss << "materials/" << assetName << ".json";

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

    material::JsonDumper dumper(*assetFile);
    dumper.Dump(asset->Asset());
}
