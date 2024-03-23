#include "JsonMaterialWriter.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonMaterial.h"
#include "MaterialConstantZoneState.h"

#include <iomanip>
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace T6
{
    class JsonDumper
    {
    public:
        JsonDumper(AssetDumpingContext& context, std::ostream& stream)
            : m_stream(stream),
              m_material_constants(*context.GetZoneAssetDumperState<MaterialConstantZoneState>())
        {
        }

        void Dump(const Material* material) const
        {
            JsonMaterial jsonMaterial;
            CreateJsonMaterial(jsonMaterial, *material);
            json jRoot = jsonMaterial;

            jRoot["_type"] = "material";
            jRoot["_version"] = 1;

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        static const char* AssetName(const char* input)
        {
            if (input && input[0] == ',')
                return &input[1];

            return input;
        }

        static void CreateJsonGameFlags(JsonMaterial& jMaterial, const unsigned gameFlags)
        {
            jMaterial.gameFlags.clear();
            for (auto i = 0u; i < sizeof(gameFlags) * 8u; i++)
            {
                const auto flag = static_cast<MaterialGameFlags>(1 << i);

                if (gameFlags & flag)
                    jMaterial.gameFlags.emplace_back(flag);
            }
        }

        static void CreateJsonSamplerState(JsonSamplerState& jSamplerState, const MaterialTextureDefSamplerState& samplerState)
        {
            jSamplerState.filter = static_cast<TextureFilter>(samplerState.filter);
            jSamplerState.mipMap = static_cast<SamplerStateBitsMipMap_e>(samplerState.mipMap);
            jSamplerState.clampU = samplerState.clampU;
            jSamplerState.clampV = samplerState.clampV;
            jSamplerState.clampW = samplerState.clampW;
        }

        void CreateJsonTexture(JsonTexture& jTextureDef, const MaterialTextureDef& textureDef) const
        {
            std::string textureDefName;
            if (m_material_constants.GetTextureDefName(textureDef.nameHash, textureDefName))
            {
                jTextureDef.name = textureDefName;
            }
            else
            {
                jTextureDef.nameHash = textureDef.nameHash;
                jTextureDef.nameStart = std::string(1u, textureDef.nameStart);
                jTextureDef.nameEnd = std::string(1u, textureDef.nameEnd);
            }

            jTextureDef.semantic = static_cast<TextureSemantic>(textureDef.semantic);
            jTextureDef.isMatureContent = textureDef.isMatureContent;

            CreateJsonSamplerState(jTextureDef.samplerState, textureDef.samplerState);

            if (textureDef.image && textureDef.image->name)
                jTextureDef.image = AssetName(textureDef.image->name);
        }

        void CreateJsonConstant(JsonConstant& jConstantDef, const MaterialConstantDef& constantDef) const
        {
            const auto fragmentLength = strnlen(constantDef.name, std::extent_v<decltype(MaterialConstantDef::name)>);
            const std::string nameFragment(constantDef.name, fragmentLength);
            std::string knownConstantName;

            if (fragmentLength < std::extent_v<decltype(MaterialConstantDef::name)> || Common::R_HashString(nameFragment.c_str(), 0) == constantDef.nameHash)
            {
                jConstantDef.name = nameFragment;
            }
            else if (m_material_constants.GetConstantName(constantDef.nameHash, knownConstantName))
            {
                jConstantDef.name = knownConstantName;
            }
            else
            {
                jConstantDef.nameHash = constantDef.nameHash;
                jConstantDef.nameFragment = nameFragment;
            }

            jConstantDef.literal = std::vector({
                constantDef.literal.x,
                constantDef.literal.y,
                constantDef.literal.z,
                constantDef.literal.w,
            });
        }

        static void CreateJsonStencil(JsonStencil& jStencil, const unsigned pass, const unsigned fail, const unsigned zFail, const unsigned func)
        {
            jStencil.pass = static_cast<GfxStencilOp>(pass);
            jStencil.fail = static_cast<GfxStencilOp>(fail);
            jStencil.zfail = static_cast<GfxStencilOp>(zFail);
            jStencil.func = static_cast<GfxStencilFunc>(func);
        }

        static void CreateJsonStateBitsTableEntry(JsonStateBitsTableEntry& jStateBitsTableEntry, const GfxStateBitsTable& stateBitsTableEntry)
        {
            const auto& structured = stateBitsTableEntry.loadBits.structured;

            jStateBitsTableEntry.srcBlendRgb = static_cast<GfxBlend>(structured.srcBlendRgb);
            jStateBitsTableEntry.dstBlendRgb = static_cast<GfxBlend>(structured.dstBlendRgb);
            jStateBitsTableEntry.blendOpRgb = static_cast<GfxBlendOp>(structured.blendOpRgb);

            assert(structured.alphaTestDisabled || structured.alphaTest == GFXS_ALPHA_TEST_GT_0 || structured.alphaTest == GFXS_ALPHA_TEST_GE_128);
            if (structured.alphaTestDisabled)
                jStateBitsTableEntry.alphaTest = JsonAlphaTest::DISABLED;
            else if (structured.alphaTest == GFXS_ALPHA_TEST_GT_0)
                jStateBitsTableEntry.alphaTest = JsonAlphaTest::GT0;
            else if (structured.alphaTest == GFXS_ALPHA_TEST_GE_128)
                jStateBitsTableEntry.alphaTest = JsonAlphaTest::GE128;
            else
                jStateBitsTableEntry.alphaTest = JsonAlphaTest::INVALID;

            assert(structured.cullFace == GFXS0_CULL_NONE || structured.cullFace == GFXS0_CULL_BACK || structured.cullFace == GFXS0_CULL_FRONT);
            if (structured.cullFace == GFXS0_CULL_NONE)
                jStateBitsTableEntry.cullFace = JsonCullFace::NONE;
            else if (structured.cullFace == GFXS0_CULL_BACK)
                jStateBitsTableEntry.cullFace = JsonCullFace::BACK;
            else if (structured.cullFace == GFXS0_CULL_FRONT)
                jStateBitsTableEntry.cullFace = JsonCullFace::FRONT;
            else
                jStateBitsTableEntry.cullFace = JsonCullFace::INVALID;

            jStateBitsTableEntry.srcBlendAlpha = static_cast<GfxBlend>(structured.srcBlendAlpha);
            jStateBitsTableEntry.dstBlendAlpha = static_cast<GfxBlend>(structured.dstBlendAlpha);
            jStateBitsTableEntry.blendOpAlpha = static_cast<GfxBlendOp>(structured.blendOpAlpha);
            jStateBitsTableEntry.colorWriteRgb = structured.colorWriteRgb;
            jStateBitsTableEntry.colorWriteAlpha = structured.colorWriteAlpha;
            jStateBitsTableEntry.polymodeLine = structured.polymodeLine;
            jStateBitsTableEntry.depthWrite = structured.depthWrite;

            assert(structured.depthTestDisabled || structured.depthTest == GFXS_DEPTHTEST_ALWAYS || structured.depthTest == GFXS_DEPTHTEST_LESS
                   || structured.depthTest == GFXS_DEPTHTEST_EQUAL || structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
            if (structured.depthTestDisabled)
                jStateBitsTableEntry.depthTest = JsonDepthTest::DISABLED;
            else if (structured.depthTest == GFXS_DEPTHTEST_ALWAYS)
                jStateBitsTableEntry.depthTest = JsonDepthTest::ALWAYS;
            else if (structured.depthTest == GFXS_DEPTHTEST_LESS)
                jStateBitsTableEntry.depthTest = JsonDepthTest::LESS;
            else if (structured.depthTest == GFXS_DEPTHTEST_EQUAL)
                jStateBitsTableEntry.depthTest = JsonDepthTest::EQUAL;
            else if (structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL)
                jStateBitsTableEntry.depthTest = JsonDepthTest::LESS_EQUAL;
            else
                jStateBitsTableEntry.depthTest = JsonDepthTest::INVALID;

            jStateBitsTableEntry.polygonOffset = static_cast<GfxPolygonOffset_e>(structured.polygonOffset);

            if (structured.stencilFrontEnabled)
            {
                JsonStencil jStencilFront;
                CreateJsonStencil(
                    jStencilFront, structured.stencilFrontPass, structured.stencilFrontFail, structured.stencilFrontZFail, structured.stencilFrontFunc);
                jStateBitsTableEntry.stencilFront = jStencilFront;
            }

            if (structured.stencilBackEnabled)
            {
                JsonStencil jStencilBack;
                CreateJsonStencil(
                    jStencilBack, structured.stencilBackPass, structured.stencilBackFail, structured.stencilBackZFail, structured.stencilBackFunc);
                jStateBitsTableEntry.stencilBack = jStencilBack;
            }
        }

        void CreateJsonMaterial(JsonMaterial& jMaterial, const Material& material) const
        {
            CreateJsonGameFlags(jMaterial, material.info.gameFlags);
            jMaterial.sortKey = material.info.sortKey;

            jMaterial.textureAtlas = JsonTextureAtlas();
            jMaterial.textureAtlas->rows = material.info.textureAtlasRowCount;
            jMaterial.textureAtlas->columns = material.info.textureAtlasColumnCount;

            jMaterial.surfaceTypeBits = material.info.surfaceTypeBits;
            jMaterial.layeredSurfaceTypes = material.info.layeredSurfaceTypes;
            jMaterial.hashIndex = material.info.hashIndex;
            jMaterial.surfaceFlags = material.info.surfaceFlags;
            jMaterial.contents = material.info.contents;

            jMaterial.stateBitsEntry.resize(std::extent_v<decltype(Material::stateBitsEntry)>);
            for (auto i = 0u; i < std::extent_v<decltype(Material::stateBitsEntry)>; i++)
                jMaterial.stateBitsEntry[i] = material.stateBitsEntry[i];

            jMaterial.stateFlags = material.stateFlags;
            jMaterial.cameraRegion = static_cast<GfxCameraRegionType>(material.cameraRegion);
            jMaterial.probeMipBits = material.probeMipBits;

            if (material.techniqueSet && material.techniqueSet->name)
                jMaterial.techniqueSet = AssetName(material.techniqueSet->name);

            jMaterial.textures.resize(material.textureCount);
            for (auto i = 0u; i < material.textureCount; i++)
                CreateJsonTexture(jMaterial.textures[i], material.textureTable[i]);

            jMaterial.constants.resize(material.constantCount);
            for (auto i = 0u; i < material.constantCount; i++)
                CreateJsonConstant(jMaterial.constants[i], material.constantTable[i]);

            jMaterial.stateBits.resize(material.stateBitsCount);
            for (auto i = 0u; i < material.stateBitsCount; i++)
                CreateJsonStateBitsTableEntry(jMaterial.stateBits[i], material.stateBitsTable[i]);

            if (material.thermalMaterial && material.thermalMaterial->info.name)
                jMaterial.thermalMaterial = AssetName(material.thermalMaterial->info.name);
        }

        std::ostream& m_stream;
        const MaterialConstantZoneState& m_material_constants;
    };

    void DumpMaterialAsJson(std::ostream& stream, const Material* material, AssetDumpingContext& context)
    {
        const JsonDumper dumper(context, stream);
        dumper.Dump(material);
    }
} // namespace T6
