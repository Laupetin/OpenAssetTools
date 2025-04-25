#pragma once

#include "IParserLineStream.h"
#include "Parsing/IParserValue.h"

#include <concepts>

template<std::derived_from<IParserValue> TokenType> class ILexer
{
public:
    ILexer() = default;
    virtual ~ILexer() = default;
    ILexer(const ILexer& other) = default;
    ILexer(ILexer&& other) noexcept = default;
    ILexer& operator=(const ILexer& other) = default;
    ILexer& operator=(ILexer&& other) noexcept = default;

    virtual const TokenType& GetToken(size_t index) = 0;
    virtual void PopTokens(size_t amount) = 0;

    [[nodiscard]] virtual bool IsEof() = 0;
    [[nodiscard]] virtual const TokenPos& GetPos() = 0;
    [[nodiscard]] virtual ParserLine GetLineForPos(const TokenPos& pos) const = 0;
};
