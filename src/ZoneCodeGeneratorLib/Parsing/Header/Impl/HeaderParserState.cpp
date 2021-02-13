#include "HeaderParserState.h"

#include "Parsing/Header/Block/HeaderBlockNone.h"

HeaderParserState::HeaderParserState()
{
    m_blocks.push(std::make_unique<HeaderBlockNone>());
}

IHeaderBlock* HeaderParserState::GetBlock() const
{
    return m_blocks.top().get();
}
