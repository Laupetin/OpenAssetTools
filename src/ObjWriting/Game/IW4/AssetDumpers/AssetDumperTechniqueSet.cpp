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

            // TODO: Dump vertex shader args

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

            // TODO: Dump pixel shader args

            DecIndent();
            Indent();
            m_stream << "}\n";
        }

        void DumpVertexDecl(const MaterialPass& pass)
        {
            // TODO
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

            if(m_last_write_was_value)
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

            for(auto techniqueIndex = 0u; techniqueIndex < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; techniqueIndex++)
            {
                const auto* technique = techset->techniques[techniqueIndex];
                if(technique == nullptr || dumpedTechniques[techniqueIndex])
                    continue;

                dumpedTechniques[techniqueIndex] = true;
                WriteTechniqueType(techniqueIndex);

                for(auto nextTechniqueIndex = techniqueIndex + 1; nextTechniqueIndex < std::extent_v<decltype(MaterialTechniqueSet::techniques)>; nextTechniqueIndex++)
                {
                    if(techset->techniques[nextTechniqueIndex] != technique)
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
