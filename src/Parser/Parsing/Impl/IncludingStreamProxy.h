#pragma once

#include "AbstractDirectiveStreamProxy.h"
#include "Parsing/IParserLineStream.h"

#include <set>

class IncludingStreamProxy final : public AbstractDirectiveStreamProxy
{
    static constexpr const char* INCLUDE_QUOTES_ERROR = "Invalid include directive. Expected \"\" or <>";
    static constexpr const char* INCLUDE_DIRECTIVE = "include";
    static constexpr const char* PRAGMA_DIRECTIVE = "pragma";
    static constexpr const char* ONCE_PRAGMA_COMMAND = "once";

    IParserLineStream* const m_stream;
    std::set<std::string> m_included_files;

    [[nodiscard]] static bool
        ExtractIncludeFilename(const ParserLine& line, size_t includeDirectivePosition, size_t& filenameStartPosition, size_t& filenameEndPosition);
    [[nodiscard]] bool MatchIncludeDirective(const ParserLine& line, size_t directiveStartPos, size_t directiveEndPos) const;
    [[nodiscard]] bool MatchPragmaOnceDirective(const ParserLine& line, size_t directiveStartPos, size_t directiveEndPos);
    [[nodiscard]] bool MatchDirectives(const ParserLine& line);

public:
    explicit IncludingStreamProxy(IParserLineStream* stream);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    [[nodiscard]] bool IsOpen() const override;
    [[nodiscard]] bool Eof() const override;
};
