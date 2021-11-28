#pragma once

#include <memory>
#include <vector>

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class AbstractScopeSequenceHolder
    {
        std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& m_all_sequences;
        std::vector<MenuFileParser::sequence_t*>& m_scope_sequences;

    protected:
        AbstractScopeSequenceHolder(std::vector<std::unique_ptr<MenuFileParser::sequence_t>>& allSequences, std::vector<MenuFileParser::sequence_t*>& scopeSequences);

        void AddSequence(std::unique_ptr<MenuFileParser::sequence_t> test) const;

    public:
        virtual ~AbstractScopeSequenceHolder() = default;
        AbstractScopeSequenceHolder(const AbstractScopeSequenceHolder& other) = delete;
        AbstractScopeSequenceHolder(AbstractScopeSequenceHolder&& other) noexcept = default;
        AbstractScopeSequenceHolder& operator=(const AbstractScopeSequenceHolder& other) = delete;
        AbstractScopeSequenceHolder& operator=(AbstractScopeSequenceHolder&& other) noexcept = delete;

        virtual void AddSequences(FeatureLevel featureLevel, bool permissive) = 0;
    };
}
