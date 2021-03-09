#include "SimpleParserValue.h"

#include <cassert>

SimpleParserValue SimpleParserValue::Invalid(const TokenPos pos)
{
    SimpleParserValue pv(pos, SimpleParserValueType::INVALID);
    return pv;
}

SimpleParserValue SimpleParserValue::EndOfFile(const TokenPos pos)
{
    SimpleParserValue pv(pos, SimpleParserValueType::END_OF_FILE);
    return pv;
}

SimpleParserValue SimpleParserValue::Character(const TokenPos pos, const char c)
{
    SimpleParserValue pv(pos, SimpleParserValueType::CHARACTER);
    pv.m_value.char_value = c;
    return pv;
}

SimpleParserValue SimpleParserValue::Integer(const TokenPos pos, const int value)
{
    SimpleParserValue pv(pos, SimpleParserValueType::INTEGER);
    pv.m_value.int_value = value;
    return pv;
}

SimpleParserValue SimpleParserValue::FloatingPoint(const TokenPos pos, const double value)
{
    SimpleParserValue pv(pos, SimpleParserValueType::FLOATING_POINT);
    pv.m_value.double_value = value;
    return pv;
}

SimpleParserValue SimpleParserValue::String(const TokenPos pos, std::string* stringValue)
{
    SimpleParserValue pv(pos, SimpleParserValueType::STRING);
    pv.m_value.string_value = stringValue;
    return pv;
}

SimpleParserValue SimpleParserValue::Identifier(const TokenPos pos, std::string* identifier)
{
    SimpleParserValue pv(pos, SimpleParserValueType::IDENTIFIER);
    pv.m_value.string_value = identifier;
    pv.m_hash = std::hash<std::string>()(*identifier);
    return pv;
}

SimpleParserValue::SimpleParserValue(const TokenPos pos, const SimpleParserValueType type)
    : m_pos(pos),
      m_type(type),
      m_hash(0),
      m_value{}
{
}

SimpleParserValue::~SimpleParserValue()
{
    switch (m_type)
    {
    case SimpleParserValueType::STRING:
    case SimpleParserValueType::IDENTIFIER:
        delete m_value.string_value;
        break;

    default:
        break;
    }

    m_value = ValueType();
}

SimpleParserValue::SimpleParserValue(SimpleParserValue&& other) noexcept
    : m_pos(other.m_pos),
      m_type(other.m_type),
      m_hash(other.m_hash),
      m_value(other.m_value)
{
    other.m_value = ValueType();
}

SimpleParserValue& SimpleParserValue::operator=(SimpleParserValue&& other) noexcept
{
    m_pos = other.m_pos;
    m_type = other.m_type;
    m_value = other.m_value;
    m_hash = other.m_hash;
    other.m_value = ValueType();

    return *this;
}

bool SimpleParserValue::IsEof() const
{
    return m_type == SimpleParserValueType::END_OF_FILE;
}

const TokenPos& SimpleParserValue::GetPos() const
{
    return m_pos;
}

char SimpleParserValue::CharacterValue() const
{
    assert(m_type == SimpleParserValueType::CHARACTER);
    return m_value.char_value;
}

int SimpleParserValue::IntegerValue() const
{
    assert(m_type == SimpleParserValueType::INTEGER);
    return m_value.int_value;
}

double SimpleParserValue::FloatingPointValue() const
{
    assert(m_type == SimpleParserValueType::FLOATING_POINT);
    return m_value.double_value;
}

std::string& SimpleParserValue::StringValue() const
{
    assert(m_type == SimpleParserValueType::STRING);
    return *m_value.string_value;
}

std::string& SimpleParserValue::IdentifierValue() const
{
    assert(m_type == SimpleParserValueType::IDENTIFIER);
    return *m_value.string_value;
}

size_t SimpleParserValue::IdentifierHash() const
{
    assert(m_type == SimpleParserValueType::IDENTIFIER);
    return m_hash;
}
