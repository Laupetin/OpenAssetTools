#pragma once

#include <unordered_map>
#include <stack>

#include "Parsing/IParserLineStream.h"

class DefinesStreamProxy final : public IParserLineStream
{
    static constexpr const char* DEFINE_DIRECTIVE = "define ";
    static constexpr const char* UNDEF_DIRECTIVE = "undef ";
    static constexpr const char* IFDEF_DIRECTIVE = "ifdef ";
    static constexpr const char* IFNDEF_DIRECTIVE = "ifndef ";
    static constexpr const char* ELSE_DIRECTIVE = "else";
    static constexpr const char* ENDIF_DIRECTIVE = "endif";

    std::unordered_map<std::string, std::string> m_defines;
    IParserLineStream* const m_stream;
    std::stack<bool> m_modes;
    unsigned m_ignore_depth;

    _NODISCARD static bool FindDirective(const ParserLine& line, unsigned& directivePosition);
    _NODISCARD bool MatchDefineDirective(const ParserLine& line, unsigned directivePosition);
    _NODISCARD bool MatchUndefDirective(const ParserLine& line, unsigned directivePosition);
    _NODISCARD bool MatchIfdefDirective(const ParserLine& line, unsigned directivePosition);
    _NODISCARD bool MatchElseDirective(const ParserLine& line, unsigned directivePosition);
    _NODISCARD bool MatchEndifDirective(const ParserLine& line, unsigned directivePosition);
    _NODISCARD bool MatchDirectives(const ParserLine& line);

    bool FindDefineForWord(const ParserLine& line, unsigned wordStart, unsigned wordEnd, std::string& value);
    void ExpandDefines(ParserLine& line);

public:
    explicit DefinesStreamProxy(IParserLineStream* stream);

    void AddDefine(const std::string& name, std::string value);
    void Undefine(const std::string& name);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
