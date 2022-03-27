#pragma once

#include "Techset/TechniqueDefinitionAcceptor.h"

namespace techset
{
    class TechniqueParserState
    {
    public:
        ITechniqueDefinitionAcceptor* const m_acceptor;

        explicit TechniqueParserState(ITechniqueDefinitionAcceptor* acceptor);
    };
}
