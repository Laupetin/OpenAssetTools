#include "AssetDumperTechniqueSet.h"

#include <sstream>

#include "Dumping/AbstractTextDumper.h"

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
    public:
        explicit TechniqueFileWriter(std::ostream& stream)
            : AbstractTextDumper(stream)
        {
        }

        void DumpTechnique(const MaterialTechnique* technique)
        {
            m_stream << "technique lol";
        }
    };

    class TechsetFileWriter : public AbstractTextDumper
    {
    public:
        explicit TechsetFileWriter(std::ostream& stream)
            : AbstractTextDumper(stream)
        {
        }

        void DumpTechset(const MaterialTechniqueSet* techset)
        {
            m_stream << "techset lol";
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
