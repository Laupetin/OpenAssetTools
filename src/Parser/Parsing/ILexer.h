#pragma once

#include "IParserLineStream.h"
#include "Parsing/IParserValue.h"

template<typename TokenType>
class ILexer
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

public:
    ILexer() = default;
    virtual ~ILexer() = default;
    ILexer(const ILexer& other) = default;
    ILexer(ILexer&& other) noexcept = default;
    ILexer& operator=(const ILexer& other) = default;
    ILexer& operator=(ILexer&& other) noexcept = default;

    virtual const TokenType& GetToken(unsigned index) = 0;
    virtual void PopTokens(int amount) = 0;

    _NODISCARD virtual bool IsEof() = 0;
    _NODISCARD virtual const TokenPos& GetPos() = 0;
    _NODISCARD virtual ParserLine GetLineForPos(const TokenPos& pos) const = 0;
};
