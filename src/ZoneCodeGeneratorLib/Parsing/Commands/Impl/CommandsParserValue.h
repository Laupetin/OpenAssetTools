#pragma once

#include "Domain/Evaluation/OperationType.h"
#include "Parsing/IParserValue.h"
#include "Parsing/TokenPos.h"
#include "Utils/ClassUtils.h"

#include <string>

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

    // Built-in types
    BUILT_IN_FIRST,
    UNSIGNED = BUILT_IN_FIRST,
    CHAR,
    SHORT,
    INT,
    LONG,
    BUILT_IN_LAST = LONG,

    // Generic token types
    INTEGER,
    FLOATING_POINT,
    STRING,
    IDENTIFIER,

    // Parser created
    TYPE_NAME,
    OPERATION_TYPE,

    // End
    MAX
};

class CommandsParserValue final : public IParserValue
{
public:
    union ValueType
    {
        char char_value;
        int int_value;
        double double_value;
        std::string* string_value;
        const OperationType* op_type_value;
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
    static CommandsParserValue OpType(TokenPos pos, const OperationType* operationType);

    CommandsParserValue(const CommandsParserValue& other) = delete;
    ~CommandsParserValue() override;
    CommandsParserValue(CommandsParserValue&& other) noexcept;
    CommandsParserValue& operator=(const CommandsParserValue& other) = delete;
    CommandsParserValue& operator=(CommandsParserValue&& other) noexcept;

    [[nodiscard]] bool IsEof() const override;
    [[nodiscard]] const TokenPos& GetPos() const override;

    [[nodiscard]] char CharacterValue() const;
    [[nodiscard]] int IntegerValue() const;
    [[nodiscard]] double FloatingPointValue() const;
    [[nodiscard]] std::string& StringValue() const;
    [[nodiscard]] std::string& IdentifierValue() const;
    [[nodiscard]] size_t IdentifierHash() const;
    [[nodiscard]] std::string& TypeNameValue() const;
    [[nodiscard]] const OperationType* OpTypeValue() const;

    TokenPos m_pos;
    CommandsParserValueType m_type;
    size_t m_hash;

private:
    CommandsParserValue(TokenPos pos, CommandsParserValueType type);
};
