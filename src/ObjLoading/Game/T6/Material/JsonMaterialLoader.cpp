#include "JsonMaterialLoader.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Material/JsonMaterial.h"

#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace T6
{
    class JsonLoader
    {
    public:
        JsonLoader(std::istream& stream, MemoryManager& memory, IAssetLoadingManager& manager, std::vector<XAssetInfoGeneric*>& dependencies)
            : m_stream(stream),
              m_memory(memory),
              m_manager(manager),
              m_dependencies(dependencies)

        {
        }

        bool Load(Material& material) const
        {
            const auto jRoot = json::parse(m_stream);
            std::string type;
            unsigned version;

            jRoot.at("_type").get_to(type);
            jRoot.at("_version").get_to(version);

            if (type != "material" || version != 1u)
            {
                std::cerr << "Tried to load material \"" << material.info.name << "\" but did not find expected type material of version 1\n";
                return false;
            }

            const auto jMaterial = jRoot.get<JsonMaterial>();
            return CreateMaterialFromJson(jMaterial, material);
        }

    private:
        static void PrintError(const Material& material, const std::string& message)
        {
            std::cerr << "Cannot load material \"" << material.info.name << "\": " << message << "\n";
        }

        static bool CreateGameFlagsFromJson(const JsonMaterial& jMaterial, unsigned& gameFlags)
        {
            for (const auto gameFlag : jMaterial.gameFlags)
                gameFlags |= gameFlag;

            return true;
        }

        bool CreateTextureDefFromJson(const JsonTexture& jTexture, MaterialTextureDef& textureDef, const Material& material) const
        {
            if (jTexture.name)
            {
                if (jTexture.name->empty())
                {
                    PrintError(material, "textureDef name cannot be empty");
                    return false;
                }

                textureDef.nameStart = jTexture.name.value()[0];
                textureDef.nameEnd = jTexture.name.value()[jTexture.name->size() - 1];
                textureDef.nameHash = Common::R_HashString(jTexture.name.value().c_str(), 0);
            }
            else
            {
                if (!jTexture.nameStart || !jTexture.nameEnd || !jTexture.nameHash)
                {
                    PrintError(material, "textureDefs without name must have nameStart, nameEnd and nameHash");
                    return false;
                }

                if (jTexture.nameStart->size() != 1 || jTexture.nameEnd->size() != 1)
                {
                    PrintError(material, "nameStart and nameEnd must be a string of exactly one character");
                    return false;
                }

                textureDef.nameStart = jTexture.nameStart.value()[0];
                textureDef.nameEnd = jTexture.nameEnd.value()[0];
                textureDef.nameHash = jTexture.nameHash.value();
            }

            textureDef.samplerState.filter = jTexture.samplerState.filter;
            textureDef.samplerState.mipMap = jTexture.samplerState.mipMap;
            textureDef.samplerState.clampU = jTexture.samplerState.clampU;
            textureDef.samplerState.clampV = jTexture.samplerState.clampV;
            textureDef.samplerState.clampW = jTexture.samplerState.clampW;

            textureDef.semantic = jTexture.semantic;
            textureDef.isMatureContent = jTexture.isMatureContent;

            auto* image = static_cast<XAssetInfo<GfxImage>*>(m_manager.LoadDependency(ASSET_TYPE_IMAGE, jTexture.image));
            if (!image)
            {
                PrintError(material, std::format("Could not find textureDef image: {}", jTexture.image));
                return false;
            }
            m_dependencies.push_back(image);
            textureDef.image = image->Asset();

            return true;
        }

        static bool CreateConstantDefFromJson(const JsonConstant& jConstant, MaterialConstantDef& constantDef, const Material& material)
        {
            if (jConstant.name)
            {
                const auto copyCount = std::min(jConstant.name->size() + 1, std::extent_v<decltype(MaterialConstantDef::name)>);
                strncpy(constantDef.name, jConstant.name->c_str(), copyCount);
                if (copyCount < std::extent_v<decltype(MaterialConstantDef::name)>)
                    memset(&constantDef.name[copyCount], 0, std::extent_v<decltype(MaterialConstantDef::name)> - copyCount);
                constantDef.nameHash = Common::R_HashString(jConstant.name->c_str(), 0);
            }
            else
            {
                if (!jConstant.nameFragment || !jConstant.nameHash)
                {
                    PrintError(material, "constantDefs without name must have nameFragment and nameHash");
                    return false;
                }

                const auto copyCount = std::min(jConstant.nameFragment->size() + 1, std::extent_v<decltype(MaterialConstantDef::name)>);
                strncpy(constantDef.name, jConstant.nameFragment->c_str(), copyCount);
                if (copyCount < std::extent_v<decltype(MaterialConstantDef::name)>)
                    memset(&constantDef.name[copyCount], 0, std::extent_v<decltype(MaterialConstantDef::name)> - copyCount);
                constantDef.nameHash = jConstant.nameHash.value();
            }

            if (jConstant.literal.size() != 4)
            {
                PrintError(material, "constantDef literal must be array of size 4");
                return false;
            }

            constantDef.literal.x = jConstant.literal[0];
            constantDef.literal.y = jConstant.literal[1];
            constantDef.literal.z = jConstant.literal[2];
            constantDef.literal.w = jConstant.literal[3];

            return true;
        }

        static bool
            CreateStateBitsTableEntryFromJson(const JsonStateBitsTableEntry& jStateBitsTableEntry, GfxStateBits& stateBitsTableEntry, const Material& material)
        {
            auto& structured = stateBitsTableEntry.loadBits.structured;

            structured.srcBlendRgb = jStateBitsTableEntry.srcBlendRgb;
            structured.dstBlendRgb = jStateBitsTableEntry.dstBlendRgb;
            structured.blendOpRgb = jStateBitsTableEntry.blendOpRgb;

            if (jStateBitsTableEntry.alphaTest == JsonAlphaTest::DISABLED)
            {
                structured.alphaTestDisabled = 1;
                structured.alphaTest = 0;
            }
            else if (jStateBitsTableEntry.alphaTest == JsonAlphaTest::GT0)
            {
                structured.alphaTestDisabled = 0;
                structured.alphaTest = GFXS_ALPHA_TEST_GT_0;
            }
            else if (jStateBitsTableEntry.alphaTest == JsonAlphaTest::GE128)
            {
                structured.alphaTestDisabled = 0;
                structured.alphaTest = GFXS_ALPHA_TEST_GE_128;
            }
            else
            {
                PrintError(material, "Invalid value for alphaTest");
                return false;
            }

            if (jStateBitsTableEntry.cullFace == JsonCullFace::NONE)
                structured.cullFace = GFXS0_CULL_NONE;
            else if (jStateBitsTableEntry.cullFace == JsonCullFace::BACK)
                structured.cullFace = GFXS0_CULL_BACK;
            else if (jStateBitsTableEntry.cullFace == JsonCullFace::FRONT)
                structured.cullFace = GFXS0_CULL_FRONT;
            else
            {
                PrintError(material, "Invalid value for cull face");
                return false;
            }

            structured.srcBlendAlpha = jStateBitsTableEntry.srcBlendAlpha;
            structured.dstBlendAlpha = jStateBitsTableEntry.dstBlendAlpha;
            structured.blendOpAlpha = jStateBitsTableEntry.blendOpAlpha;
            structured.colorWriteRgb = jStateBitsTableEntry.colorWriteRgb;
            structured.colorWriteAlpha = jStateBitsTableEntry.colorWriteAlpha;
            structured.polymodeLine = jStateBitsTableEntry.polymodeLine;
            structured.depthWrite = jStateBitsTableEntry.depthWrite;

            if (jStateBitsTableEntry.depthTest == JsonDepthTest::DISABLED)
                structured.depthTestDisabled = 1;
            else if (jStateBitsTableEntry.depthTest == JsonDepthTest::ALWAYS)
                structured.depthTest = GFXS_DEPTHTEST_ALWAYS;
            else if (jStateBitsTableEntry.depthTest == JsonDepthTest::LESS)
                structured.depthTest = GFXS_DEPTHTEST_LESS;
            else if (jStateBitsTableEntry.depthTest == JsonDepthTest::EQUAL)
                structured.depthTest = GFXS_DEPTHTEST_EQUAL;
            else if (jStateBitsTableEntry.depthTest == JsonDepthTest::LESS_EQUAL)
                structured.depthTest = GFXS_DEPTHTEST_LESSEQUAL;
            else
            {
                PrintError(material, "Invalid value for depth test");
                return false;
            }

            structured.polygonOffset = jStateBitsTableEntry.polygonOffset;

            if (jStateBitsTableEntry.stencilFront)
            {
                structured.stencilFrontEnabled = 1;
                structured.stencilFrontPass = jStateBitsTableEntry.stencilFront->pass;
                structured.stencilFrontFail = jStateBitsTableEntry.stencilFront->fail;
                structured.stencilFrontZFail = jStateBitsTableEntry.stencilFront->zfail;
                structured.stencilFrontFunc = jStateBitsTableEntry.stencilFront->func;
            }

            if (jStateBitsTableEntry.stencilBack)
            {
                structured.stencilBackEnabled = 1;
                structured.stencilBackPass = jStateBitsTableEntry.stencilBack->pass;
                structured.stencilBackFail = jStateBitsTableEntry.stencilBack->fail;
                structured.stencilBackZFail = jStateBitsTableEntry.stencilBack->zfail;
                structured.stencilBackFunc = jStateBitsTableEntry.stencilBack->func;
            }

            return true;
        }

        bool CreateMaterialFromJson(const JsonMaterial& jMaterial, Material& material) const
        {
            if (!CreateGameFlagsFromJson(jMaterial, material.info.gameFlags))
                return false;

            material.info.sortKey = static_cast<unsigned char>(jMaterial.sortKey);

            if (jMaterial.textureAtlas)
            {
                material.info.textureAtlasRowCount = jMaterial.textureAtlas->rows;
                material.info.textureAtlasColumnCount = jMaterial.textureAtlas->columns;
            }
            else
            {
                material.info.textureAtlasRowCount = 0;
                material.info.textureAtlasColumnCount = 0;
            }

            material.info.surfaceTypeBits = jMaterial.surfaceTypeBits;
            material.info.layeredSurfaceTypes = jMaterial.layeredSurfaceTypes;
            material.info.hashIndex = static_cast<uint16_t>(jMaterial.hashIndex);
            material.info.surfaceFlags = jMaterial.surfaceFlags;
            material.info.contents = jMaterial.contents;

            if (jMaterial.stateBitsEntry.size() != std::extent_v<decltype(Material::stateBitsEntry)>)
            {
                PrintError(material, std::format("StateBitsEntry size is not {}", jMaterial.stateBitsEntry.size()));
                return false;
            }
            for (auto i = 0u; i < std::extent_v<decltype(Material::stateBitsEntry)>; i++)
                material.stateBitsEntry[i] = jMaterial.stateBitsEntry[i];

            material.stateFlags = static_cast<unsigned char>(jMaterial.stateFlags);
            material.cameraRegion = jMaterial.cameraRegion;
            material.probeMipBits = jMaterial.probeMipBits;

            auto* techniqueSet = static_cast<XAssetInfo<MaterialTechniqueSet>*>(m_manager.LoadDependency(ASSET_TYPE_TECHNIQUE_SET, jMaterial.techniqueSet));
            if (!techniqueSet)
            {
                PrintError(material, "Could not find technique set");
                return false;
            }
            m_dependencies.push_back(techniqueSet);
            material.techniqueSet = techniqueSet->Asset();

            if (!jMaterial.textures.empty())
            {
                material.textureCount = static_cast<unsigned char>(jMaterial.textures.size());
                material.textureTable = static_cast<MaterialTextureDef*>(m_memory.Alloc(sizeof(MaterialTextureDef) * material.textureCount));
                memset(material.textureTable, 0, sizeof(MaterialTextureDef) * material.textureCount);

                for (auto i = 0u; i < material.textureCount; i++)
                {
                    if (!CreateTextureDefFromJson(jMaterial.textures[i], material.textureTable[i], material))
                        return false;
                }
            }
            else
            {
                material.textureCount = 0;
                material.textureTable = nullptr;
            }

            if (!jMaterial.constants.empty())
            {
                material.constantCount = static_cast<unsigned char>(jMaterial.constants.size());
                material.constantTable = static_cast<MaterialConstantDef*>(m_memory.Alloc(sizeof(MaterialConstantDef) * material.constantCount));
                memset(material.constantTable, 0, sizeof(MaterialConstantDef) * material.constantCount);

                for (auto i = 0u; i < material.constantCount; i++)
                {
                    if (!CreateConstantDefFromJson(jMaterial.constants[i], material.constantTable[i], material))
                        return false;
                }
            }
            else
            {
                material.constantCount = 0;
                material.constantTable = nullptr;
            }

            if (!jMaterial.stateBits.empty())
            {
                material.stateBitsCount = static_cast<unsigned char>(jMaterial.stateBits.size());
                material.stateBitsTable = static_cast<GfxStateBitsTable*>(m_memory.Alloc(sizeof(GfxStateBitsTable) * material.stateBitsCount));
                memset(material.stateBitsTable, 0, sizeof(GfxStateBitsTable) * material.stateBitsCount);

                for (auto i = 0u; i < material.stateBitsCount; i++)
                {
                    if (!CreateStateBitsTableEntryFromJson(jMaterial.stateBits[i], material.stateBitsTable[i], material))
                        return false;
                }
            }
            else
            {
                material.stateBitsCount = 0;
                material.stateBitsTable = nullptr;
            }

            if (jMaterial.thermalMaterial)
            {
                auto* thermalMaterial = static_cast<XAssetInfo<Material>*>(m_manager.LoadDependency(ASSET_TYPE_MATERIAL, jMaterial.thermalMaterial.value()));
                if (!thermalMaterial)
                {
                    PrintError(material, "Could not find thermal material");
                    return false;
                }
                m_dependencies.push_back(thermalMaterial);
                material.thermalMaterial = thermalMaterial->Asset();
            }
            else
            {
                material.thermalMaterial = nullptr;
            }

            return true;
        }

        std::istream& m_stream;
        MemoryManager& m_memory;
        IAssetLoadingManager& m_manager;
        std::vector<XAssetInfoGeneric*>& m_dependencies;
    };

    bool LoadMaterialAsJson(
        std::istream& stream, Material& material, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies)
    {
        const JsonLoader loader(stream, *memory, *manager, dependencies);

        return loader.Load(material);
    }
} // namespace T6
