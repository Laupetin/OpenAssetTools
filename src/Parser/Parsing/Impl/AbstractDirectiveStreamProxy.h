#pragma once

#include "Parsing/IParserLineStream.h"
#include "Parsing/TokenPos.h"

class AbstractDirectiveStreamProxy : public IParserLineStream
{
protected:
    AbstractDirectiveStreamProxy() = default;

    static TokenPos CreatePos(const ParserLine& line, unsigned position);

    static bool SkipWhitespace(const ParserLine& line, unsigned& position);
    static bool ExtractInteger(const ParserLine& line, unsigned& position, int& value);
    static bool ExtractIdentifier(const ParserLine& line, unsigned& position);
    static bool MatchCharacter(const ParserLine& line, unsigned& position, char c);
    static bool MatchNextCharacter(const ParserLine& line, unsigned& position, char c);
    static bool MatchString(const ParserLine& line, unsigned& position, const char* str, unsigned len);
    static bool MatchNextString(const ParserLine& line, unsigned& position, const char* str, unsigned len);

    static bool FindDirective(const ParserLine& line, unsigned& directiveStartPosition, unsigned& directiveEndPos);
};
