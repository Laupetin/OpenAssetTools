#pragma once

#include <string>

#include "Parsing/IParserValue.h"
#include "Utils/ClassUtils.h"
#include "Parsing/TokenPos.h"

enum class SimpleParserValueType
{
    // Meta tokens
    INVALID,
    END_OF_FILE,
    NEW_LINE,

    // Single character
    CHARACTER,

    // Generic token types
    INTEGER,
    FLOATING_POINT,
    STRING,
    IDENTIFIER,

    // End
    MAX
};

class SimpleParserValue final : public IParserValue
{
public:
    TokenPos m_pos;
    SimpleParserValueType m_type;
    size_t m_hash;
    union ValueType
    {
        char char_value;
        int int_value;
        double double_value;
        std::string* string_value;
    } m_value;

    static SimpleParserValue Invalid(TokenPos pos);
    static SimpleParserValue EndOfFile(TokenPos pos);
    static SimpleParserValue NewLine(TokenPos pos);
    static SimpleParserValue Character(TokenPos pos, char c);
    static SimpleParserValue Integer(TokenPos pos, int value);
    static SimpleParserValue FloatingPoint(TokenPos pos, double value);
    static SimpleParserValue String(TokenPos pos, std::string* stringValue);
    static SimpleParserValue Identifier(TokenPos pos, std::string* identifier);

private:
    SimpleParserValue(TokenPos pos, SimpleParserValueType type);

public:
    ~SimpleParserValue() override;
    SimpleParserValue(const SimpleParserValue& other) = delete;
    SimpleParserValue(SimpleParserValue&& other) noexcept;
    SimpleParserValue& operator=(const SimpleParserValue& other) = delete;
    SimpleParserValue& operator=(SimpleParserValue&& other) noexcept;

    _NODISCARD bool IsEof() const override;
    _NODISCARD const TokenPos& GetPos() const override;

    _NODISCARD char CharacterValue() const;
    _NODISCARD int IntegerValue() const;
    _NODISCARD double FloatingPointValue() const;
    _NODISCARD std::string& StringValue() const;
    _NODISCARD std::string& IdentifierValue() const;
    _NODISCARD size_t IdentifierHash() const;
};
