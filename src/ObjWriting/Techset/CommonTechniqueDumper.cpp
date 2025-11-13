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
        explicit TechniqueFileWriter(std::ostream& stream)
            : AbstractTextDumper(stream)
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
            DumpShader(pass, pass.m_vertex_shader, TechniqueShaderType::VERTEX_SHADER);
            DumpShader(pass, pass.m_pixel_shader, TechniqueShaderType::PIXEL_SHADER);
            // DumpVertexDecl(pass);

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

        void DumpShader(const CommonPass& pass, const CommonTechniqueShader& shader, const TechniqueShaderType shaderType) const
        {
            if (!shader.m_shader_bin)
            {
                if (!shader.m_name.empty())
                    m_stream << std::format("// Cannot dump shader {} as its data is not loaded\n", shader.m_name);

                return;
            }

            unsigned versionMajor, versionMinor;
            if (pass.m_dx_version == DxVersion::DX9)
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
                assert(pass.m_dx_version == DxVersion::DX11);
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
    };
} // namespace

namespace techset
{
    void DumpCommonTechnique(const AssetDumpingContext& context, const CommonTechnique& technique)
    {
        const auto techniqueFile = context.OpenAssetFile(GetFileNameForTechniqueName(technique.m_name));
        if (techniqueFile)
        {
            TechniqueFileWriter writer(*techniqueFile);
            writer.DumpTechnique(technique);
        }
    }
} // namespace techset
