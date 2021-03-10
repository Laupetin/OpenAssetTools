#include "ZoneDefinitionLexer.h"

ZoneDefinitionLexer::ZoneDefinitionLexer(IParserLineStream* stream)
    : SimpleLexer(stream)
{
    SetShouldEmitNewLineTokens(true);
    SetShouldReadStrings(true);
    SetShouldReadNumbers(false);
}