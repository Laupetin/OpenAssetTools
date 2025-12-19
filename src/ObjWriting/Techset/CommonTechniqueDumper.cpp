#include "CommonTechniqueDumper.h"

#include "Dumping/AbstractTextDumper.h"
#include "Shader/D3D11ShaderAnalyser.h"
#include "Shader/D3D9ShaderAnalyser.h"
#include "Techset/TechsetCommon.h"
#include "Utils/Logging/Log.h"

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
        TechniqueFileWriter(std::ostream& stream,
                            const CommonCodeSourceInfos& codeSourceInfos,
                            const CommonStreamRoutingInfos& routingInfos,
                            const AbstractMaterialConstantZoneState& constantZoneState)
            : AbstractTextDumper(stream),
              m_code_source_infos(codeSourceInfos),
              m_routing_infos(routingInfos),
              m_constant_zone_state(constantZoneState)
        {
        }

        void DumpTechnique(const CommonTechnique& technique)
        {
#ifdef TECHSET_DEBUG
            if (technique.m_flags)
            {
                for (auto i = 0u; i < sizeof(CommonTechnique::m_flags) * 8u; i++)
                {
                    const auto mask = 1ull << i;
                    if (technique.m_flags & mask)
                    {
                        Indent();
                        m_stream << std::format("// TECHNIQUE FLAGS: 0x{:x}\n", mask);
                    }
                }
            }
#endif

            for (const auto& pass : technique.m_passes)
                DumpPass(technique, pass);
        }

    private:
        void DumpPass(const CommonTechnique& technique, const CommonPass& pass)
        {
            m_stream << "{\n";
            IncIndent();

#ifdef TECHSET_DEBUG
            for (auto i = 0u; i < sizeof(CommonPass::m_sampler_flags) * 8u; i++)
            {
                const auto mask = 1ull << i;
                if (pass.m_sampler_flags & mask)
                {
                    Indent();
                    m_stream << std::format("// CUSTOM SAMPLER FLAGS: 0x{:x}\n", mask);
                }
            }
#endif

            DumpStateMap();
            DumpShader(technique, pass.m_vertex_shader, TechniqueShaderType::VERTEX_SHADER, pass.m_dx_version);
            DumpShader(technique, pass.m_pixel_shader, TechniqueShaderType::PIXEL_SHADER, pass.m_dx_version);
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

        void DumpShader(const CommonTechnique& technique, const CommonTechniqueShader& shader, const TechniqueShaderType shaderType, const DxVersion dxVersion)
        {
            if (!shader.m_shader_bin)
            {
                if (!shader.m_name.empty())
                {
                    m_stream << std::format("// ERROR: Cannot dump shader {} as its data is not loaded\n", shader.m_name);
                    con::error("Technique {}: Cannot dump shader {} as its data is not loaded\n", technique.m_name, shader.m_name);
                }

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
                    DumpShaderArgDx9(technique, arg, *shaderInfo);
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
                    DumpShaderArgDx11(technique, arg, *shaderInfo);
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

        void DumpShaderArgDx9(const CommonTechnique& technique, const CommonShaderArg& arg, const d3d9::ShaderInfo& shaderInfo) const
        {
            const auto expectedRegisterSet = arg.m_type == CommonShaderArgType::CODE_SAMPLER || arg.m_type == CommonShaderArgType::MATERIAL_SAMPLER
                                                 ? d3d9::RegisterSet::SAMPLER
                                                 : d3d9::RegisterSet::FLOAT_4;
            const auto destinationRegister = arg.m_destination.dx9.m_destination_register;
            const auto targetShaderArg = std::ranges::find_if(shaderInfo.m_constants,
                                                              [destinationRegister, expectedRegisterSet](const d3d9::ShaderConstant& constant)
                                                              {
                                                                  return constant.m_register_set == expectedRegisterSet
                                                                         && constant.m_register_index <= destinationRegister
                                                                         && constant.m_register_index + constant.m_register_count > destinationRegister;
                                                              });

            assert(targetShaderArg != shaderInfo.m_constants.end());
            if (targetShaderArg == shaderInfo.m_constants.end())
            {
                Indent();
                m_stream << std::format("// Unrecognized arg dest: {} type: {}\n", destinationRegister, static_cast<unsigned>(arg.m_type));
                con::error("Technique {}: Could not find arg (type: {}; dest: {}) in shader",
                           technique.m_name,
                           destinationRegister,
                           static_cast<unsigned>(arg.m_type));
                return;
            }

            std::string codeDestAccessor;
            if (targetShaderArg->m_type_elements > 1)
            {
                codeDestAccessor = std::format("{}[{}]", targetShaderArg->m_name, destinationRegister - targetShaderArg->m_register_index);
            }
            else
                codeDestAccessor = targetShaderArg->m_name;

            DumpShaderArg(technique, arg, codeDestAccessor);
        }

        void DumpShaderArgDx11(const CommonTechnique& technique, const CommonShaderArg& arg, const d3d11::ShaderInfo& shaderInfo) const
        {
            const auto& destination = arg.m_destination.dx11;
            if (arg.m_type == CommonShaderArgType::CODE_CONST || arg.m_type == CommonShaderArgType::MATERIAL_CONST
                || arg.m_type == CommonShaderArgType::LITERAL_CONST)
            {
                const auto boundResource = std::ranges::find_if(shaderInfo.m_bound_resources,
                                                                [destination](const d3d11::BoundResource& resource)
                                                                {
                                                                    return resource.m_type == d3d11::BoundResourceType::CBUFFER
                                                                           && resource.m_bind_point >= destination.m_buffer
                                                                           && resource.m_bind_point + resource.m_bind_count > destination.m_buffer;
                                                                });
                if (boundResource == shaderInfo.m_bound_resources.end())
                {
                    Indent();
                    m_stream << std::format("// Could not find bound resource for arg buffer: {} offset: {} type: {}\n",
                                            destination.m_buffer,
                                            destination.m_location,
                                            static_cast<unsigned>(arg.m_type));
                    con::error("Technique {}: Could not find bound resource for arg (buffer: {} offset: {} type: {}) in shader",
                               technique.m_name,
                               destination.m_buffer,
                               destination.m_location,
                               static_cast<unsigned>(arg.m_type));
                    return;
                }
                const auto buffer = std::ranges::find_if(shaderInfo.m_constant_buffers,
                                                         [&boundResource](const d3d11::ConstantBuffer& constantBuffer)
                                                         {
                                                             return constantBuffer.m_name == boundResource->m_name;
                                                         });
                if (buffer == shaderInfo.m_constant_buffers.end())
                {
                    Indent();
                    m_stream << std::format("// Could not find buffer for bound resource: {}\n", boundResource->m_name);
                    con::error("Technique {}: Could not find buffer for bound resource: {} in shader", technique.m_name, boundResource->m_name);
                    return;
                }

                const auto variable = std::ranges::find_if(buffer->m_variables,
                                                           [destination](const d3d11::ConstantBufferVariable& var)
                                                           {
                                                               return var.m_offset <= destination.m_location
                                                                      && var.m_offset + var.m_size >= destination.m_location + destination.m_size;
                                                           });
                if (variable == buffer->m_variables.end())
                {
                    Indent();
                    m_stream << std::format("// Could not find variable in buffer: {} offset: {} type: {}\n",
                                            buffer->m_name,
                                            destination.m_location,
                                            static_cast<unsigned>(arg.m_type));
                    con::error("Technique {}: Could not find variable in buffer for arg (buffer: {} offset: {} type: {}) in shader",
                               technique.m_name,
                               buffer->m_name,
                               destination.m_location,
                               static_cast<unsigned>(arg.m_type));
                    return;
                }

                DumpShaderArg(technique, arg, variable->m_name);
            }
            else
            {
                assert(arg.m_type == CommonShaderArgType::CODE_SAMPLER || arg.m_type == CommonShaderArgType::MATERIAL_SAMPLER);

                const auto boundResource = std::ranges::find_if(shaderInfo.m_bound_resources,
                                                                [destination](const d3d11::BoundResource& resource)
                                                                {
                                                                    return (resource.m_type == d3d11::BoundResourceType::SAMPLER
                                                                            || resource.m_type == d3d11::BoundResourceType::TEXTURE)
                                                                           && resource.m_bind_point == destination.m_location;
                                                                });
                if (boundResource == shaderInfo.m_bound_resources.end())
                {
                    Indent();
                    m_stream << std::format("// Could not find buffer for arg buffer: {} offset: {} type: {}\n",
                                            destination.m_buffer,
                                            destination.m_location,
                                            static_cast<unsigned>(arg.m_type));
                    con::error("Technique {}: Could not find buffer for arg (buffer: {} offset: {} type: {}) in shader",
                               technique.m_name,
                               destination.m_buffer,
                               destination.m_location,
                               static_cast<unsigned>(arg.m_type));
                    return;
                }
                DumpShaderArg(technique, arg, boundResource->m_name);
            }
        }

        void DumpShaderArg(const CommonTechnique& technique, const CommonShaderArg& arg, std::string codeDestAccessor) const
        {
            if (arg.m_type == CommonShaderArgType::CODE_CONST)
            {
                const auto constSourceInfo = m_code_source_infos.GetInfoForCodeConstSource(arg.m_value.code_const_source);
                if (constSourceInfo)
                {
                    if (codeDestAccessor != constSourceInfo->accessor)
                    {
                        Indent();
                        m_stream << std::format("{} = constant.{};\n", codeDestAccessor, constSourceInfo->accessor);
                    }
                    else
                    {
#ifdef TECHSET_DEBUG
                        Indent();
                        m_stream << std::format("// Omitted due to matching accessors: {} = constant.{};\n", codeDestAccessor, constSourceInfo->accessor);
#endif
                    }
                }
                else
                {
                    assert(false);
                    Indent();
                    m_stream << std::format("// ERROR: Could not find code source info for const: {}\n", codeDestAccessor);
                    con::error("Technique {}: Could not find code source info for const {}", technique.m_name, codeDestAccessor);
                }
            }
            else if (arg.m_type == CommonShaderArgType::CODE_SAMPLER)
            {
                const auto samplerSourceInfo = m_code_source_infos.GetInfoForCodeSamplerSource(arg.m_value.code_sampler_source);
                if (samplerSourceInfo)
                {
                    if (codeDestAccessor != samplerSourceInfo->accessor)
                    {
                        Indent();
                        m_stream << std::format("{} = sampler.{};\n", codeDestAccessor, samplerSourceInfo->accessor);
                    }
                    else
                    {
#ifdef TECHSET_DEBUG
                        Indent();
                        m_stream << std::format("// Omitted due to matching accessors: {} = sampler.{};\n", codeDestAccessor, samplerSourceInfo->accessor);
#endif
                    }
                }
                else
                {
                    assert(false);
                    Indent();
                    m_stream << std::format("// ERROR: Could not find code source info for sampler: {}\n", codeDestAccessor);
                    con::error("Technique {}: Could not find code source info for sampler {}", technique.m_name, codeDestAccessor);
                }
            }
            else if (arg.m_type == CommonShaderArgType::LITERAL_CONST)
            {
                Indent();
                m_stream << std::format("{} = float4({}, {}, {}, {});\n",
                                        codeDestAccessor,
                                        arg.m_value.literal_value[0],
                                        arg.m_value.literal_value[1],
                                        arg.m_value.literal_value[2],
                                        arg.m_value.literal_value[3]);
            }
            else if (arg.m_type == CommonShaderArgType::MATERIAL_CONST || arg.m_type == CommonShaderArgType::MATERIAL_SAMPLER)
            {
                Indent();

                std::string materialPropertyName;
                if (m_constant_zone_state.GetConstantName(arg.m_value.name_hash, materialPropertyName)
                    || m_constant_zone_state.GetTextureDefName(arg.m_value.name_hash, materialPropertyName))
                {
                    m_stream << std::format("{} = material.{};\n", codeDestAccessor, materialPropertyName);
                }
                else if (m_constant_zone_state.HashString(codeDestAccessor) == arg.m_value.name_hash)
                {
                    m_stream << std::format("{} = material.{};\n", codeDestAccessor, codeDestAccessor);
                }
                else
                {
                    m_stream << std::format("{} = material.#0x{:x};\n", codeDestAccessor, arg.m_value.name_hash);
                }
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

        const CommonCodeSourceInfos& m_code_source_infos;
        const CommonStreamRoutingInfos& m_routing_infos;
        const AbstractMaterialConstantZoneState& m_constant_zone_state;
    };
} // namespace

namespace techset
{
    void DumpCommonTechnique(const AssetDumpingContext& context,
                             const CommonTechnique& technique,
                             const CommonCodeSourceInfos& codeSourceInfos,
                             const CommonStreamRoutingInfos& routingInfos,
                             const AbstractMaterialConstantZoneState& constantZoneState)
    {
        const auto techniqueFile = context.OpenAssetFile(GetFileNameForTechniqueName(technique.m_name));
        if (techniqueFile)
        {
            TechniqueFileWriter writer(*techniqueFile, codeSourceInfos, routingInfos, constantZoneState);
            writer.DumpTechnique(technique);
        }
    }
} // namespace techset
