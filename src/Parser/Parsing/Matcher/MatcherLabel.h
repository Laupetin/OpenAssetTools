#pragma once

#include "AbstractMatcher.h"
#include "Parsing/IParserValue.h"
#include "Utils/ClassUtils.h"

#include <cassert>

template<typename TokenType> class IMatcherForLabelSupplier
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

public:
    IMatcherForLabelSupplier() = default;
    virtual ~IMatcherForLabelSupplier() = default;
    IMatcherForLabelSupplier(const IMatcherForLabelSupplier& other) = default;
    IMatcherForLabelSupplier(IMatcherForLabelSupplier&& other) noexcept = default;
    IMatcherForLabelSupplier& operator=(const IMatcherForLabelSupplier& other) = default;
    IMatcherForLabelSupplier& operator=(IMatcherForLabelSupplier&& other) noexcept = default;

    _NODISCARD virtual AbstractMatcher<TokenType>* GetMatcherForLabel(int label) const = 0;
};

template<typename TokenType> class MatcherLabel final : public AbstractMatcher<TokenType>
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    const IMatcherForLabelSupplier<TokenType>* m_supplier;
    int m_label;

protected:
    MatcherResult<TokenType> CanMatch(ILexer<TokenType>* lexer, unsigned tokenOffset) override
    {
        AbstractMatcher<TokenType>* matcher = m_supplier->GetMatcherForLabel(m_label);

        assert(matcher);
        if (matcher)
            return matcher->Match(lexer, tokenOffset);

        return MatcherResult<TokenType>::NoMatch();
    }

public:
    MatcherLabel(const IMatcherForLabelSupplier<TokenType>* supplier, const int label)
        : m_supplier(supplier),
          m_label(label)
    {
    }
};
