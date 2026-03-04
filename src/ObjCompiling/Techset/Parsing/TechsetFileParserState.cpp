#include "TechsetFileParserState.h"

namespace techset
{
    TechsetParserState::TechsetParserState(std::string techsetName, const CommonTechniqueTypeNames& techniqueTypeNames)
        : m_technique_type_names(techniqueTypeNames),
          m_definition(std::make_unique<CommonTechset>(std::move(techsetName), techniqueTypeNames.GetTechniqueTypeCount()))
    {
    }
} // namespace techset
