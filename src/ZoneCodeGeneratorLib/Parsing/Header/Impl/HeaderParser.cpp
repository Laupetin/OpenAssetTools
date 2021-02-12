#include "HeaderParser.h"

#include "Parsing/Header/Sequence/SequenceNamespace.h"

HeaderParser::HeaderParser(HeaderLexer* lexer, IDataRepository* targetRepository)
    : AbstractParser(lexer, std::make_unique<HeaderParserState>()),
      m_repository(targetRepository)
{
    auto sequenceNamespace = std::make_unique<SequenceNamespace>();
    m_normal_tests.push_back(sequenceNamespace.get());
    m_tests.emplace_back(std::move(sequenceNamespace));
}

const std::vector<HeaderParser::sequence_t*>& HeaderParser::GetTestsForState()
{
    return m_normal_tests;
}
