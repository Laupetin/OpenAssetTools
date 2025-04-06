#pragma once

#include "Parsing/IParserLineStream.h"
#include "Parsing/TokenPos.h"

#include <cstddef>

class AbstractDirectiveStreamProxy : public IParserLineStream
{
protected:
    AbstractDirectiveStreamProxy() = default;

    static TokenPos CreatePos(const ParserLine& line, size_t position);

    static bool SkipWhitespace(const ParserLine& line, size_t& position);
    static bool ExtractInteger(const ParserLine& line, size_t& position, int& value);
    static bool ExtractIdentifier(const ParserLine& line, size_t& position);
    static bool MatchCharacter(const ParserLine& line, size_t& position, char c);
    static bool MatchNextCharacter(const ParserLine& line, size_t& position, char c);
    static bool MatchString(const ParserLine& line, size_t& position, const char* str, size_t len);
    static bool MatchNextString(const ParserLine& line, size_t& position, const char* str, size_t len);

    static bool FindDirective(const ParserLine& line, size_t& directiveStartPosition, size_t& directiveEndPos);
};
