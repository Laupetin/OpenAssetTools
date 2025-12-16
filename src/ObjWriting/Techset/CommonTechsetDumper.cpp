#include "CommonTechsetDumper.h"

#include "Dumping/AbstractTextDumper.h"
#include "Techset/TechsetCommon.h"

#include <cassert>

using namespace techset;

namespace
{
    class TechsetFileWriter : public AbstractTextDumper
    {
    public:
        TechsetFileWriter(const CommonTechniqueTypeNames& techniqueTypeNames, std::ostream& stream)
            : AbstractTextDumper(stream),
              m_last_write_was_value(false),
              m_technique_type_names(techniqueTypeNames)
        {
        }

        void DumpTechset(const CommonTechset& techset)
        {
            const auto techniqueCount = m_technique_type_names.GetTechniqueTypeCount();
            assert(techset.m_technique_names.size() == techniqueCount);

            std::vector<bool> dumpedTechniques(techniqueCount);

            for (auto techniqueIndex = 0u; techniqueIndex < techniqueCount; techniqueIndex++)
            {
                const auto& technique = techset.m_technique_names[techniqueIndex];
                if (technique.empty() || dumpedTechniques[techniqueIndex])
                    continue;

                dumpedTechniques[techniqueIndex] = true;
                WriteTechniqueType(techniqueIndex);

                for (auto nextTechniqueIndex = techniqueIndex + 1; nextTechniqueIndex < techniqueCount; nextTechniqueIndex++)
                {
                    if (techset.m_technique_names[nextTechniqueIndex] != technique)
                        continue;

                    dumpedTechniques[nextTechniqueIndex] = true;
                    WriteTechniqueType(nextTechniqueIndex);
                }

                WriteTechniqueValue(technique);
            }
        }

    private:
        void WriteTechniqueType(const size_t techniqueIndex)
        {
            if (m_last_write_was_value)
            {
                m_stream << "\n";
                m_last_write_was_value = false;
            }

            m_stream << '"' << m_technique_type_names.GetTechniqueTypeName(techniqueIndex) << "\":\n";
        }

        void WriteTechniqueValue(const std::string& value)
        {
            m_last_write_was_value = true;

            IncIndent();
            Indent();
            m_stream << value << ";\n";
            DecIndent();
        }

        bool m_last_write_was_value;
        const CommonTechniqueTypeNames& m_technique_type_names;
    };
} // namespace

namespace techset
{
    void DumpCommonTechset(const CommonTechniqueTypeNames& techniqueTypeNames, const AssetDumpingContext& context, const CommonTechset& techset)
    {
        const auto techsetFile = context.OpenAssetFile(GetFileNameForTechsetName(techset.m_name));

        if (techsetFile)
        {
            TechsetFileWriter writer(techniqueTypeNames, *techsetFile);
            writer.DumpTechset(techset);
        }
    }
} // namespace techset
