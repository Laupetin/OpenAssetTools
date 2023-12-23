#pragma once

#include "AbstractDirectiveStreamProxy.h"
#include "Parsing/IParserLineStream.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

#include <map>
#include <sstream>
#include <stack>

class DefinesStreamProxy final : public AbstractDirectiveStreamProxy
{
    static constexpr const char* DEFINE_DIRECTIVE = "define";
    static constexpr const char* UNDEF_DIRECTIVE = "undef";
    static constexpr const char* IF_DIRECTIVE = "if";
    static constexpr const char* ELIF_DIRECTIVE = "elif";
    static constexpr const char* IFDEF_DIRECTIVE = "ifdef";
    static constexpr const char* IFNDEF_DIRECTIVE = "ifndef";
    static constexpr const char* ELSE_DIRECTIVE = "else";
    static constexpr const char* ENDIF_DIRECTIVE = "endif";
    static constexpr const char* DEFINED_KEYWORD = "defined";

    static constexpr auto MAX_DEFINE_ITERATIONS = 128u;

public:
    class DefineParameterPosition
    {
    public:
        unsigned m_parameter_index;
        unsigned m_parameter_position;

        DefineParameterPosition();
        DefineParameterPosition(unsigned index, unsigned position);
    };

    class Define
    {
    public:
        std::string m_name;
        std::string m_value;
        std::vector<DefineParameterPosition> m_parameter_positions;

        Define();
        Define(std::string name, std::string value);
        void IdentifyParameters(const std::vector<std::string>& parameterNames);
        _NODISCARD std::string Render(const std::vector<std::string>& parameterValues) const;
    };

private:
    enum class BlockMode : uint8_t
    {
        NOT_IN_BLOCK,
        IN_BLOCK,
        BLOCK_BLOCKED
    };

    enum class ParameterState : uint8_t
    {
        NOT_IN_PARAMETERS,
        AFTER_OPEN,
        AFTER_PARAM,
        AFTER_COMMA
    };

    IParserLineStream* const m_stream;
    const bool m_skip_directive_lines;
    std::map<std::string, Define> m_defines;
    std::stack<BlockMode> m_modes;
    unsigned m_ignore_depth;

    bool m_in_define;
    ParameterState m_parameter_state;
    Define m_current_define;
    std::ostringstream m_current_define_value;
    std::vector<std::string> m_current_define_parameters;

    const Define* m_current_macro;
    ParameterState m_macro_parameter_state;
    std::vector<std::string> m_macro_parameters;
    std::ostringstream m_current_macro_parameter;
    std::stack<char> m_macro_bracket_depth;

    static int GetLineEndEscapePos(const ParserLine& line);
    void MatchDefineParameters(const ParserLine& line, unsigned& currentPos);
    void ContinueDefine(const ParserLine& line, unsigned currentPos);
    void ContinueParameters(const ParserLine& line, unsigned& currentPos);
    _NODISCARD bool MatchDefineDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchUndefDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchIfDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchElIfDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchIfdefDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchElseDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchEndifDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchDirectives(ParserLine& line);

    void ExtractParametersFromDefineUsage(const ParserLine& line, unsigned parameterStart, unsigned& parameterEnd);
    bool FindDefineForWord(const std::string& line, unsigned wordStart, unsigned wordEnd, const Define*& value) const;

    static bool MatchDefinedExpression(const ParserLine& line, unsigned& pos, std::string& definitionName);
    void ExpandDefinedExpressions(ParserLine& line) const;

    void ContinueMacroParameters(const ParserLine& line, unsigned& pos);
    void ContinueMacro(ParserLine& line);
    void ProcessDefine(const ParserLine& line, unsigned& pos, std::ostringstream& out);
    bool FindNextDefine(const std::string& line, unsigned& pos, unsigned& defineStart, const DefinesStreamProxy::Define*& define);

public:
    explicit DefinesStreamProxy(IParserLineStream* stream, bool skipDirectiveLines = false);

    void AddDefine(Define define);
    void Undefine(const std::string& name);

    void ExpandDefines(ParserLine& line);

    _NODISCARD std::unique_ptr<ISimpleExpression> ParseExpression(std::shared_ptr<std::string> fileName, int lineNumber, std::string expressionString);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
