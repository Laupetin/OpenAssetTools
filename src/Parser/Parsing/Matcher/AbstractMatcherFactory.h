#pragma once

#include "AbstractMatcher.h"
#include "MatcherAnd.h"
#include "MatcherFalse.h"
#include "MatcherLabel.h"
#include "MatcherLoop.h"
#include "MatcherOptional.h"
#include "MatcherOr.h"
#include "MatcherTrue.h"
#include "Parsing/IParserValue.h"
#include "Utils/ClassUtils.h"

#include <memory>

template<typename TokenType> class MatcherFactoryWrapper
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    std::unique_ptr<AbstractMatcher<TokenType>> m_matcher;

public:
    typedef typename AbstractMatcher<TokenType>::token_list_t token_list_t;

    explicit MatcherFactoryWrapper(std::unique_ptr<AbstractMatcher<TokenType>> matcher)
        : m_matcher(std::move(matcher))
    {
    }

    MatcherFactoryWrapper<TokenType>& Tag(const int tagId)
    {
        m_matcher->SetTag(tagId);
        return *this;
    }

    MatcherFactoryWrapper<TokenType>& Capture(const int captureId)
    {
        m_matcher->SetCapture(captureId);
        return *this;
    }

    MatcherFactoryWrapper<TokenType>& NoConsume()
    {
        m_matcher->SetConsume(false);
        return *this;
    }

    MatcherFactoryWrapper<TokenType>& Transform(std::function<TokenType(token_list_t&)> transform)
    {
        m_matcher->SetTransform(std::move(transform));
        return *this;
    }

    std::unique_ptr<AbstractMatcher<TokenType>> Build()
    {
        return std::move(m_matcher);
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator std::unique_ptr<AbstractMatcher<TokenType>>()
    {
        return Build();
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator Movable<std::unique_ptr<AbstractMatcher<TokenType>>>()
    {
        return Build();
    }
};

template<typename TokenType> class AbstractMatcherFactory
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    const IMatcherForLabelSupplier<TokenType>* m_label_supplier;

public:
    typedef typename AbstractMatcher<TokenType>::token_list_t token_list_t;

    explicit AbstractMatcherFactory(const IMatcherForLabelSupplier<TokenType>* labelSupplier)
        : m_label_supplier(labelSupplier)
    {
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> False() const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherFalse<TokenType>>());
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> True() const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherTrue<TokenType>>());
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> And(std::initializer_list<Movable<std::unique_ptr<AbstractMatcher<TokenType>>>> matchers) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherAnd<TokenType>>(matchers));
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> And(std::vector<std::unique_ptr<AbstractMatcher<TokenType>>> matchers) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherAnd<TokenType>>(std::move(matchers)));
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> Or(std::initializer_list<Movable<std::unique_ptr<AbstractMatcher<TokenType>>>> matchers) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherOr<TokenType>>(matchers));
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> Or(std::vector<std::unique_ptr<AbstractMatcher<TokenType>>> matchers) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherOr<TokenType>>(std::move(matchers)));
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> Loop(std::unique_ptr<AbstractMatcher<TokenType>> matcher) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherLoop<TokenType>>(std::move(matcher)));
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> OptionalLoop(std::unique_ptr<AbstractMatcher<TokenType>> matcher) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherOptional<TokenType>>(std::make_unique<MatcherLoop<TokenType>>(std::move(matcher))));
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> Optional(std::unique_ptr<AbstractMatcher<TokenType>> matcher) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherOptional<TokenType>>(std::move(matcher)));
    }

    _NODISCARD MatcherFactoryWrapper<TokenType> Label(const int label) const
    {
        return MatcherFactoryWrapper<TokenType>(std::make_unique<MatcherLabel<TokenType>>(m_label_supplier, label));
    }
};
