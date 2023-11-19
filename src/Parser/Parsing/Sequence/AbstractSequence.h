#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Matcher/MatcherAnd.h"
#include "Parsing/Matcher/MatcherLabel.h"
#include "SequenceResult.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <unordered_map>

template<typename TokenType, typename ParserState> class AbstractSequence : protected IMatcherForLabelSupplier<TokenType>
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

public:
    typedef AbstractMatcher<TokenType> matcher_t;

private:
    std::unique_ptr<matcher_t> m_entry;
    std::unordered_map<int, std::unique_ptr<matcher_t>> m_matchers;

protected:
    static constexpr int ENTRY_LABEL = 0;

    AbstractSequence() = default;

    virtual void ProcessMatch(ParserState* state, SequenceResult<TokenType>& result) const = 0;

    void AddMatchers(std::unique_ptr<matcher_t> matcher)
    {
        assert(!m_entry);
        m_entry = std::move(matcher);
    }

    void AddMatchers(std::initializer_list<Movable<std::unique_ptr<matcher_t>>> matchers)
    {
        assert(!m_entry);
        m_entry = std::make_unique<MatcherAnd<TokenType>>(matchers);
    }

    void AddLabeledMatchers(std::unique_ptr<matcher_t> matcher, const int label)
    {
        assert(m_matchers.find(label) == m_matchers.end());
        m_matchers.emplace(label, std::move(matcher));
    }

    void AddLabeledMatchers(std::initializer_list<Movable<std::unique_ptr<matcher_t>>> matchers, const int label)
    {
        assert(m_matchers.find(label) == m_matchers.end());
        m_matchers.emplace(label, std::make_unique<MatcherAnd<TokenType>>(matchers));
    }

public:
    ~AbstractSequence() override = default;
    AbstractSequence(const AbstractSequence& other) = default;
    AbstractSequence(AbstractSequence&& other) noexcept = default;
    AbstractSequence& operator=(const AbstractSequence& other) = default;
    AbstractSequence& operator=(AbstractSequence&& other) noexcept = default;

    _NODISCARD matcher_t* GetMatcherForLabel(const int label) const override
    {
        if (label == 0)
            return m_entry.get();

        const auto foundEntry = m_matchers.find(label);

        if (foundEntry != m_matchers.end())
            return foundEntry->second.get();

        return nullptr;
    }

    _NODISCARD bool MatchSequence(ILexer<TokenType>* lexer, ParserState* state, unsigned& consumedTokenCount) const
    {
        if (!m_entry)
            return false;

        auto result = m_entry->Match(lexer, 0);

        if (result.m_matches)
        {
            SequenceResult<TokenType> sequenceResult(lexer, result);
            ProcessMatch(state, sequenceResult);
            consumedTokenCount = result.m_consumed_token_count;
        }

        return result.m_matches;
    }
};
