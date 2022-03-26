#include "TechsetFileParserState.h"

using namespace techset;

ParserState::ParserState(const char** validTechniqueTypeNames, size_t validTechniqueTypeNameCount)
    : m_definition(std::make_unique<TechsetDefinition>(validTechniqueTypeNameCount))
{
    for(auto i = 0u; i < validTechniqueTypeNameCount; i++)
    {
        m_valid_technique_type_names.emplace(std::make_pair(std::string(validTechniqueTypeNames[i]), i));
    }
}

bool ParserState::FindTechniqueTypeIndex(const std::string& techniqueTypeName, size_t& techniqueTypeIndex) const
{
    const auto foundTechniqueType = m_valid_technique_type_names.find(techniqueTypeName);

    if(foundTechniqueType != m_valid_technique_type_names.end())
    {
        techniqueTypeIndex = foundTechniqueType->second;
        return true;
    }

    return false;
}
