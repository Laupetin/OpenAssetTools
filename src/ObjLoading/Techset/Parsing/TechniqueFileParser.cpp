#include "TechniqueFileParser.h"

#include "Sequence/TechniqueNoScopeSequences.h"
#include "Sequence/TechniquePassScopeSequences.h"
#include "Sequence/TechniqueShaderScopeSequences.h"

using namespace techset;

TechniqueParser::TechniqueParser(SimpleLexer* lexer, ITechniqueDefinitionAcceptor* acceptor)
    : AbstractParser(lexer, std::make_unique<TechniqueParserState>(acceptor))
{
}

const std::vector<TechniqueParser::sequence_t*>& TechniqueParser::GetTestsForState()
{
    if (m_state->m_in_shader)
        return TechniqueShaderScopeSequences::GetSequences();

    if (m_state->m_in_pass)
        return TechniquePassScopeSequences::GetSequences();

    return TechniqueNoScopeSequences::GetSequences();
}
