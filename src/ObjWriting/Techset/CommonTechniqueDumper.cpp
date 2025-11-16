#include "CommonTechniqueDumper.h"

#include "Dumping/AbstractTextDumper.h"
#include "Shader/D3D11ShaderAnalyser.h"
#include "Shader/D3D9ShaderAnalyser.h"
#include "Techset/TechsetCommon.h"

#include <cassert>
#include <cstdint>

using namespace techset;

namespace
{
    enum class TechniqueShaderType : std::uint8_t
    {
        VERTEX_SHADER,
        PIXEL_SHADER
    };

    class TechniqueFileWriter : public AbstractTextDumper
    {
    public:
        TechniqueFileWriter(const CommonStreamRoutingInfos& routingInfos, std::ostream& stream)
            : AbstractTextDumper(stream),
              m_routing_infos(routingInfos)
        {
        }

        void DumpTechnique(const CommonTechnique& technique)
        {
#ifdef TECHSET_DEBUG
            if (technique.m_flags)
            {
                for (auto i = 0u; i < sizeof(CommonTechnique::m_flags) * 8u; i++)
                {
                    const auto mask = 1ui64 << i;
                    if (technique.m_flags & mask)
                    {
                        Indent();
                        m_stream << std::format("// TECHNIQUE FLAGS: 0x{:x}\n", mask);
                    }
                }
            }
#endif

            for (const auto& pass : technique.m_passes)
                DumpPass(pass);
        }

    private:
        void DumpPass(const CommonPass& pass)
        {
            m_stream << "{\n";
            IncIndent();

#ifdef TECHSET_DEBUG
            for (auto i = 0u; i < sizeof(CommonPass::m_sampler_flags) * 8u; i++)
            {
                const auto mask = 1ui64 << i;
                if (pass.m_sampler_flags & mask)
                {
                    Indent();
                    m_stream << std::format("// CUSTOM SAMPLER FLAGS: 0x{:x}\n", mask);
                }
            }
#endif

            DumpStateMap();
            DumpShader(pass.m_vertex_shader, TechniqueShaderType::VERTEX_SHADER, pass.m_dx_version);
            DumpShader(pass.m_pixel_shader, TechniqueShaderType::PIXEL_SHADER, pass.m_dx_version);
            DumpVertexDecl(pass.m_vertex_declaration);

            DecIndent();
            m_stream << "}\n";
        }

        void DumpStateMap() const
        {
            Indent();
            // TODO: Actual statemap: Maybe find all materials using this techset and try to make out rules
            // for the flags based on the statebitstable
            m_stream << "stateMap \"passthrough\"; // TODO\n";
        }

        void DumpShader(const CommonTechniqueShader& shader, const TechniqueShaderType shaderType, const DxVersion dxVersion)
        {
            if (!shader.m_shader_bin)
            {
                if (!shader.m_name.empty())
                    m_stream << std::format("// Cannot dump shader {} as its data is not loaded\n", shader.m_name);

                return;
            }

            unsigned versionMajor, versionMinor;
            if (dxVersion == DxVersion::DX9)
            {
                const auto shaderInfo = d3d9::ShaderAnalyser::GetShaderInfo(shader.m_shader_bin, shader.m_shader_bin_size);
                assert(shaderInfo);
                if (!shaderInfo)
                    return;

                versionMajor = shaderInfo->m_version_major;
                versionMinor = shaderInfo->m_version_minor;

                DumpShaderHeader(shader, shaderType, versionMajor, versionMinor);

                for (const auto& arg : shader.m_args)
                    DumpShaderArgDx9(arg, *shaderInfo);
            }
            else
            {
                assert(dxVersion == DxVersion::DX11);
                const auto shaderInfo = d3d11::ShaderAnalyser::GetShaderInfo(shader.m_shader_bin, shader.m_shader_bin_size);
                assert(shaderInfo);
                if (!shaderInfo)
                    return;

                versionMajor = shaderInfo->m_version_major;
                versionMinor = shaderInfo->m_version_minor;

                DumpShaderHeader(shader, shaderType, versionMajor, versionMinor);

                for (const auto& arg : shader.m_args)
                {
                }
            }

            DecIndent();
            Indent();
            m_stream << "}\n";
        }

        void DumpShaderHeader(const CommonTechniqueShader& shader, const TechniqueShaderType shaderType, unsigned versionMajor, unsigned versionMinor)
        {
            const auto shaderTypeName = shaderType == TechniqueShaderType::VERTEX_SHADER ? "vertexShader" : "pixelShader";

            m_stream << "\n";
            Indent();
            m_stream << std::format("{} {}.{} \"{}\"\n", shaderTypeName, versionMajor, versionMinor, shader.m_name);
            Indent();
            m_stream << "{\n";
            IncIndent();
        }

        void DumpShaderArgDx9(const CommonShaderArg& arg, const d3d9::ShaderInfo& shaderInfo)
        {
            const auto expectedRegisterSet = arg.m_type == CommonShaderArgType::CODE_SAMPLER || arg.m_type == CommonShaderArgType::MATERIAL_SAMPLER
                                                 ? d3d9::RegisterSet::SAMPLER
                                                 : d3d9::RegisterSet::FLOAT_4;
            const auto targetShaderArg = std::ranges::find_if(shaderInfo.m_constants,
                                                              [arg, expectedRegisterSet](const d3d9::ShaderConstant& constant)
                                                              {
                                                                  return constant.m_register_set == expectedRegisterSet && constant.m_register_index <= arg.dest
                                                                         && constant.m_register_index + constant.m_register_count > arg.dest;
                                                              });

            assert(targetShaderArg != shaderInfo.m_constants.end());
            if (targetShaderArg == shaderInfo.m_constants.end())
            {
                m_stream << std::format("// Unrecognized arg dest: {} type: {}\n", arg.dest, arg.type);
                return;
            }

            std::string codeDestAccessor;
            if (targetShaderArg->m_type_elements > 1)
            {
                codeDestAccessor = std::format("{}[{}]", targetShaderArg->m_name, arg.dest - targetShaderArg->m_register_index)
            }
            else
                codeDestAccessor = targetShaderArg->m_name;

            if (arg.m_type == CommonShaderArgType::CODE_CONST)
            {
                const auto sourceIndex = static_cast<MaterialConstantSource>(arg.u.codeConst.index);
                std::string codeSourceAccessor;
                if (FindCodeConstantSourceAccessor(sourceIndex, s_codeConsts, codeSourceAccessor)
                    || FindCodeConstantSourceAccessor(sourceIndex, s_defaultCodeConsts, codeSourceAccessor))
                {
                    if (codeDestAccessor != codeSourceAccessor)
                    {
                        Indent();
                        m_stream << codeDestAccessor << " = constant." << codeSourceAccessor << ";\n";
                    }
                    else
                    {
#ifdef TECHSET_DEBUG
                        Indent();
                        m_stream << "// Omitted due to matching accessors: " << codeDestAccessor << " = constant." << codeSourceAccessor << ";\n";
#endif
                    }
                }
                else
                {
                    assert(false);
                    Indent();
                    m_stream << codeDestAccessor << " = UNKNOWN;\n";
                }
            }
            else if (arg.m_type == CommonShaderArgType::CODE_SAMPLER)
            {
                const auto sourceIndex = static_cast<MaterialTextureSource>(arg.u.codeSampler);
                std::string codeSourceAccessor;
                if (FindCodeSamplerSourceAccessor(sourceIndex, s_codeSamplers, codeSourceAccessor)
                    || FindCodeSamplerSourceAccessor(sourceIndex, s_defaultCodeSamplers, codeSourceAccessor))
                {
                    if (codeDestAccessor != codeSourceAccessor)
                    {
                        Indent();
                        m_stream << codeDestAccessor << " = sampler." << codeSourceAccessor << ";\n";
                    }
                    else
                    {
#ifdef TECHSET_DEBUG
                        Indent();
                        m_stream << "// Omitted due to matching accessors: " << codeDestAccessor << " = sampler." << codeSourceAccessor << ";\n";
#endif
                    }
                }
                else
                {
                    assert(false);
                    Indent();
                    m_stream << codeDestAccessor << " = UNKNOWN;\n";
                }
            }
            else if (arg.m_type == CommonShaderArgType::LITERAL_CONST)
            {
                if (arg.u.literalConst)
                {
                    Indent();
                    m_stream << codeDestAccessor << " = float4( " << (*arg.u.literalConst)[0] << ", " << (*arg.u.literalConst)[1] << ", "
                             << (*arg.u.literalConst)[2] << ", " << (*arg.u.literalConst)[3] << " );\n";
                }
            }
            else if (arg.m_type == CommonShaderArgType::MATERIAL_CONST || arg.m_type == CommonShaderArgType::MATERIAL_SAMPLER)
            {
                Indent();
                m_stream << codeDestAccessor << " = material.";

                const auto knownConstantName = knownConstantNames.find(arg.u.nameHash);
                if (knownConstantName != knownConstantNames.end())
                {
                    m_stream << knownConstantName->second;
                }
                else
                {
                    const auto knownMaterialTextureName = knownTextureMaps.find(arg.u.nameHash);

                    if (knownMaterialTextureName != knownTextureMaps.end())
                    {
                        m_stream << knownMaterialTextureName->second.m_name;
                    }
                    else
                    {
                        const auto shaderArgNameHash = Common::R_HashString(targetShaderArg->m_name.c_str(), 0u);
                        if (shaderArgNameHash == arg.u.nameHash)
                            m_stream << targetShaderArg->m_name;
                        else
                            m_stream << "#0x" << std::hex << arg.u.nameHash;
                    }
                }

                m_stream << ";\n";
            }
            else
            {
                assert(false);
            }
        }

        void DumpVertexDecl(const CommonVertexDeclaration& vertexDeclaration) const
        {
            if (vertexDeclaration.m_routing.empty())
                return;

            m_stream << "\n";

            for (const auto& routing : vertexDeclaration.m_routing)
            {
                Indent();
                m_stream << std::format(
                    "vertex.{} = code.{};\n", m_routing_infos.GetDestinationName(routing.m_destination), m_routing_infos.GetSourceName(routing.m_source));
            }
        }

        const CommonStreamRoutingInfos& m_routing_infos;
    };
} // namespace

namespace techset
{
    void DumpCommonTechnique(const CommonStreamRoutingInfos& routingInfos, const AssetDumpingContext& context, const CommonTechnique& technique)
    {
        const auto techniqueFile = context.OpenAssetFile(GetFileNameForTechniqueName(technique.m_name));
        if (techniqueFile)
        {
            TechniqueFileWriter writer(routingInfos, *techniqueFile);
            writer.DumpTechnique(technique);
        }
    }
} // namespace techset
