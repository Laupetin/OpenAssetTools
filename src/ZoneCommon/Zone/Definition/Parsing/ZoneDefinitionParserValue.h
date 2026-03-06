#pragma once

#include "Parsing/IParserValue.h"
#include "Parsing/TokenPos.h"

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

    [[nodiscard]] bool IsEof() const override;
    [[nodiscard]] const TokenPos& GetPos() const override;

    [[nodiscard]] char CharacterValue() const;
    [[nodiscard]] std::string& StringValue() const;
    [[nodiscard]] std::string& FieldValue() const;
    [[nodiscard]] size_t FieldHash() const;
};
