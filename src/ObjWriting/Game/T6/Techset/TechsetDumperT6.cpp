#include "TechsetDumperT6.h"

#include "Shader/ShaderCommon.h"

#include <sstream>
#include <unordered_set>

using namespace T6;

namespace
{
    class ShaderZoneState final : public IZoneAssetDumperState
    {
    public:
        bool ShouldDumpTechnique(const MaterialTechnique* technique)
        {
            const auto existingTechnique = m_dumped_techniques.find(technique);
            if (existingTechnique == m_dumped_techniques.end())
            {
                m_dumped_techniques.emplace(technique);
                return true;
            }

            return false;
        }

        bool ShouldDumpPixelShader(const MaterialPixelShader* pixelShader)
        {
            const auto existingPixelShader = m_dumped_pixel_shaders.find(pixelShader);
            if (existingPixelShader == m_dumped_pixel_shaders.end())
            {
                m_dumped_pixel_shaders.emplace(pixelShader);
                return true;
            }

            return false;
        }

        bool ShouldDumpVertexShader(const MaterialVertexShader* vertexShader)
        {
            const auto existingVertexShader = m_dumped_vertex_shaders.find(vertexShader);
            if (existingVertexShader == m_dumped_vertex_shaders.end())
            {
                m_dumped_vertex_shaders.emplace(vertexShader);
                return true;
            }

            return false;
        }

    private:
        std::unordered_set<const MaterialTechnique*> m_dumped_techniques;
        std::unordered_set<const MaterialPixelShader*> m_dumped_pixel_shaders;
        std::unordered_set<const MaterialVertexShader*> m_dumped_vertex_shaders;
    };

    void DumpPixelShader(const AssetDumpingContext& context, const MaterialPixelShader& pixelShader)
    {
        const auto shaderFile = context.OpenAssetFile(shader::GetFileNameForPixelShaderAssetName(pixelShader.name));

        if (!shaderFile)
            return;

        shaderFile->write(pixelShader.prog.loadDef.program, pixelShader.prog.loadDef.programSize);
    }

    void DumpVertexShader(const AssetDumpingContext& context, const MaterialVertexShader& vertexShader)
    {
        const auto shaderFile = context.OpenAssetFile(shader::GetFileNameForVertexShaderAssetName(vertexShader.name));

        if (!shaderFile)
            return;

        shaderFile->write(vertexShader.prog.loadDef.program, vertexShader.prog.loadDef.programSize);
    }
} // namespace

namespace techset
{
    DumperT6::DumperT6(const AssetPool<AssetTechniqueSet::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetTechniqueSet::Type>& asset)
    {
        const auto* techniqueSet = asset.Asset();
        auto* shaderState = context.GetZoneAssetDumperState<ShaderZoneState>();

        for (const auto* technique : techniqueSet->techniques)
        {
            if (!technique || !shaderState->ShouldDumpTechnique(technique))
                continue;

            for (auto passIndex = 0u; passIndex < technique->passCount; passIndex++)
            {
                const auto* pixelShader = technique->passArray[passIndex].pixelShader;
                if (pixelShader && shaderState->ShouldDumpPixelShader(pixelShader))
                    DumpPixelShader(context, *pixelShader);

                const auto* vertexShader = technique->passArray[passIndex].vertexShader;
                if (vertexShader && shaderState->ShouldDumpVertexShader(vertexShader))
                    DumpVertexShader(context, *vertexShader);
            }
        }
    }
} // namespace techset
