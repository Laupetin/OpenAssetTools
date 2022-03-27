#pragma once

#include <map>
#include <memory>
#include <string>

#include "Techset/TechsetDefinition.h"

namespace techset
{
    class TechsetParserState
    {
    public:
        std::map<std::string, size_t> m_valid_technique_type_names;
        std::unique_ptr<TechsetDefinition> m_definition;
        std::vector<size_t> m_current_technique_types;

        TechsetParserState(const char** validTechniqueTypeNames, size_t validTechniqueTypeNameCount);

        bool FindTechniqueTypeIndex(const std::string& techniqueTypeName, size_t& techniqueTypeIndex) const;
    };
}
