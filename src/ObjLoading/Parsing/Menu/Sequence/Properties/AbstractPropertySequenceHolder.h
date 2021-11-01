#pragma once

#include <memory>
#include <vector>

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class AbstractPropertySequenceHolder
    {
        std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& m_all_sequences;
        std::vector<MenuFileParser::sequence_t*>& m_scope_sequences;

    protected:
        AbstractPropertySequenceHolder(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequence(std::unique_ptr<MenuFileParser::sequence_t> test) const;

    public:
        virtual ~AbstractPropertySequenceHolder() = default;
        AbstractPropertySequenceHolder(const AbstractPropertySequenceHolder& other) = delete;
        AbstractPropertySequenceHolder(AbstractPropertySequenceHolder&& other) noexcept = default;
        AbstractPropertySequenceHolder& operator=(const AbstractPropertySequenceHolder& other) = delete;
        AbstractPropertySequenceHolder& operator=(AbstractPropertySequenceHolder&& other) noexcept = default;

        virtual void AddSequences(FeatureLevel featureLevel) = 0;
    };
}
