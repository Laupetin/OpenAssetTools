#pragma once

#include <map>
#include <stack>
#include <sstream>

#include "AbstractDirectiveStreamProxy.h"
#include "Parsing/IParserLineStream.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

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
        std::string Render(const std::vector<std::string>& parameterValues);
    };

private:
    enum class BlockMode
    {
        NOT_IN_BLOCK,
        IN_BLOCK,
        BLOCK_BLOCKED
    };

    IParserLineStream* const m_stream;
    std::map<std::string, Define> m_defines;
    std::stack<BlockMode> m_modes;
    unsigned m_ignore_depth;

    bool m_in_define;
    Define m_current_define;
    std::ostringstream m_current_define_value;
    std::vector<std::string> m_current_define_parameters;

    static int GetLineEndEscapePos(const ParserLine& line);
    static std::vector<std::string> MatchDefineParameters(const ParserLine& line, unsigned& parameterPosition);
    void ContinueDefine(const ParserLine& line);
    _NODISCARD bool MatchDefineDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchUndefDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchIfDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchElIfDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchIfdefDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchElseDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchEndifDirective(const ParserLine& line, unsigned directiveStartPosition, unsigned directiveEndPosition);
    _NODISCARD bool MatchDirectives(const ParserLine& line);

    static void ExtractParametersFromDefineUsage(const ParserLine& line, unsigned parameterStart, unsigned& parameterEnd, std::vector<std::string>& parameterValues);
    bool FindDefineForWord(const ParserLine& line, unsigned wordStart, unsigned wordEnd, Define*& value);

public:
    explicit DefinesStreamProxy(IParserLineStream* stream);

    void AddDefine(Define define);
    void Undefine(const std::string& name);

    void ExpandDefines(ParserLine& line);

    _NODISCARD std::unique_ptr<ISimpleExpression> ParseExpression(const std::string& expressionString) const;

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
