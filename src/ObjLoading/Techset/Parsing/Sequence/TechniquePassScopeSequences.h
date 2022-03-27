#pragma once
#include <vector>

#include "Techset/Parsing/TechniqueFileParser.h"

namespace techset
{
    class TechniquePassScopeSequences
    {
        TechniquePassScopeSequences() = default;

    public:
        static const std::vector<TechniqueParser::sequence_t*>& GetSequences();
    };
}
