#include "HeaderParser.h"

#include "Parsing/Header/Sequence/SequenceNamespace.h"

HeaderParser::HeaderParser(HeaderLexer* lexer, const IPackValueSupplier* packValueSupplier)
    : AbstractParser(lexer, std::make_unique<HeaderParserState>(packValueSupplier))
{
    auto sequenceNamespace = std::make_unique<SequenceNamespace>();
}

const std::vector<HeaderParser::sequence_t*>& HeaderParser::GetTestsForState()
{
    return m_state->GetBlock()->GetTestsForBlock();
}

bool HeaderParser::SaveToRepository(IDataRepository* repository) const
{
    return m_state->SaveToRepository(repository);
}
