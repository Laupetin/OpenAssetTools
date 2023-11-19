#include "ZoneDefinitionParserValue.h"

#include <cassert>

ZoneDefinitionParserValue ZoneDefinitionParserValue::Invalid(const TokenPos pos)
{
    ZoneDefinitionParserValue pv(pos, ZoneDefinitionParserValueType::INVALID);
    return pv;
}

ZoneDefinitionParserValue ZoneDefinitionParserValue::EndOfFile(const TokenPos pos)
{
    ZoneDefinitionParserValue pv(pos, ZoneDefinitionParserValueType::END_OF_FILE);
    return pv;
}

ZoneDefinitionParserValue ZoneDefinitionParserValue::Character(const TokenPos pos, const char c)
{
    ZoneDefinitionParserValue pv(pos, ZoneDefinitionParserValueType::CHARACTER);
    pv.m_value.char_value = c;
    return pv;
}

ZoneDefinitionParserValue ZoneDefinitionParserValue::String(const TokenPos pos, std::string* str)
{
    ZoneDefinitionParserValue pv(pos, ZoneDefinitionParserValueType::STRING);
    pv.m_value.string_value = str;
    return pv;
}

ZoneDefinitionParserValue ZoneDefinitionParserValue::Field(const TokenPos pos, std::string* field)
{
    ZoneDefinitionParserValue pv(pos, ZoneDefinitionParserValueType::FIELD);
    pv.m_value.string_value = field;
    pv.m_hash = std::hash<std::string>()(*field);
    return pv;
}

ZoneDefinitionParserValue::ZoneDefinitionParserValue(const TokenPos pos, const ZoneDefinitionParserValueType type)
    : m_pos(pos),
      m_type(type),
      m_hash(0),
      m_value{}
{
}

ZoneDefinitionParserValue::~ZoneDefinitionParserValue()
{
    switch (m_type)
    {
    case ZoneDefinitionParserValueType::STRING:
    case ZoneDefinitionParserValueType::FIELD:
        delete m_value.string_value;
        break;

    default:
        break;
    }

    m_value = ValueType();
}

ZoneDefinitionParserValue::ZoneDefinitionParserValue(ZoneDefinitionParserValue&& other) noexcept
    : m_pos(other.m_pos),
      m_type(other.m_type),
      m_hash(other.m_hash),
      m_value(other.m_value)
{
    other.m_value = ValueType();
}

ZoneDefinitionParserValue& ZoneDefinitionParserValue::operator=(ZoneDefinitionParserValue&& other) noexcept
{
    m_pos = other.m_pos;
    m_type = other.m_type;
    m_value = other.m_value;
    m_hash = other.m_hash;
    other.m_value = ValueType();

    return *this;
}

bool ZoneDefinitionParserValue::IsEof() const
{
    return m_type == ZoneDefinitionParserValueType::END_OF_FILE;
}

const TokenPos& ZoneDefinitionParserValue::GetPos() const
{
    return m_pos;
}

char ZoneDefinitionParserValue::CharacterValue() const
{
    assert(m_type == ZoneDefinitionParserValueType::CHARACTER);
    return m_value.char_value;
}

std::string& ZoneDefinitionParserValue::StringValue() const
{
    assert(m_type == ZoneDefinitionParserValueType::STRING);
    return *m_value.string_value;
}

std::string& ZoneDefinitionParserValue::FieldValue() const
{
    assert(m_type == ZoneDefinitionParserValueType::FIELD);
    return *m_value.string_value;
}

size_t ZoneDefinitionParserValue::FieldHash() const
{
    assert(m_type == ZoneDefinitionParserValueType::FIELD);
    return m_hash;
}
