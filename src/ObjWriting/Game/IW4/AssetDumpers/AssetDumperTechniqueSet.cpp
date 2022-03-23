#include "AssetDumperTechniqueSet.h"

#include <cassert>
#include <sstream>
#include <type_traits>

#include "Dumping/AbstractTextDumper.h"
#include "Game/IW4/TechsetConstantsIW4.h"

using namespace IW4;

namespace IW4
{
    class TechniqueDumpingZoneState final : public IZoneAssetDumperState
    {
        std::set<const MaterialTechnique*> m_dumped_techniques;

    public:
        bool ShouldDumpTechnique(const MaterialTechnique* technique)
        {
            if (m_dumped_techniques.find(technique) != m_dumped_techniques.end())
                return false;

            m_dumped_techniques.emplace(technique);
            return true;
        }
    };

    class TechniqueFileWriter : public AbstractTextDumper
    {
        void DumpStateMap() const
        {
            Indent();
            // TODO: Actual statemap: Maybe find all materials using this techset and try to make out rules for the flags based on the statebitstable
            m_stream << "stateMap \"\"; // TODO\n";
        }

        void DumpShaderArg(const MaterialShaderArgument& arg)
        {
            Indent();
            m_stream << "// Some arg dest:" << arg.dest << " type: " << arg.type << "\n";
        }

        void DumpVertexShader(const MaterialPass& pass)
        {
            if (pass.vertexShader == nullptr)
                return;

            m_stream << "\n";
            Indent();
            // TODO: Actually find out which version this shader uses
            m_stream << "vertexShader 1.0 \"" << pass.vertexShader->name << "\"\n";
            Indent();
            m_stream << "{\n";
            IncIndent();

            if (pass.args)
            {
                const auto totalArgCount = static_cast<size_t>(pass.perPrimArgCount)
                    + static_cast<size_t>(pass.perObjArgCount)
                    + static_cast<size_t>(pass.stableArgCount);
                for (auto i = 0u; i < totalArgCount; i++)
                {
                    const auto& arg = pass.args[i];
                    if (arg.type == MTL_ARG_MATERIAL_VERTEX_CONST
                        || arg.type == MTL_ARG_LITERAL_VERTEX_CONST
                        || arg.type == MTL_ARG_CODE_VERTEX_CONST)
                    {
                        DumpShaderArg(arg);
                    }
                }
            }

            DecIndent();
            Indent();
            m_stream << "}\n";
        }

        void DumpPixelShader(const MaterialPass& pass)
        {
            if (pass.pixelShader == nullptr)
                return;

            m_stream << "\n";
            Indent();
            // TODO: Actually find out which version this shader uses
            m_stream << "pixelShader 1.0 \"" << pass.pixelShader->name << "\"\n";
            Indent();
            m_stream << "{\n";
            IncIndent();

            if (pass.args)
            {
                const auto totalArgCount = static_cast<size_t>(pass.perPrimArgCount)
                    + static_cast<size_t>(pass.perObjArgCount)
                    + static_cast<size_t>(pass.stableArgCount);
                for (auto i = 0u; i < totalArgCount; i++)
                {
                    const auto& arg = pass.args[i];
                    if (arg.type == MTL_ARG_MATERIAL_PIXEL_SAMPLER
                        || arg.type == MTL_ARG_CODE_PIXEL_SAMPLER
                        || arg.type == MTL_ARG_CODE_PIXEL_CONST
                        || arg.type == MTL_ARG_MATERIAL_PIXEL_CONST
                        || arg.type == MTL_ARG_LITERAL_PIXEL_CONST)
                    {
                        DumpShaderArg(arg);
                    }
                }
            }

            DecIndent();
            Indent();
            m_stream << "}\n";
        }

        const char* GetStreamDestinationString(const MaterialStreamDestination_e dst)
        {
            static const char* destinationNames[]
            {
                "position",
                "normal",
                "color[0]",
                "color[1]",
                "depth",
                "texcoord[0]",
                "texcoord[1]",
                "texcoord[2]",
                "texcoord[3]",
                "texcoord[4]",
                "texcoord[5]",
                "texcoord[6]",
                "texcoord[7]",
            };
            static_assert(std::extent_v<decltype(destinationNames)> == STREAM_DST_COUNT);

            const auto dstIndex = static_cast<size_t>(dst);
            assert(dstIndex < std::extent_v<decltype(destinationNames)>);
            if (dstIndex < std::extent_v<decltype(destinationNames)>)
                return destinationNames[dstIndex];
            return "";
        }

        const char* GetStreamSourceString(const MaterialStreamStreamSource_e src)
        {
            static const char* sourceNames[]
            {
                "position",
                "color",
                "texcoord[0]",
                "normal",
                "tangent",
                "texcoord[1]",
                "texcoord[2]",
                "normalTransform[0]",
                "normalTransform[1]"
            };
            static_assert(std::extent_v<decltype(sourceNames)> == STREAM_SRC_COUNT);

            const auto srcIndex = static_cast<size_t>(src);
            assert(srcIndex < std::extent_v<decltype(sourceNames)>);
            if (srcIndex < std::extent_v<decltype(sourceNames)>)
                return sourceNames[srcIndex];
            return "";
        }

        void DumpVertexDecl(const MaterialPass& pass)
        {
            if (pass.vertexDecl == nullptr || pass.vertexDecl->streamCount <= 0)
                return;

            m_stream << "\n";

            const auto streamCount = std::min(static_cast<size_t>(pass.vertexDecl->streamCount), std::extent_v<decltype(MaterialVertexStreamRouting::data)>);
            for (auto streamIndex = 0u; streamIndex < streamCount; streamIndex++)
            {
                const auto& stream = pass.vertexDecl->routing.data[streamIndex];
                Indent();
                m_stream << "vertex." << GetStreamDestinationString(static_cast<MaterialStreamDestination_e>(stream.dest))
                    << " = code." << GetStreamSourceString(static_cast<MaterialStreamStreamSource_e>(stream.source)) << ";\n";
            }
        }

        void DumpPass(const MaterialPass& pass)
        {
            m_stream << "{\n";
            IncIndent();

            DumpStateMap();
            DumpVertexShader(pass);
            DumpPixelShader(pass);
            DumpVertexDecl(pass);

            DecIndent();
            m_stream << "}\n";
        }

    public:
        explicit TechniqueFileWriter(std::ostream& stream)
            : AbstractTextDumper(stream)
        {
        }

        void DumpTechnique(const MaterialTechnique* technique)
        {
            for (auto i = 0u; i < technique->passCount; i++)
                DumpPass(technique->passArray[i]);
        }
    };

    class TechsetFileWriter : public AbstractTextDumper
    {
        bool m_last_write_was_value;

    public:
        explicit TechsetFileWriter(std::ostream& stream)
            : AbstractTextDumper(stream),
              m_last_write_was_value(false)
        {
        }

        void WriteTechniqueType(const size_t techniqueIndex)
        {
            assert(techniqueIndex < std::extent_v<decltype(techniqueTypeNames)>);

            if (m_last_write_was_value)
            {
                m_stream << "\n";
                m_last_write_was_value = false;
            }
            m_stream << '"' << techniqueTypeNames[techniqueIndex] << "\":\n";
        }

        void WriteTechniqueValue(const char* value)
        {
            m_last_write_was_value = true;

            IncIndent();
            Indent();
            m_stream << value << ";\n";
            DecIndent();
        }

        void DumpTechset(const MaterialTechniqueSet* techset)
        {
            std::vector<bool> dumpedTechniques(std::extent_v<decltype(MaterialTechniqueSet::techniques)>);

            for (auto techniqueIndex = 0u; techniqueIndex < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; techniqueIndex++)
            {
                const auto* technique = techset->techniques[techniqueIndex];
                if (technique == nullptr || dumpedTechniques[techniqueIndex])
                    continue;

                dumpedTechniques[techniqueIndex] = true;
                WriteTechniqueType(techniqueIndex);

                for (auto nextTechniqueIndex = techniqueIndex + 1; nextTechniqueIndex < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; nextTechniqueIndex++)
                {
                    if (techset->techniques[nextTechniqueIndex] != technique)
                        continue;

                    dumpedTechniques[nextTechniqueIndex] = true;
                    WriteTechniqueType(nextTechniqueIndex);
                }

                WriteTechniqueValue(technique->name);
            }
        }
    };
}

std::string AssetDumperTechniqueSet::GetTechniqueFileName(const MaterialTechnique* technique)
{
    std::ostringstream ss;
    ss << "techniques/" << technique->name << ".tech";
    return ss.str();
}

std::string AssetDumperTechniqueSet::GetTechsetFileName(const MaterialTechniqueSet* techset)
{
    std::ostringstream ss;
    ss << "techsets/" << techset->name << ".techset";
    return ss.str();
}

bool AssetDumperTechniqueSet::ShouldDump(XAssetInfo<MaterialTechniqueSet>* asset)
{
    return true;
}

void AssetDumperTechniqueSet::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialTechniqueSet>* asset)
{
    const auto* techset = asset->Asset();

    const auto techsetFile = context.OpenAssetFile(GetTechsetFileName(techset));

    if (techsetFile)
    {
        TechsetFileWriter writer(*techsetFile);
        writer.DumpTechset(techset);
    }

    auto* techniqueState = context.GetZoneAssetDumperState<TechniqueDumpingZoneState>();
    for (const auto* technique : techset->techniques)
    {
        if (technique && techniqueState->ShouldDumpTechnique(technique))
        {
            const auto techniqueFile = context.OpenAssetFile(GetTechniqueFileName(technique));
            if (techniqueFile)
            {
                TechniqueFileWriter writer(*techniqueFile);
                writer.DumpTechnique(technique);
            }
        }
    }
}
