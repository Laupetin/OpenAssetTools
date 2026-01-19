#include "TechsetFileParserState.h"

namespace techset
{
    TechsetParserState::TechsetParserState(const CommonTechniqueTypeNames& techniqueTypeNames)
        : m_technique_type_names(techniqueTypeNames),
          m_definition(std::make_unique<CommonTechset>(techniqueTypeNames.GetTechniqueTypeCount()))
    {
    }
} // namespace techset
