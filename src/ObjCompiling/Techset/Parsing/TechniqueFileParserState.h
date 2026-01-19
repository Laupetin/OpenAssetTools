#pragma once

#include "Techset/TechniqueDefinitionAcceptor.h"

namespace techset
{
    class TechniqueParserState
    {
    public:
        explicit TechniqueParserState(ITechniqueDefinitionAcceptor* acceptor);

        ITechniqueDefinitionAcceptor* const m_acceptor;

        bool m_in_pass;
        bool m_in_shader;
        ShaderSelector m_current_shader;
    };
} // namespace techset
