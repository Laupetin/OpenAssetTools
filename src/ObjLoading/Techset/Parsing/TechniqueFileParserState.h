#pragma once

#include "Techset/TechniqueDefinitionAcceptor.h"

namespace techset
{
    class TechniqueParserState
    {
    public:
        ITechniqueDefinitionAcceptor* const m_acceptor;

        bool m_before_first_pass;
        bool m_in_pass;
        bool m_in_shader;
        ShaderSelector m_current_shader;

        explicit TechniqueParserState(ITechniqueDefinitionAcceptor* acceptor);
    };
}
