#pragma once

#include "Techset/CommonTechset.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace techset
{
    class TechsetParserState
    {
    public:
        TechsetParserState(std::string techsetName, const CommonTechniqueTypeNames& techniqueTypeNames);

        const CommonTechniqueTypeNames& m_technique_type_names;
        std::unique_ptr<CommonTechset> m_definition;
        std::vector<size_t> m_current_technique_types;
    };
} // namespace techset
