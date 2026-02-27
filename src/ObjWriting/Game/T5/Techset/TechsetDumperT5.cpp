#include "TechsetDumperT5.h"

#include "Game/T5/Material/MaterialConstantZoneStateT5.h"
#include "Game/T5/Techset/TechsetConstantsT5.h"
#include "Shader/ShaderCommon.h"
#include "Techset/CommonTechniqueDumper.h"
#include "Techset/CommonTechsetDumper.h"
#include "Techset/ShaderDumpingZoneState.h"
#include "Techset/TechniqueDumpingZoneState.h"

#include <cstdint>

using namespace T5;

namespace
{
    void DumpPixelShader(const AssetDumpingContext& context, const MaterialPixelShader& pixelShader)
    {
        const auto shaderFile = context.OpenAssetFile(shader::GetFileNameForPixelShaderAssetName(pixelShader.name));

        if (!shaderFile)
            return;

        shaderFile->write(reinterpret_cast<char*>(pixelShader.prog.loadDef.program), pixelShader.prog.loadDef.programSize * sizeof(uint32_t));
    }

    void DumpVertexShader(const AssetDumpingContext& context, const MaterialVertexShader& vertexShader)
    {
        const auto shaderFile = context.OpenAssetFile(shader::GetFileNameForVertexShaderAssetName(vertexShader.name));

        if (!shaderFile)
            return;

        shaderFile->write(reinterpret_cast<char*>(vertexShader.prog.loadDef.program), vertexShader.prog.loadDef.programSize * sizeof(uint32_t));
    }

    void DumpShaders(AssetDumpingContext& context, const MaterialTechniqueSet& techset)
    {
        auto* shaderState = context.GetZoneAssetDumperState<techset::ShaderDumpingZoneState>();

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

    techset::CommonVertexDeclaration ConvertToCommonVertexDeclaration(const MaterialVertexDeclaration* vertexDecl)
    {
        std::vector<techset::CommonStreamRouting> commonRouting;

        if (vertexDecl)
        {
            const auto streamCount = std::min(static_cast<size_t>(vertexDecl->streamCount), std::extent_v<decltype(MaterialVertexStreamRouting::data)>);
            for (auto streamIndex = 0u; streamIndex < streamCount; streamIndex++)
            {
                const auto& routing = vertexDecl->routing.data[streamIndex];
                commonRouting.emplace_back(static_cast<techset::CommonStreamSource>(routing.source),
                                           static_cast<techset::CommonStreamDestination>(routing.dest));
            }
        }

        return techset::CommonVertexDeclaration(std::move(commonRouting));
    }

    techset::CommonShaderArg ConvertToCommonArg(const MaterialShaderArgument& arg)
    {
        const techset::CommonShaderArgDestination destination{.dx9 = {.m_destination_register = arg.dest}};

        switch (arg.type)
        {
        case MTL_ARG_CODE_VERTEX_CONST:
        case MTL_ARG_CODE_PIXEL_CONST:
        {
            const techset::CommonShaderArgCodeConstValue codeConstValue{
                .m_index = static_cast<techset::CommonCodeConstSource>(arg.u.codeConst.index),
                .m_first_row = arg.u.codeConst.firstRow,
                .m_row_count = arg.u.codeConst.rowCount,
            };
            const techset::CommonShaderArgValue value{.code_const_source = codeConstValue};

            return techset::CommonShaderArg(commonArgumentTypes[arg.type], destination, value);
        }

        case MTL_ARG_MATERIAL_VERTEX_CONST:
        case MTL_ARG_MATERIAL_PIXEL_CONST:
        {
            const techset::CommonShaderArgValue value{
                .name_hash = arg.u.nameHash,
            };

            return techset::CommonShaderArg(commonArgumentTypes[arg.type], destination, value);
        }

        case MTL_ARG_CODE_PIXEL_SAMPLER:
        {
            const techset::CommonShaderArgValue value{
                .code_sampler_source = static_cast<techset::CommonCodeSamplerSource>(arg.u.codeSampler),
            };

            return techset::CommonShaderArg(commonArgumentTypes[arg.type], destination, value);
        }

        case MTL_ARG_MATERIAL_PIXEL_SAMPLER:
        {
            const techset::CommonShaderArgValue value{
                .name_hash = arg.u.nameHash,
            };

            return techset::CommonShaderArg(commonArgumentTypes[arg.type], destination, value);
        }

        default:
        case MTL_ARG_LITERAL_VERTEX_CONST:
        case MTL_ARG_LITERAL_PIXEL_CONST:
        {
            techset::CommonShaderArgValue value{};
            if (arg.u.literalConst)
            {
                value.literal_value = {
                    (*arg.u.literalConst)[0],
                    (*arg.u.literalConst)[1],
                    (*arg.u.literalConst)[2],
                    (*arg.u.literalConst)[3],
                };
            }

            return techset::CommonShaderArg(commonArgumentTypes[arg.type], destination, value);
        }
        }
    }

    techset::CommonTechniqueShader ConvertToCommonShader(const MaterialVertexShader* vertexShader)
    {
        techset::CommonTechniqueShader result{};
        if (!vertexShader)
            return result;

        if (vertexShader->name)
            result.m_name = vertexShader->name;

        if (vertexShader->prog.loadDef.program)
        {
            result.m_bin = techset::CommonTechniqueShaderBin{
                .m_shader_bin = vertexShader->prog.loadDef.program,
                .m_shader_bin_size = vertexShader->prog.loadDef.programSize * sizeof(uint32_t),
            };
        }

        return result;
    }

    techset::CommonTechniqueShader ConvertToCommonShader(const MaterialPixelShader* pixelShader)
    {
        techset::CommonTechniqueShader result{};
        if (!pixelShader)
            return result;

        if (pixelShader->name)
            result.m_name = pixelShader->name;

        if (pixelShader->prog.loadDef.program)
        {
            result.m_bin = techset::CommonTechniqueShaderBin{
                .m_shader_bin = pixelShader->prog.loadDef.program,
                .m_shader_bin_size = pixelShader->prog.loadDef.programSize * sizeof(uint32_t),
            };
        }

        return result;
    }

    techset::CommonTechnique ConvertToCommonTechnique(const MaterialTechnique& technique)
    {
        techset::CommonTechnique commonTechnique(technique.name ? technique.name : std::string(), technique.flags);

        for (auto passIndex = 0u; passIndex < technique.passCount; passIndex++)
        {
            const auto& pass = technique.passArray[passIndex];
            techset::CommonPass commonPass(pass.customSamplerFlags,
                                           // No clue what the actual state map was
                                           "passthrough",
                                           ConvertToCommonShader(pass.vertexShader),
                                           ConvertToCommonShader(pass.pixelShader),
                                           ConvertToCommonVertexDeclaration(pass.vertexDecl));

            if (pass.args)
            {
                const size_t totalArgCount = pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount;
                commonPass.m_args.reserve(totalArgCount);
                for (auto argIndex = 0uz; argIndex < totalArgCount; argIndex++)
                    commonPass.m_args.emplace_back(ConvertToCommonArg(pass.args[argIndex]));
            }

            commonTechnique.m_passes.emplace_back(std::move(commonPass));
        }

        return commonTechnique;
    }

    void DumpTechniques(AssetDumpingContext& context, const MaterialTechniqueSet& techset)
    {
        auto* techniqueState = context.GetZoneAssetDumperState<techset::TechniqueDumpingZoneState>();
        const auto* materialConstantState = context.GetZoneAssetDumperState<MaterialConstantZoneState>();
        for (const auto* technique : techset.techniques)
        {
            if (technique && techniqueState->ShouldDumpTechnique(technique))
            {
                const auto commonTechnique = ConvertToCommonTechnique(*technique);

                techset::DumpCommonTechnique(
                    context, commonTechnique, techset::DxVersion::DX9, commonCodeSourceInfos, commonRoutingInfos, *materialConstantState);
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

        return techset::CommonTechset(techset.name, std::move(techniqueNames));
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
    void DumperT5::Dump(AssetDumpingContext& context)
    {
        context.GetZoneAssetDumperState<MaterialConstantZoneState>()->EnsureInitialized();
        AbstractAssetDumper::Dump(context);
    }

    void DumperT5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetTechniqueSet::Type>& asset)
    {
        const auto* techniqueSet = asset.Asset();
        DumpTechset(context, *techniqueSet);
        DumpTechniques(context, *techniqueSet);
        DumpShaders(context, *techniqueSet);
    }
} // namespace techset
