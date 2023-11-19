#pragma once

#include "Parsing/IParserValue.h"
#include "Parsing/TokenPos.h"
#include "Utils/ClassUtils.h"

#include <string>

enum class HeaderParserValueType
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

    // Built-in types
    BUILT_IN_FIRST,
    UNSIGNED = BUILT_IN_FIRST,
    CHAR,
    SHORT,
    INT,
    LONG,
    BUILT_IN_LAST = LONG,

    // Keywords
    DECLSPEC,
    ALIGN,
    ALIGNAS,
    CONST,
    ENUM,
    NAMESPACE,
    STRUCT,
    TYPEDEF,
    UNION,

    // Parser created
    TYPE_NAME,

    // End
    MAX
};

class HeaderParserValue final : public IParserValue
{
public:
    TokenPos m_pos;
    HeaderParserValueType m_type;

    union ValueType
    {
        char char_value;
        int int_value;
        double double_value;
        std::string* string_value;
    } m_value;

    static HeaderParserValue Invalid(TokenPos pos);
    static HeaderParserValue EndOfFile(TokenPos pos);
    static HeaderParserValue Character(TokenPos pos, char c);
    static HeaderParserValue ShiftLeft(TokenPos pos);
    static HeaderParserValue ShiftRight(TokenPos pos);
    static HeaderParserValue Equals(TokenPos pos);
    static HeaderParserValue NotEqual(TokenPos pos);
    static HeaderParserValue GreaterEqual(TokenPos pos);
    static HeaderParserValue LessEqual(TokenPos pos);
    static HeaderParserValue LogicalAnd(TokenPos pos);
    static HeaderParserValue LogicalOr(TokenPos pos);
    static HeaderParserValue Integer(TokenPos pos, int value);
    static HeaderParserValue FloatingPoint(TokenPos pos, double value);
    static HeaderParserValue String(TokenPos pos, std::string* stringValue);
    static HeaderParserValue Identifier(TokenPos pos, std::string* identifier);
    static HeaderParserValue Keyword(TokenPos pos, HeaderParserValueType type);
    static HeaderParserValue TypeName(TokenPos pos, std::string* typeName);

private:
    HeaderParserValue(TokenPos pos, HeaderParserValueType type);

public:
    ~HeaderParserValue() override;
    HeaderParserValue(const HeaderParserValue& other) = delete;
    HeaderParserValue(HeaderParserValue&& other) noexcept;
    HeaderParserValue& operator=(const HeaderParserValue& other) = delete;
    HeaderParserValue& operator=(HeaderParserValue&& other) noexcept;

    _NODISCARD bool IsEof() const override;
    _NODISCARD const TokenPos& GetPos() const override;

    _NODISCARD char CharacterValue() const;
    _NODISCARD int IntegerValue() const;
    _NODISCARD double FloatingPointValue() const;
    _NODISCARD std::string& StringValue() const;
    _NODISCARD std::string& IdentifierValue() const;
    _NODISCARD std::string& TypeNameValue() const;
};
