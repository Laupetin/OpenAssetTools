#pragma once

#include <set>

#include "AbstractDirectiveStreamProxy.h"
#include "Parsing/IParserLineStream.h"

class IncludingStreamProxy final : public AbstractDirectiveStreamProxy
{
    static constexpr const char* INCLUDE_QUOTES_ERROR = "Invalid include directive. Expected \"\" or <>";
    static constexpr const char* INCLUDE_DIRECTIVE = "include";
    static constexpr const char* PRAGMA_DIRECTIVE = "pragma";
    static constexpr const char* ONCE_PRAGMA_COMMAND = "once";

    IParserLineStream* const m_stream;
    std::set<std::string> m_included_files;
    
    _NODISCARD static bool ExtractIncludeFilename(const ParserLine& line, unsigned includeDirectivePosition, unsigned& filenameStartPosition, unsigned& filenameEndPosition);
    _NODISCARD bool MatchIncludeDirective(const ParserLine& line, unsigned directiveStartPos, unsigned directiveEndPos) const;
    _NODISCARD bool MatchPragmaOnceDirective(const ParserLine& line, unsigned directiveStartPos, unsigned directiveEndPos);
    _NODISCARD bool MatchDirectives(const ParserLine& line);

public:
    explicit IncludingStreamProxy(IParserLineStream* stream);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
