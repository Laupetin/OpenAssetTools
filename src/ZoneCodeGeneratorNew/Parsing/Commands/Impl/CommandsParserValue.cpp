#include "CommandsParserValue.h"

#include <cassert>

CommandsParserValue CommandsParserValue::Invalid(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::INVALID);
    return pv;
}

CommandsParserValue CommandsParserValue::EndOfFile(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::END_OF_FILE);
    return pv;
}

CommandsParserValue CommandsParserValue::Character(const TokenPos pos, const char c)
{
    CommandsParserValue pv(pos, c);
    return pv;
}

CommandsParserValue CommandsParserValue::ShiftLeft(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::SHIFT_LEFT);
    return pv;
}

CommandsParserValue CommandsParserValue::ShiftRight(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::SHIFT_RIGHT);
    return pv;
}

CommandsParserValue CommandsParserValue::Equals(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::EQUALS);
    return pv;
}

CommandsParserValue CommandsParserValue::NotEqual(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::NOT_EQUAL);
    return pv;
}

CommandsParserValue CommandsParserValue::GreaterEqual(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::GREATER_EQUAL);
    return pv;
}

CommandsParserValue CommandsParserValue::LessEqual(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::LESS_EQUAL);
    return pv;
}

CommandsParserValue CommandsParserValue::LogicalAnd(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::LOGICAL_AND);
    return pv;
}

CommandsParserValue CommandsParserValue::LogicalOr(const TokenPos pos)
{
    CommandsParserValue pv(pos, CommandsParserValueType::LOGICAL_OR);
    return pv;
}

CommandsParserValue CommandsParserValue::Integer(const TokenPos pos, const int value)
{
    CommandsParserValue pv(pos, CommandsParserValueType::INTEGER);
    pv.m_value.int_value = value;
    return pv;
}

CommandsParserValue CommandsParserValue::FloatingPoint(const TokenPos pos, const double value)
{
    CommandsParserValue pv(pos, CommandsParserValueType::FLOATING_POINT);
    pv.m_value.double_value = value;
    return pv;
}

CommandsParserValue CommandsParserValue::String(const TokenPos pos, std::string* stringValue)
{
    CommandsParserValue pv(pos, CommandsParserValueType::STRING);
    pv.m_value.string_value = stringValue;
    return pv;
}

CommandsParserValue CommandsParserValue::Identifier(const TokenPos pos, std::string* identifier)
{
    CommandsParserValue pv(pos, CommandsParserValueType::IDENTIFIER);
    pv.m_value.string_value = identifier;
    return pv;
}

CommandsParserValue CommandsParserValue::TypeName(const TokenPos pos, std::string* typeName)
{
    CommandsParserValue pv(pos, CommandsParserValueType::TYPE_NAME);
    pv.m_value.string_value = typeName;
    return pv;
}

CommandsParserValue::CommandsParserValue(const TokenPos pos, const int type)
    : m_pos(pos),
      m_type(type),
      m_value()
{
}

CommandsParserValue::~CommandsParserValue()
{
    switch (m_type)
    {
    case CommandsParserValueType::STRING:
    case CommandsParserValueType::IDENTIFIER:
    case CommandsParserValueType::TYPE_NAME:
        delete m_value.string_value;
        break;

    default:
        break;
    }

    m_value = ValueType();
}

CommandsParserValue::CommandsParserValue(CommandsParserValue&& other) noexcept
    : m_type(other.m_type),
      m_value(other.m_value)
{
    other.m_value = ValueType();
}

CommandsParserValue& CommandsParserValue::operator=(CommandsParserValue&& other) noexcept
{
    m_type = other.m_type;
    m_value = other.m_value;
    other.m_value = ValueType();

    return *this;
}

int CommandsParserValue::IntegerValue() const
{
    assert(m_type == CommandsParserValueType::INTEGER);
    return m_value.int_value;
}

double CommandsParserValue::FloatingPointValue() const
{
    assert(m_type == CommandsParserValueType::FLOATING_POINT);
    return m_value.double_value;
}

std::string& CommandsParserValue::StringValue() const
{
    assert(m_type == CommandsParserValueType::STRING);
    return *m_value.string_value;
}

std::string& CommandsParserValue::IdentifierValue() const
{
    assert(m_type == CommandsParserValueType::IDENTIFIER);
    return *m_value.string_value;
}

std::string& CommandsParserValue::TypeNameValue() const
{
    assert(m_type == CommandsParserValueType::TYPE_NAME);
    return *m_value.string_value;
}
