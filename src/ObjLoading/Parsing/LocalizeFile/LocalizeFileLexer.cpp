#include "LocalizeFileLexer.h"

LocalizeFileLexer::LocalizeFileLexer(IParserLineStream* stream)
    : SimpleLexer(stream)
{
    SetShouldEmitNewLineTokens(true);
    SetShouldReadNumbers(false);
    SetShouldReadStrings(true);
}
