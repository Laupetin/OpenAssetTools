#include "HeaderParserValue.h"

#include <cassert>

HeaderParserValue HeaderParserValue::Invalid(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::INVALID);
    return pv;
}

HeaderParserValue HeaderParserValue::EndOfFile(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::END_OF_FILE);
    return pv;
}

HeaderParserValue HeaderParserValue::Character(const TokenPos pos, const char c)
{
    HeaderParserValue pv(pos, HeaderParserValueType::CHARACTER);
    pv.m_value.char_value = c;
    return pv;
}

HeaderParserValue HeaderParserValue::ShiftLeft(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::SHIFT_LEFT);
    return pv;
}

HeaderParserValue HeaderParserValue::ShiftRight(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::SHIFT_RIGHT);
    return pv;
}

HeaderParserValue HeaderParserValue::Equals(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::EQUALS);
    return pv;
}

HeaderParserValue HeaderParserValue::NotEqual(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::NOT_EQUAL);
    return pv;
}

HeaderParserValue HeaderParserValue::GreaterEqual(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::GREATER_EQUAL);
    return pv;
}

HeaderParserValue HeaderParserValue::LessEqual(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::LESS_EQUAL);
    return pv;
}

HeaderParserValue HeaderParserValue::LogicalAnd(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::LOGICAL_AND);
    return pv;
}

HeaderParserValue HeaderParserValue::LogicalOr(const TokenPos pos)
{
    HeaderParserValue pv(pos, HeaderParserValueType::LOGICAL_OR);
    return pv;
}

HeaderParserValue HeaderParserValue::Integer(const TokenPos pos, const int value)
{
    HeaderParserValue pv(pos, HeaderParserValueType::INTEGER);
    pv.m_value.int_value = value;
    return pv;
}

HeaderParserValue HeaderParserValue::FloatingPoint(const TokenPos pos, const double value)
{
    HeaderParserValue pv(pos, HeaderParserValueType::FLOATING_POINT);
    pv.m_value.double_value = value;
    return pv;
}

HeaderParserValue HeaderParserValue::String(const TokenPos pos, std::string* stringValue)
{
    HeaderParserValue pv(pos, HeaderParserValueType::STRING);
    pv.m_value.string_value = stringValue;
    return pv;
}

HeaderParserValue HeaderParserValue::Identifier(const TokenPos pos, std::string* identifier)
{
    HeaderParserValue pv(pos, HeaderParserValueType::IDENTIFIER);
    pv.m_value.string_value = identifier;
    return pv;
}

HeaderParserValue HeaderParserValue::TypeName(const TokenPos pos, std::string* typeName)
{
    HeaderParserValue pv(pos, HeaderParserValueType::TYPE_NAME);
    pv.m_value.string_value = typeName;
    return pv;
}

HeaderParserValue::HeaderParserValue(const TokenPos pos, const HeaderParserValueType type)
    : m_pos(pos),
    m_type(type),
    m_value()
{
}

HeaderParserValue::~HeaderParserValue()
{
    switch (m_type)
    {
    case HeaderParserValueType::STRING:
    case HeaderParserValueType::IDENTIFIER:
    case HeaderParserValueType::TYPE_NAME:
        delete m_value.string_value;
        break;

    default:
        break;
    }

    m_value = ValueType();
}

HeaderParserValue::HeaderParserValue(HeaderParserValue&& other) noexcept
    : m_type(other.m_type),
    m_value(other.m_value)
{
    other.m_value = ValueType();
}

HeaderParserValue& HeaderParserValue::operator=(HeaderParserValue&& other) noexcept
{
    m_type = other.m_type;
    m_value = other.m_value;
    other.m_value = ValueType();

    return *this;
}

char HeaderParserValue::CharacterValue() const
{
    assert(m_type == HeaderParserValueType::CHARACTER);
    return m_value.char_value;
}

int HeaderParserValue::IntegerValue() const
{
    assert(m_type == HeaderParserValueType::INTEGER);
    return m_value.int_value;
}

double HeaderParserValue::FloatingPointValue() const
{
    assert(m_type == HeaderParserValueType::FLOATING_POINT);
    return m_value.double_value;
}

std::string& HeaderParserValue::StringValue() const
{
    assert(m_type == HeaderParserValueType::STRING);
    return *m_value.string_value;
}

std::string& HeaderParserValue::IdentifierValue() const
{
    assert(m_type == HeaderParserValueType::IDENTIFIER);
    return *m_value.string_value;
}

std::string& HeaderParserValue::TypeNameValue() const
{
    assert(m_type == HeaderParserValueType::TYPE_NAME);
    return *m_value.string_value;
}
