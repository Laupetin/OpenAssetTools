#pragma once

#include "AbstractDirectiveStreamProxy.h"
#include "Parsing/IParserLineStream.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <vector>

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
        bool m_stringize;

        DefineParameterPosition();
        DefineParameterPosition(unsigned index, unsigned position, bool stringize);
    };

    class Define
    {
    public:
        std::string m_name;
        std::string m_value;
        std::vector<DefineParameterPosition> m_parameter_positions;
        bool m_contains_token_joining_operators;

        Define();
        Define(std::string name, std::string value);
        void IdentifyParameters(const std::vector<std::string>& parameterNames);

    private:
        void IdentifyTokenJoinsOnly();
    };

    enum class ParameterState : uint8_t
    {
        NOT_IN_PARAMETERS,
        AFTER_OPEN,
        AFTER_PARAM,
        AFTER_COMMA
    };

    class MacroParameterState
    {
    public:
        ParameterState m_parameter_state;
        std::ostringstream m_current_parameter;
        std::vector<std::string> m_parameters;
        std::stack<char> m_bracket_depth;

        MacroParameterState();
    };

private:
    enum class BlockMode : uint8_t

    {
        NOT_IN_BLOCK,
        IN_BLOCK,
        BLOCK_BLOCKED
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
    MacroParameterState m_multi_line_macro_parameters;

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

    void ExtractParametersFromMacroUsage(const ParserLine& line, unsigned& linePos, MacroParameterState& state, const std::string& input, unsigned& inputPos);
    bool FindMacroForIdentifier(const std::string& input, unsigned wordStart, unsigned wordEnd, const Define*& value) const;

    static bool MatchDefinedExpression(const ParserLine& line, unsigned& pos, std::string& definitionName);
    void ExpandDefinedExpressions(ParserLine& line) const;

    bool FindNextMacro(const std::string& input, unsigned& inputPos, unsigned& defineStart, const DefinesStreamProxy::Define*& define);

    void ProcessTokenJoiningOperators(ParserLine& line, unsigned& linePos, std::vector<const Define*>& callstack, std::string& input, unsigned& inputPos);
    void InsertMacroParameters(std::ostringstream& out, const DefinesStreamProxy::Define* macro, std::vector<std::string>& parameterValues);
    void ExpandMacro(ParserLine& line,
                     unsigned& linePos,
                     std::ostringstream& out,
                     std::vector<const Define*>& callstack,
                     const DefinesStreamProxy::Define* macro,
                     std::vector<std::string>& parameterValues);

    void ContinueMacroParameters(const ParserLine& line, unsigned& linePos, MacroParameterState& state, const std::string& input, unsigned& inputPos);
    void ContinueMultiLineMacro(ParserLine& line);

    void ProcessNestedMacros(ParserLine& line, unsigned& linePos, std::vector<const Define*>& callstack, std::string& input, unsigned& inputPos);
    void ProcessMacrosSingleLine(ParserLine& line);
    void ProcessMacrosMultiLine(ParserLine& line);

public:
    explicit DefinesStreamProxy(IParserLineStream* stream, bool skipDirectiveLines = false);

    void AddDefine(Define define);
    void Undefine(const std::string& name);

    _NODISCARD std::unique_ptr<ISimpleExpression> ParseExpression(std::shared_ptr<std::string> fileName, int lineNumber, std::string expressionString);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
