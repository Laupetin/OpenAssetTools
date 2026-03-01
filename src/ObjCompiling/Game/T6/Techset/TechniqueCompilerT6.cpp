#include "TechniqueCompilerT6.h"

#include "Game/T6/T6.h"
#include "Game/T6/Techset/TechsetConstantsT6.h"
#include "Techset/CommonShaderArgCreator.h"
#include "Techset/CommonTechniqueLoader.h"
#include "Techset/LiteralConstsZoneState.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <sstream>

using namespace T6;

namespace
{
    unsigned ConvertArgumentType(const techset::CommonShaderArgumentType& type)
    {
        if (type.m_shader_type == techset::CommonTechniqueShaderType::VERTEX)
        {
            switch (type.m_value_type)
            {
            case techset::CommonShaderValueType::LITERAL_CONST:
                return MTL_ARG_LITERAL_VERTEX_CONST;
            case techset::CommonShaderValueType::MATERIAL_CONST:
                return MTL_ARG_MATERIAL_VERTEX_CONST;
            case techset::CommonShaderValueType::CODE_CONST:
                return MTL_ARG_CODE_VERTEX_CONST;
            case techset::CommonShaderValueType::MATERIAL_SAMPLER:
            case techset::CommonShaderValueType::CODE_SAMPLER:
            default:
                assert(false);
                return 0;
            }
        }

        assert(type.m_shader_type == techset::CommonTechniqueShaderType::PIXEL);

        switch (type.m_value_type)
        {
        case techset::CommonShaderValueType::LITERAL_CONST:
            return MTL_ARG_LITERAL_PIXEL_CONST;
        case techset::CommonShaderValueType::MATERIAL_CONST:
            return MTL_ARG_MATERIAL_PIXEL_CONST;
        case techset::CommonShaderValueType::CODE_CONST:
            return MTL_ARG_CODE_PIXEL_CONST;
        case techset::CommonShaderValueType::MATERIAL_SAMPLER:
            return MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        case techset::CommonShaderValueType::CODE_SAMPLER:
            return MTL_ARG_CODE_PIXEL_SAMPLER;
        default:
            assert(false);
            return 0;
        }
    }

    void ConvertArgumentValue(MaterialArgumentDef& argValue, const techset::CommonShaderArg& commonArg, MemoryManager& memory, AssetCreationContext& context)
    {
        switch (commonArg.m_type.m_value_type)
        {
        case techset::CommonShaderValueType::LITERAL_CONST:
        {
            const techset::LiteralConst literal(commonArg.m_value.literal_value);
            argValue.literalConst = context.GetZoneAssetCreationState<techset::LiteralConstsZoneState<float>>().GetAllocatedLiteral(literal)->GamePtr();
            break;
        }
        case techset::CommonShaderValueType::CODE_CONST:
            argValue.codeConst.index = static_cast<decltype(MaterialArgumentCodeConst::index)>(commonArg.m_value.code_const_source.m_index);
            argValue.codeConst.firstRow = static_cast<decltype(MaterialArgumentCodeConst::firstRow)>(commonArg.m_value.code_const_source.m_first_row);
            argValue.codeConst.rowCount = static_cast<decltype(MaterialArgumentCodeConst::rowCount)>(commonArg.m_value.code_const_source.m_row_count);
            break;
        case techset::CommonShaderValueType::CODE_SAMPLER:
            argValue.codeSampler = static_cast<decltype(MaterialArgumentDef::codeSampler)>(commonArg.m_value.code_sampler_source);
            break;
        case techset::CommonShaderValueType::MATERIAL_CONST:
        case techset::CommonShaderValueType::MATERIAL_SAMPLER:
            argValue.nameHash = static_cast<decltype(MaterialArgumentDef::nameHash)>(commonArg.m_value.name_hash);
            break;
        }
    }

    void ConvertMaterialArgs(MaterialPass& pass, const techset::CommonPass& commonPass, MemoryManager& memory, AssetCreationContext& context)
    {
        pass.args = memory.Alloc<MaterialShaderArgument>(commonPass.m_args.size());

        const auto frequencyCount = commonPass.GetFrequencyCounts(commonCodeSourceInfos);

        pass.perObjArgCount = static_cast<unsigned char>(frequencyCount[std::to_underlying(techset::CommonCodeSourceUpdateFrequency::PER_OBJECT)]);
        pass.perPrimArgCount = static_cast<unsigned char>(frequencyCount[std::to_underlying(techset::CommonCodeSourceUpdateFrequency::PER_PRIM)]);
        pass.stableArgCount = static_cast<unsigned char>(frequencyCount[std::to_underlying(techset::CommonCodeSourceUpdateFrequency::RARELY)]);

        const auto commonArgCount = commonPass.m_args.size();
        for (auto argIndex = 0u; argIndex < commonArgCount; argIndex++)
        {
            auto& arg = pass.args[argIndex];
            const auto& commonArg = commonPass.m_args[argIndex];

            arg.type = static_cast<decltype(MaterialShaderArgument::type)>(ConvertArgumentType(commonArg.m_type));

            arg.size = static_cast<decltype(MaterialShaderArgument::size)>(commonArg.m_destination.dx11.m_size);
            arg.buffer = static_cast<decltype(MaterialShaderArgument::buffer)>(commonArg.m_destination.dx11.m_buffer);

            if (techset::IsConstValueType(commonArg.m_type.m_value_type))
            {
                arg.location.offset = static_cast<decltype(MaterialArgumentLocation::offset)>(commonArg.m_destination.dx11.m_location.constant_buffer_offset);
            }
            else
            {
                assert(techset::IsSamplerValueType(commonArg.m_type.m_value_type));
                arg.location.textureIndex =
                    static_cast<decltype(MaterialArgumentLocation::textureIndex)>(commonArg.m_destination.dx11.m_location.texture_index);
                arg.location.samplerIndex =
                    static_cast<decltype(MaterialArgumentLocation::samplerIndex)>(commonArg.m_destination.dx11.m_location.sampler_index);
            }

            ConvertArgumentValue(arg.u, commonArg, memory, context);
        }
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

        ConvertMaterialArgs(pass, commonPass, memory, context);
        pass.customSamplerFlags = static_cast<decltype(MaterialPass::customSamplerFlags)>(commonPass.m_sampler_flags);
    }

    bool AnyDeclHasOptionalSource(const MaterialTechnique& technique)
    {
        for (auto passIndex = 0u; passIndex < technique.passCount; passIndex++)
        {
            const auto& pass = technique.passArray[passIndex];
            if (!pass.vertexDecl)
                continue;

            if (pass.vertexDecl->hasOptionalSource)
                return true;
        }

        return false;
    }

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

        if (AnyDeclHasOptionalSource(technique))
            technique.flags |= TECHNIQUE_FLAG_8;
    }

    MaterialTechnique* ConvertTechnique(const techset::CommonTechnique& commonTechnique, AssetCreationContext& context, MemoryManager& memory)
    {
        const auto additionalPassCount = std::max(commonTechnique.m_passes.size(), 1uz) - 1uz;
        auto* technique = static_cast<MaterialTechnique*>(memory.AllocRaw(sizeof(MaterialTechnique) + additionalPassCount * sizeof(MaterialPass)));

        const auto passCount = static_cast<decltype(MaterialTechnique::passCount)>(commonTechnique.m_passes.size());

        technique->name = memory.Dup(commonTechnique.m_name.c_str());

        technique->passCount = passCount;

        for (auto passIndex = 0u; passIndex < passCount; passIndex++)
            ConvertMaterialPass(technique->passArray[passIndex], commonTechnique.m_passes[passIndex], context, memory);

        // Take common flags and apply further logic
        technique->flags = static_cast<decltype(MaterialTechnique::flags)>(commonTechnique.m_flags);
        UpdateTechniqueFlags(*technique, commonTechnique);

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
            const auto commonShaderArgCreator = techset::CommonShaderArgCreator::CreateDx11(shaderLoader, context, commonCodeSourceInfos);

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
