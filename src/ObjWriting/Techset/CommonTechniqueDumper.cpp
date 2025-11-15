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

        void DumpShader(const CommonTechniqueShader& shader, const TechniqueShaderType shaderType, const DxVersion dxVersion) const
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
            }

            const auto shaderTypeName = shaderType == TechniqueShaderType::VERTEX_SHADER ? "vertexShader" : "pixelShader";

            m_stream << "\n";
            Indent();
            m_stream << std::format("{} {}.{} \"{}\"\n", shaderTypeName, versionMajor, versionMinor, shader.m_name);
            Indent();
            m_stream << "{\n";

            Indent();
            m_stream << "}\n";
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
