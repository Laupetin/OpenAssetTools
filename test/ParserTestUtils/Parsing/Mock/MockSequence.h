#pragma once

#include "Parsing/Sequence/AbstractSequence.h"

struct MockSequenceState
{
    char m_dummy;
};

template<typename TokenType> class MockSequence final : public AbstractSequence<TokenType, MockSequenceState>
{
public:
    typedef AbstractSequence<TokenType, MockSequenceState> parent_t;

private:
    using parent_t::AddLabeledMatchers;
    using parent_t::AddMatchers;

    std::function<void(SequenceResult<TokenType>&)> m_handler;

protected:
    void ProcessMatch(MockSequenceState* state, SequenceResult<TokenType>& result) const override
    {
        if (m_handler)
            m_handler(result);
    }

public:
    void AddMockMatchers(std::initializer_list<Movable<std::unique_ptr<typename parent_t::matcher_t>>> matchers)
    {
        AddMatchers(matchers);
    }

    void AddMockLabeledMatchers(std::initializer_list<Movable<std::unique_ptr<typename parent_t::matcher_t>>> matchers, const int label)
    {
        AddLabeledMatchers(matchers, label);
    }

    void Handle(std::function<void(SequenceResult<TokenType>&)> handler)
    {
        m_handler = std::move(handler);
    }

    IMatcherForLabelSupplier<TokenType>* GetLabelSupplier()
    {
        return this;
    }
};
