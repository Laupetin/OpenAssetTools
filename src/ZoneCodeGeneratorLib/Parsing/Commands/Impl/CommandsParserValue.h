#pragma once

#include <string>


#include "Parsing/IParserValue.h"
#include "Utils/ClassUtils.h"
#include "Parsing/TokenPos.h"

enum class CommandsParserValueType
{
    // Meta tokens
    INVALID,
    END_OF_FILE,

    // Single character
    CHARACTER,

    // Symbol tokens
    SHIFT_LEFT,
    SHIFT_RIGHT,
    EQUALS,
    NOT_EQUAL,
    GREATER_EQUAL,
    LESS_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,

    // Generic token types
    INTEGER,
    FLOATING_POINT,
    STRING,
    IDENTIFIER,

    // Parser created
    TYPE_NAME,

    // End
    MAX
};

class CommandsParserValue final : public IParserValue
{
public:
    TokenPos m_pos;
    CommandsParserValueType m_type;
    size_t m_hash;
    union ValueType
    {
        char char_value;
        int int_value;
        double double_value;
        std::string* string_value;
    } m_value;

    static CommandsParserValue Invalid(TokenPos pos);
    static CommandsParserValue EndOfFile(TokenPos pos);
    static CommandsParserValue Character(TokenPos pos, char c);
    static CommandsParserValue ShiftLeft(TokenPos pos);
    static CommandsParserValue ShiftRight(TokenPos pos);
    static CommandsParserValue Equals(TokenPos pos);
    static CommandsParserValue NotEqual(TokenPos pos);
    static CommandsParserValue GreaterEqual(TokenPos pos);
    static CommandsParserValue LessEqual(TokenPos pos);
    static CommandsParserValue LogicalAnd(TokenPos pos);
    static CommandsParserValue LogicalOr(TokenPos pos);
    static CommandsParserValue Integer(TokenPos pos, int value);
    static CommandsParserValue FloatingPoint(TokenPos pos, double value);
    static CommandsParserValue String(TokenPos pos, std::string* stringValue);
    static CommandsParserValue Identifier(TokenPos pos, std::string* identifier);
    static CommandsParserValue TypeName(TokenPos pos, std::string* typeName);

private:
    CommandsParserValue(TokenPos pos, CommandsParserValueType type);

public:
    ~CommandsParserValue() override;
    CommandsParserValue(const CommandsParserValue& other) = delete;
    CommandsParserValue(CommandsParserValue&& other) noexcept;
    CommandsParserValue& operator=(const CommandsParserValue& other) = delete;
    CommandsParserValue& operator=(CommandsParserValue&& other) noexcept;

    _NODISCARD bool IsEof() const override;
    _NODISCARD const TokenPos& GetPos() const override;

    _NODISCARD char CharacterValue() const;
    _NODISCARD int IntegerValue() const;
    _NODISCARD double FloatingPointValue() const;
    _NODISCARD std::string& StringValue() const;
    _NODISCARD std::string& IdentifierValue() const;
    _NODISCARD size_t IdentifierHash() const;
    _NODISCARD std::string& TypeNameValue() const;
};