#include "TechniqueFileParser.h"

using namespace techset;

TechniqueParser::TechniqueParser(SimpleLexer* lexer, ITechniqueDefinitionAcceptor* acceptor)
    : AbstractParser(lexer, std::make_unique<TechniqueParserState>(acceptor))
{
}

const std::vector<AbstractParser<SimpleParserValue, TechniqueParserState>::sequence_t*>& TechniqueParser::GetTestsForState()
{
    // TODO: Tests
    static std::vector<sequence_t*> tests({
    });

    return tests;
}
