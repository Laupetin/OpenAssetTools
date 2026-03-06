#pragma once

#include "Parsing/IParserValue.h"
#include "Parsing/TokenPos.h"

#include <string>

enum class SimpleParserValueType
{
    // Meta tokens
    INVALID,
    END_OF_FILE,
    NEW_LINE,

    // Character sequences
    CHARACTER,
    MULTI_CHARACTER,

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
    bool m_has_sign_prefix;

    union ValueType
    {
        char char_value;
        int int_value;
        int multi_character_sequence_id;
        double double_value;
        std::string* string_value;
    } m_value;

    static SimpleParserValue Invalid(TokenPos pos);
    static SimpleParserValue EndOfFile(TokenPos pos);
    static SimpleParserValue NewLine(TokenPos pos);
    static SimpleParserValue Character(TokenPos pos, char c);
    static SimpleParserValue MultiCharacter(TokenPos pos, int multiCharacterSequenceId);
    static SimpleParserValue Integer(TokenPos pos, int value);
    static SimpleParserValue Integer(TokenPos pos, int value, bool hasSignPrefix);
    static SimpleParserValue FloatingPoint(TokenPos pos, double value);
    static SimpleParserValue FloatingPoint(TokenPos pos, double value, bool hasSignPrefix);
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

    [[nodiscard]] bool IsEof() const override;
    [[nodiscard]] const TokenPos& GetPos() const override;

    [[nodiscard]] char CharacterValue() const;
    [[nodiscard]] int MultiCharacterValue() const;
    [[nodiscard]] int IntegerValue() const;
    [[nodiscard]] double FloatingPointValue() const;
    [[nodiscard]] std::string& StringValue() const;
    [[nodiscard]] std::string& IdentifierValue() const;
    [[nodiscard]] size_t IdentifierHash() const;
};
