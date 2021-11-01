#include "AbstractPropertySequenceHolder.h"

using namespace menu;

AbstractPropertySequenceHolder::AbstractPropertySequenceHolder(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences)
    : m_all_sequences(allSequences),
      m_scope_sequences(scopeSequences)
{
}

void AbstractPropertySequenceHolder::AddSequence(std::unique_ptr<MenuFileParser::sequence_t> test) const
{
    m_scope_sequences.push_back(test.get());
    m_all_sequences.emplace_back(std::move(test));
}
