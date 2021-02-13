#include "HeaderParser.h"

#include "Parsing/Header/Sequence/SequenceNamespace.h"

HeaderParser::HeaderParser(HeaderLexer* lexer, IDataRepository* targetRepository)
    : AbstractParser(lexer, std::make_unique<HeaderParserState>()),
      m_repository(targetRepository)
{
    auto sequenceNamespace = std::make_unique<SequenceNamespace>();
}

const std::vector<HeaderParser::sequence_t*>& HeaderParser::GetTestsForState()
{
    return m_state->GetBlock()->GetTestsForBlock();
}
