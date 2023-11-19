#pragma once

#include <memory>
#include <vector>

template<typename ParserType> class AbstractScopeSequenceHolder
{
    std::vector<std::unique_ptr<typename ParserType::sequence_t>>& m_all_sequences;
    std::vector<typename ParserType::sequence_t*>& m_scope_sequences;

protected:
    AbstractScopeSequenceHolder(std::vector<std::unique_ptr<typename ParserType::sequence_t>>& allSequences,
                                std::vector<typename ParserType::sequence_t*>& scopeSequences)
        : m_all_sequences(allSequences),
          m_scope_sequences(scopeSequences)
    {
    }

    void AddSequence(std::unique_ptr<typename ParserType::sequence_t> test) const
    {
        m_scope_sequences.push_back(test.get());
        m_all_sequences.emplace_back(std::move(test));
    }

public:
    virtual ~AbstractScopeSequenceHolder() = default;
    AbstractScopeSequenceHolder(const AbstractScopeSequenceHolder& other) = delete;
    AbstractScopeSequenceHolder(AbstractScopeSequenceHolder&& other) noexcept = default;
    AbstractScopeSequenceHolder& operator=(const AbstractScopeSequenceHolder& other) = delete;
    AbstractScopeSequenceHolder& operator=(AbstractScopeSequenceHolder&& other) noexcept = delete;
};
