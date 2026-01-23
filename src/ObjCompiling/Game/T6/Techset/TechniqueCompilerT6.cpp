#include "TechniqueCompilerT6.h"

#include "Game/T6/T6.h"
#include "Game/T6/Techset/TechsetConstantsT6.h"
#include "Techset/CommonShaderArgCreator.h"
#include "Techset/CommonTechniqueLoader.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <sstream>

using namespace T6;

namespace
{
    void UpdateTechniqueFlags(MaterialTechnique& technique, const techset::CommonTechnique& commonTechnique)
    {
        std::string lowerTechniqueName(commonTechnique.m_name);
        utils::MakeStringLowerCase(lowerTechniqueName);

        // Not a particularly cool way to do this but...
        // the game actually does this :shrug:
        if (lowerTechniqueName == "zprepass" || lowerTechniqueName.starts_with("pimp_technique_zprepass_")
            || lowerTechniqueName.starts_with("pimp_technique_layer_zprepass_") || lowerTechniqueName.starts_with("pimp_technique_buildshadowmap_"))
        {
            technique.flags |= TECHNIQUE_FLAG_4;
        }
    }

    void ConvertMaterialArgs(MaterialPass& pass, const techset::CommonPass& commonPass, MemoryManager& memory)
    {
        pass.args = memory.Alloc<MaterialShaderArgument>(commonPass.m_args.size());

        size_t perObjArgCount = 0u;
        size_t perPrimArgCount = 0u;
        size_t stableArgCount = 0u;
        size_t argIndex = 0u;

        const auto frequencyCount = commonPass.GetFrequencyCounts(commonCodeSourceInfos);

        for (const auto& arg : commonPass.m_args)
        {
            UpdateTechniqueFlagsForArgument(technique.flags, arg);
            switch (arg.m_update_frequency)
            {
            case MTL_UPDATE_PER_PRIM:
                perPrimArgCount++;
                break;
            case MTL_UPDATE_PER_OBJECT:
                perObjArgCount++;
                break;
            case MTL_UPDATE_RARELY:
                stableArgCount++;
                break;
            case MTL_UPDATE_CUSTOM:
            {
                assert(arg.m_arg.type == MTL_ARG_CODE_PIXEL_SAMPLER);
                if (arg.m_arg.type == MTL_ARG_CODE_PIXEL_SAMPLER)
                {
                    const auto customSampler = std::ranges::find(g_customSamplerSrc, arg.m_arg.u.codeSampler);
                    assert(customSampler != std::end(g_customSamplerSrc));
                    if (customSampler != std::end(g_customSamplerSrc))
                    {
                        const auto customSamplerIndex = customSampler - std::begin(g_customSamplerSrc);
                        out.customSamplerFlags |= 1 << customSamplerIndex;
                    }
                }
            }
                continue;
            default:
                assert(false);
                continue;
            }

            out.args[argIndex++] = arg.m_arg;
        }

        pass.perObjArgCount = static_cast<unsigned char>(frequencyCount[std::to_underlying(techset::CommonCodeSourceUpdateFrequency::PER_OBJECT)]);
        pass.perPrimArgCount = static_cast<unsigned char>(frequencyCount[std::to_underlying(techset::CommonCodeSourceUpdateFrequency::PER_PRIM)]);
        pass.stableArgCount = static_cast<unsigned char>(frequencyCount[std::to_underlying(techset::CommonCodeSourceUpdateFrequency::RARELY)]);
    }

    void ConvertVertexDecl(MaterialPass& pass, const techset::CommonVertexDeclaration& commonDecl, AssetCreationContext& context)
    {
        std::ostringstream nameStream;
        for (const auto& entry : commonDecl.m_routing)
        {
            nameStream << commonRoutingInfos.GetSourceAbbreviation(entry.m_source);
            nameStream << commonRoutingInfos.GetDestinationAbbreviation(entry.m_destination);
        }

        const std::string declName(nameStream.str());
        auto* vertexDeclAsset = context.LoadSubAsset<SubAssetVertexDecl>(declName);
        assert(vertexDeclAsset);
        pass.vertexDecl = vertexDeclAsset ? vertexDeclAsset->Asset() : nullptr;
    }

    void ConvertMaterialPass(MaterialPass& pass, const techset::CommonPass& commonPass, AssetCreationContext& context, MemoryManager& memory)
    {
        ConvertVertexDecl(pass, commonPass.m_vertex_declaration, context);

        if (!commonPass.m_vertex_shader.m_name.empty())
        {
            auto* vertexShaderAsset = context.LoadSubAsset<SubAssetVertexShader>(commonPass.m_vertex_shader.m_name);
            assert(vertexShaderAsset);
            pass.vertexShader = vertexShaderAsset ? vertexShaderAsset->Asset() : nullptr;
        }

        if (!commonPass.m_pixel_shader.m_name.empty())
        {
            auto* pixelShaderAsset = context.LoadSubAsset<SubAssetPixelShader>(commonPass.m_pixel_shader.m_name);
            assert(pixelShaderAsset);
            pass.pixelShader = pixelShaderAsset ? pixelShaderAsset->Asset() : nullptr;
        }
    }

    MaterialTechnique* ConvertTechnique(const techset::CommonTechnique& commonTechnique, AssetCreationContext& context, MemoryManager& memory)
    {
        const auto additionalPassCount = std::max(commonTechnique.m_passes.size(), 1u) - 1u;
        auto* technique = static_cast<MaterialTechnique*>(memory.AllocRaw(sizeof(MaterialTechnique) + additionalPassCount * sizeof(MaterialPass)));

        const auto passCount = static_cast<decltype(MaterialTechnique::passCount)>(commonTechnique.m_passes.size());

        technique->name = memory.Dup(commonTechnique.m_name.c_str());

        // Take common flags and apply further logic
        technique->flags = static_cast<decltype(MaterialTechnique::flags)>(commonTechnique.m_flags);
        UpdateTechniqueFlags(*technique, commonTechnique);

        technique->passCount = passCount;

        for (auto passIndex = 0u; passIndex < passCount; passIndex++)
            ConvertMaterialPass(technique->passArray[passIndex], commonTechnique.m_passes[passIndex], context, memory);

        return technique;
    }

    class TechniqueShaderLoaderT6 final : public techset::ITechniqueShaderLoader
    {
    public:
        explicit TechniqueShaderLoaderT6(AssetCreationContext& context)
            : m_context(context)
        {
        }

        std::optional<techset::CommonTechniqueShaderBin> LoadVertexShader(const std::string& name) override
        {
            auto* shaderAsset = m_context.LoadSubAsset<SubAssetVertexShader>(name);
            if (!shaderAsset)
                return std::nullopt;

            const auto* shader = shaderAsset->Asset();
            assert(shader->prog.loadDef.program && shader->prog.loadDef.programSize > 0);
            if (!shader->prog.loadDef.program || shader->prog.loadDef.programSize == 0)
                return std::nullopt;

            return techset::CommonTechniqueShaderBin{
                .m_shader_bin = shader->prog.loadDef.program,
                .m_shader_bin_size = shader->prog.loadDef.programSize,
            };
        }

        std::optional<techset::CommonTechniqueShaderBin> LoadPixelShader(const std::string& name) override
        {
            auto* shaderAsset = m_context.LoadSubAsset<SubAssetPixelShader>(name);
            if (!shaderAsset)
                return std::nullopt;

            const auto* shader = shaderAsset->Asset();
            assert(shader->prog.loadDef.program && shader->prog.loadDef.programSize > 0);
            if (!shader->prog.loadDef.program || shader->prog.loadDef.programSize == 0)
                return std::nullopt;

            return techset::CommonTechniqueShaderBin{
                .m_shader_bin = shader->prog.loadDef.program,
                .m_shader_bin_size = shader->prog.loadDef.programSize,
            };
        }

    private:
        AssetCreationContext& m_context;
    };

    class TechniqueCompilerT6 final : public SubAssetCreator<SubAssetTechnique>
    {
    public:
        TechniqueCompilerT6(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateSubAsset(const std::string& subAssetName, AssetCreationContext& context) override
        {
            bool failure = false;
            TechniqueShaderLoaderT6 shaderLoader(context);
            const auto commonShaderArgCreator = techset::CommonShaderArgCreator::CreateDx11(shaderLoader, context);

            const auto commonTechnique =
                techset::LoadCommonTechnique(subAssetName, commonCodeSourceInfos, commonRoutingInfos, *commonShaderArgCreator, m_search_path, failure);

            if (!commonTechnique)
                return failure ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            auto* convertedTechnique = ConvertTechnique(*commonTechnique, context, m_memory);
            assert(convertedTechnique);

            return AssetCreationResult::Success(context.AddSubAsset(AssetRegistration<SubAssetTechnique>(subAssetName, convertedTechnique)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace techset
{
    std::unique_ptr<ISubAssetCreator> CreateTechniqueCompilerT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<TechniqueCompilerT6>(memory, searchPath);
    }
} // namespace techset
