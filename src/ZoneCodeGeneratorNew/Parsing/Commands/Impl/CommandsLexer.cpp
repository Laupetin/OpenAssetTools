#include "CommandsLexer.h"

CommandsLexer::CommandsLexer(IParserLineStream* stream)
    : AbstractLexer(stream)
{
}

CommandsParserValue CommandsLexer::GetNextToken()
{
    return CommandsParserValue::Invalid(TokenPos());
}
