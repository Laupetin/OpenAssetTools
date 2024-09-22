#include "JsonMaterialLoader.h"

#include "Game/IW5/CommonIW5.h"
#include "Game/IW5/Material/JsonMaterial.h"
#include "Impl/Base64.h"

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace IW5;

namespace
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
            std::string game;
            std::string type;
            unsigned version;

            jRoot.at("_type").get_to(type);
            jRoot.at("_game").get_to(game);
            jRoot.at("_version").get_to(version);

            if (type != "material" || version != 1u || game != "iw5")
            {
                std::cerr << std::format("Tried to load material \"{}\" but did not find expected type material of version 1\n", material.info.name);
                return false;
            }

            try
            {
                const auto jMaterial = jRoot.get<JsonMaterial>();
                return CreateMaterialFromJson(jMaterial, material);
            }
            catch (const json::exception& e)
            {
                std::cerr << std::format("Failed to parse json of material: {}\n", e.what());
            }

            return false;
        }

    private:
        static void PrintError(const Material& material, const std::string& message)
        {
            std::cerr << std::format("Cannot load material \"{}\": {}\n", material.info.name, message);
        }

        static bool CreateGameFlagsFromJson(const JsonMaterial& jMaterial, unsigned char& gameFlags)
        {
            for (const auto gameFlag : jMaterial.gameFlags)
                gameFlags |= gameFlag;

            return true;
        }

        static void CreateSamplerStateFromJson(const JsonSamplerState& jSamplerState, MaterialTextureDefSamplerState& samplerState)
        {
            samplerState.filter = jSamplerState.filter;
            samplerState.mipMap = jSamplerState.mipMap;
            samplerState.clampU = jSamplerState.clampU;
            samplerState.clampV = jSamplerState.clampV;
            samplerState.clampW = jSamplerState.clampW;
        }

        bool CreateWaterFromJson(const JsonWater& jWater, water_t& water, const Material& material) const
        {
            water.writable.floatTime = jWater.floatTime;
            water.M = jWater.m;
            water.N = jWater.n;
            water.Lx = jWater.lx;
            water.Lz = jWater.lz;
            water.gravity = jWater.gravity;
            water.windvel = jWater.windvel;
            water.winddir[0] = jWater.winddir[0];
            water.winddir[1] = jWater.winddir[1];
            water.amplitude = jWater.amplitude;
            water.codeConstant[0] = jWater.codeConstant[0];
            water.codeConstant[1] = jWater.codeConstant[1];
            water.codeConstant[2] = jWater.codeConstant[2];
            water.codeConstant[3] = jWater.codeConstant[3];

            const auto expectedH0Size = water.M * water.N * sizeof(complex_s);
            if (expectedH0Size > 0)
            {
                water.H0 = m_memory.Alloc<complex_s>(water.M * water.N);
                const auto h0Size = base64::DecodeBase64(jWater.h0.data(), jWater.h0.size(), water.H0, expectedH0Size);
                if (h0Size != expectedH0Size)
                {
                    PrintError(material, std::format("Water h0 size {} does not match expected {}", h0Size, expectedH0Size));
                    return false;
                }
            }

            const auto expectedWTermSize = water.M * water.N * sizeof(float);
            if (expectedWTermSize > 0)
            {
                water.wTerm = m_memory.Alloc<float>(water.M * water.N);
                auto wTermSize = base64::DecodeBase64(jWater.wTerm.data(), jWater.wTerm.size(), water.wTerm, expectedWTermSize);
                if (wTermSize != expectedWTermSize)
                {
                    PrintError(material, std::format("Water wTerm size {} does not match expected {}", wTermSize, expectedWTermSize));
                    return false;
                }
            }

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

            CreateSamplerStateFromJson(jTexture.samplerState, textureDef.samplerState);

            textureDef.semantic = jTexture.semantic;

            auto* imageAsset = m_manager.LoadDependency<AssetImage>(jTexture.image);
            if (!imageAsset)
            {
                PrintError(material, std::format("Could not find textureDef image: {}", jTexture.image));
                return false;
            }
            m_dependencies.push_back(imageAsset);

            if (jTexture.water)
            {
                if (jTexture.semantic != TS_WATER_MAP)
                {
                    PrintError(material, "Only textureDefs with semantic waterMap can define water params");
                    return false;
                }
            }
            else
            {
                if (jTexture.semantic == TS_WATER_MAP)
                {
                    PrintError(material, "TextureDefs with semantic waterMap must define water params");
                    return false;
                }
            }

            if (jTexture.water)
            {
                auto* water = m_memory.Alloc<water_t>();
                water->image = imageAsset->Asset();

                if (!CreateWaterFromJson(*jTexture.water, *water, material))
                    return false;

                textureDef.u.water = water;
            }
            else
                textureDef.u.image = imageAsset->Asset();

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
            else if (jStateBitsTableEntry.alphaTest == JsonAlphaTest::LT128)
            {
                structured.alphaTestDisabled = 0;
                structured.alphaTest = GFXS_ALPHA_TEST_LT_128;
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
                structured.cullFace = GFXS_CULL_NONE;
            else if (jStateBitsTableEntry.cullFace == JsonCullFace::BACK)
                structured.cullFace = GFXS_CULL_BACK;
            else if (jStateBitsTableEntry.cullFace == JsonCullFace::FRONT)
                structured.cullFace = GFXS_CULL_FRONT;
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
            structured.gammaWrite = jStateBitsTableEntry.gammaWrite;
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

            if (jMaterial.stateBitsEntry.size() != std::extent_v<decltype(Material::stateBitsEntry)>)
            {
                PrintError(material, std::format("StateBitsEntry size is not {}", std::extent_v<decltype(Material::stateBitsEntry)>));
                return false;
            }
            for (auto i = 0u; i < std::extent_v<decltype(Material::stateBitsEntry)>; i++)
                material.stateBitsEntry[i] = jMaterial.stateBitsEntry[i];

            material.stateFlags = static_cast<unsigned char>(jMaterial.stateFlags);
            material.cameraRegion = jMaterial.cameraRegion;

            auto* techniqueSet = m_manager.LoadDependency<AssetTechniqueSet>(jMaterial.techniqueSet);
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
                material.textureTable = m_memory.Alloc<MaterialTextureDef>(material.textureCount);

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
                material.constantTable = m_memory.Alloc<MaterialConstantDef>(material.constantCount);

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
                material.stateBitsTable = m_memory.Alloc<GfxStateBits>(material.stateBitsCount);

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

            return true;
        }

        std::istream& m_stream;
        MemoryManager& m_memory;
        IAssetLoadingManager& m_manager;
        std::vector<XAssetInfoGeneric*>& m_dependencies;
    };
} // namespace

namespace IW5
{
    bool LoadMaterialAsJson(
        std::istream& stream, Material& material, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies)
    {
        const JsonLoader loader(stream, *memory, *manager, dependencies);

        return loader.Load(material);
    }
} // namespace IW5
