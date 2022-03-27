#include "TechniqueFileParserState.h"

#include <cassert>

using namespace techset;

TechniqueParserState::TechniqueParserState(ITechniqueDefinitionAcceptor* acceptor)
    : m_acceptor(acceptor)
{
    assert(acceptor);
}
