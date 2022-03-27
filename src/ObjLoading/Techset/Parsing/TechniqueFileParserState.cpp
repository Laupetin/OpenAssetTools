#include "TechniqueFileParserState.h"

#include <cassert>

using namespace techset;

TechniqueParserState::TechniqueParserState(ITechniqueDefinitionAcceptor* acceptor)
    : m_acceptor(acceptor),
      m_before_first_pass(true),
      m_in_pass(false),
      m_in_shader(false),
      m_current_shader(ShaderSelector::VERTEX_SHADER)
{
    assert(acceptor);
}
