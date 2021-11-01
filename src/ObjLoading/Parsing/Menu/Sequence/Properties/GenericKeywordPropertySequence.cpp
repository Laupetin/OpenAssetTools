#include "GenericKeywordPropertySequence.h"

#include <utility>

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

GenericKeywordPropertySequence::GenericKeywordPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)),
    });
}

void GenericKeywordPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if(m_set_callback)
    {
        m_set_callback(state);
    }
}
