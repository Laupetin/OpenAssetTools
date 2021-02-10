#pragma once

#include "Parsing/IParserLineStream.h"

class IncludingStreamProxy final : public IParserLineStream
{
    static constexpr const char* INCLUDE_QUOTES_ERROR = "Invalid include directive. Expected \"\" or <>";
    static constexpr const char* INCLUDE_DIRECTIVE = "include ";
    static constexpr int INCLUDE_DIRECTIVE_LENGTH = std::char_traits<char>::length(INCLUDE_DIRECTIVE);
    static constexpr int INCLUDE_DIRECTIVE_MINIMUM_TOTAL_LENGTH = INCLUDE_DIRECTIVE_LENGTH + 1 + 2; // #=+1 ""=+2

    IParserLineStream* m_stream;

    bool MatchIncludeDirective(const ParserLine& line) const;

public:
    explicit IncludingStreamProxy(IParserLineStream* stream);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
