#include "AbstractMenuDumper.h"

#include <algorithm>
#include <sstream>
#include <cmath>

#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"

AbstractMenuDumper::AbstractMenuDumper(std::ostream& stream)
    : m_stream(stream),
      m_indent(0u)
{
}

void AbstractMenuDumper::IncIndent()
{
    m_indent++;
}

void AbstractMenuDumper::DecIndent()
{
    if (m_indent > 0)
        m_indent--;
}

void AbstractMenuDumper::Indent() const
{
    for (auto i = 0u; i < m_indent; i++)
        m_stream << "    ";
}

void AbstractMenuDumper::StartScope(const std::string& scopeName)
{
    Indent();
    m_stream << scopeName << "\n";
    Indent();
    m_stream << "{\n";
    IncIndent();
}

void AbstractMenuDumper::StartMenuDefScope()
{
    StartScope("menuDef");
}

void AbstractMenuDumper::StartItemDefScope()
{
    StartScope("itemDef");
}

void AbstractMenuDumper::StartFunctionDefScope()
{
    StartScope("functionDef");
}

void AbstractMenuDumper::EndScope()
{
    DecIndent();
    Indent();
    m_stream << "}\n";
}

std::vector<std::string> AbstractMenuDumper::CreateScriptTokenList(const char* script)
{
    const std::string scriptString(script);
    std::istringstream stringStream(scriptString);
    ParserSingleInputStream inputStream(stringStream, "MenuScript");

    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_strings = true;
    lexerConfig.m_read_numbers = false;
    SimpleLexer lexer(&inputStream, std::move(lexerConfig));

    std::vector<std::string> result;
    auto hasLexerTokens = true;
    while (hasLexerTokens)
    {
        const auto& token = lexer.GetToken(0);
        switch (token.m_type)
        {
        case SimpleParserValueType::IDENTIFIER:
            result.emplace_back(token.IdentifierValue());
            break;

        case SimpleParserValueType::STRING:
            result.emplace_back(token.StringValue());
            break;

        case SimpleParserValueType::CHARACTER:
            result.emplace_back(1, token.CharacterValue());
            break;

        case SimpleParserValueType::INVALID:
        case SimpleParserValueType::END_OF_FILE:
            hasLexerTokens = false;
            break;

        default:
            assert(false);
            break;
        }

        lexer.PopTokens(1);
    }

    return result;
}

bool AbstractMenuDumper::DoesTokenNeedQuotationMarks(const std::string& token)
{
    const auto hasAlNumCharacter = std::any_of(token.begin(), token.end(), [](const char& c)
    {
        return isalnum(c);
    });

    if (!hasAlNumCharacter)
        return false;

    const auto hasNonIdentifierCharacter = std::any_of(token.begin(), token.end(), [](const char& c)
    {
        return !isalnum(c) && c != '_';
    });

    return hasNonIdentifierCharacter;
}

const std::string& AbstractMenuDumper::BoolValue(const bool value)
{
    return value ? BOOL_VALUE_TRUE : BOOL_VALUE_FALSE;
}

void AbstractMenuDumper::WriteKey(const std::string& keyName) const
{
    m_stream << keyName;

    if (keyName.size() < MENU_KEY_SPACING)
    {
        const auto spacingLength = MENU_KEY_SPACING - keyName.size();
        for (auto i = 0u; i < spacingLength; i++)
            m_stream << " ";
    }
}

void AbstractMenuDumper::WriteStringProperty(const std::string& propertyKey, const std::string& propertyValue) const
{
    if (propertyValue.empty())
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << "\"" << propertyValue << "\"\n";
}

void AbstractMenuDumper::WriteStringProperty(const std::string& propertyKey, const char* propertyValue) const
{
    if (propertyValue == nullptr || propertyValue[0] == '\0')
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << "\"" << propertyValue << "\"\n";
}

void AbstractMenuDumper::WriteBoolProperty(const std::string& propertyKey, const bool propertyValue, const bool defaultValue) const
{
    if (propertyValue == defaultValue)
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << BoolValue(propertyValue) << "\n";
}

void AbstractMenuDumper::WriteIntProperty(const std::string& propertyKey, const int propertyValue, const int defaultValue) const
{
    if (propertyValue == defaultValue)
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << propertyValue << "\n";
}

void AbstractMenuDumper::WriteFloatProperty(const std::string& propertyKey, const float propertyValue, const float defaultValue) const
{
    if (std::fabs(propertyValue - defaultValue) < std::numeric_limits<float>::epsilon())
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << propertyValue << "\n";
}

void AbstractMenuDumper::WriteColorProperty(const std::string& propertyKey, const float (&propertyValue)[4], const float (&defaultValue)[4]) const
{
    if (std::fabs(propertyValue[0] - defaultValue[0]) < std::numeric_limits<float>::epsilon()
        && std::fabs(propertyValue[1] - defaultValue[1]) < std::numeric_limits<float>::epsilon()
        && std::fabs(propertyValue[2] - defaultValue[2]) < std::numeric_limits<float>::epsilon()
        && std::fabs(propertyValue[3] - defaultValue[3]) < std::numeric_limits<float>::epsilon())
    {
        return;
    }

    Indent();
    WriteKey(propertyKey);
    m_stream << propertyValue[0] << " " << propertyValue[1] << " " << propertyValue[2] << " " << propertyValue[3] << "\n";
}

void AbstractMenuDumper::WriteKeywordProperty(const std::string& propertyKey, const bool shouldWrite) const
{
    if (!shouldWrite)
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << "\n";
}

void AbstractMenuDumper::WriteFlagsProperty(const std::string& propertyKey, const int flagsValue) const
{
    for (auto i = 0u; i < sizeof(flagsValue) * 8; i++)
    {
        if (flagsValue & (1 << i))
        {
            Indent();
            WriteKey(propertyKey);
            m_stream << i << "\n";
        }
    }
}

void AbstractMenuDumper::Start()
{
    Indent();
    m_stream << "{\n";
    IncIndent();
}

void AbstractMenuDumper::End()
{
    for (auto i = 0u; i < m_indent; i++)
    {
        DecIndent();
        Indent();
        m_stream << "}\n";
    }
}

void AbstractMenuDumper::IncludeMenu(const std::string& menuPath) const
{
    Indent();
    m_stream << "loadMenu { \"" << menuPath << "\" }\n";
}
