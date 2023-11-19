#pragma once

#include "Parsing/IParserValue.h"
#include "Parsing/TokenPos.h"
#include "Utils/ClassUtils.h"

#include <string>

enum class ZoneDefinitionParserValueType
{
    // Meta tokens
    INVALID,
    END_OF_FILE,

    CHARACTER,
    STRING,
    FIELD,

    // End
    MAX
};

class ZoneDefinitionParserValue final : public IParserValue
{
public:
    TokenPos m_pos;
    ZoneDefinitionParserValueType m_type;
    size_t m_hash;

    union ValueType
    {
        char char_value;
        std::string* string_value;
    } m_value;

    static ZoneDefinitionParserValue Invalid(TokenPos pos);
    static ZoneDefinitionParserValue EndOfFile(TokenPos pos);
    static ZoneDefinitionParserValue Character(TokenPos pos, char c);
    static ZoneDefinitionParserValue String(TokenPos pos, std::string* str);
    static ZoneDefinitionParserValue Field(TokenPos pos, std::string* field);

private:
    ZoneDefinitionParserValue(TokenPos pos, ZoneDefinitionParserValueType type);

public:
    ~ZoneDefinitionParserValue() override;
    ZoneDefinitionParserValue(const ZoneDefinitionParserValue& other) = delete;
    ZoneDefinitionParserValue(ZoneDefinitionParserValue&& other) noexcept;
    ZoneDefinitionParserValue& operator=(const ZoneDefinitionParserValue& other) = delete;
    ZoneDefinitionParserValue& operator=(ZoneDefinitionParserValue&& other) noexcept;

    _NODISCARD bool IsEof() const override;
    _NODISCARD const TokenPos& GetPos() const override;

    _NODISCARD char CharacterValue() const;
    _NODISCARD std::string& StringValue() const;
    _NODISCARD std::string& FieldValue() const;
    _NODISCARD size_t FieldHash() const;
};
