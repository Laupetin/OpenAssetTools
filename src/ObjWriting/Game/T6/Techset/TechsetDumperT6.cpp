#include "TechsetDumperT6.h"

#include "Game/T6/Techset/TechsetConstantsT6.h"
#include "Shader/ShaderCommon.h"
#include "Techset/CommonTechniqueDumper.h"
#include "Techset/CommonTechsetDumper.h"
#include "Techset/TechniqueDumpingZoneState.h"

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

    void DumpShaders(AssetDumpingContext& context, const MaterialTechniqueSet& techset)
    {
        auto* shaderState = context.GetZoneAssetDumperState<ShaderZoneState>();

        for (const auto* technique : techset.techniques)
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

    techset::CommonTechnique ConvertToCommonTechnique(const MaterialTechnique& technique)
    {
        std::vector<techset::CommonPass> passes;

        for (auto passIndex = 0u; passIndex < technique.passCount; passIndex++)
        {
            const auto& pass = technique.passArray[passIndex];

            techset::CommonTechniqueShader vertexShader{};
            techset::CommonTechniqueShader pixelShader{};

            if (pass.vertexShader)
            {
                if (pass.vertexShader->name)
                    vertexShader.m_name = pass.vertexShader->name;

                if (pass.vertexShader->prog.loadDef.program)
                {
                    vertexShader.m_shader_bin = pass.vertexShader->prog.loadDef.program;
                    vertexShader.m_shader_bin_size = pass.vertexShader->prog.loadDef.programSize * sizeof(uint32_t);
                }
            }

            if (pass.pixelShader)
            {
                if (pass.pixelShader->name)
                    pixelShader.m_name = pass.pixelShader->name;

                if (pass.pixelShader->prog.loadDef.program)
                {
                    pixelShader.m_shader_bin = pass.pixelShader->prog.loadDef.program;
                    pixelShader.m_shader_bin_size = pass.pixelShader->prog.loadDef.programSize * sizeof(uint32_t);
                }
            }

            passes.emplace_back(techset::CommonPass{
                .m_sampler_flags = pass.customSamplerFlags,
                .m_dx_version = techset::DxVersion::DX11,
                .m_vertex_shader = vertexShader,
                .m_pixel_shader = pixelShader,
            });
        }

        return techset::CommonTechnique{
            .m_name = technique.name ? technique.name : std::string(),
            .m_flags = technique.flags,
            .m_passes = std::move(passes),
        };
    }

    void DumpTechniques(AssetDumpingContext& context, const MaterialTechniqueSet& techset)
    {
        auto* techniqueState = context.GetZoneAssetDumperState<techset::TechniqueDumpingZoneState>();
        for (const auto* technique : techset.techniques)
        {
            if (technique && techniqueState->ShouldDumpTechnique(technique))
            {
                const auto commonTechnique = ConvertToCommonTechnique(*technique);

                techset::DumpCommonTechnique(context, commonTechnique);
            }
        }
    }

    techset::CommonTechset ConvertToCommonTechset(const MaterialTechniqueSet& techset)
    {
        std::vector<std::string> techniqueNames(std::extent_v<decltype(techniqueTypeNames)>);

        for (auto techniqueIndex = 0u; techniqueIndex < std::extent_v<decltype(techniqueTypeNames)>; techniqueIndex++)
        {
            const auto* technique = techset.techniques[techniqueIndex];
            if (technique && technique->name)
                techniqueNames[techniqueIndex] = technique->name;
        }

        return techset::CommonTechset{
            .m_name = techset.name,
            .m_technique_names = std::move(techniqueNames),
        };
    }

    void DumpTechset(const AssetDumpingContext& context, const MaterialTechniqueSet& techset)
    {
        static techset::CommonTechniqueTypeNames commonNames(techniqueTypeNames, std::extent_v<decltype(techniqueTypeNames)>);
        const auto commonTechset = ConvertToCommonTechset(techset);

        techset::DumpCommonTechset(commonNames, context, commonTechset);
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
        DumpTechset(context, *techniqueSet);
        DumpTechniques(context, *techniqueSet);
        DumpShaders(context, *techniqueSet);
    }
} // namespace techset
