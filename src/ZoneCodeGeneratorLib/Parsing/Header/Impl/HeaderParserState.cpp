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

void HeaderParserState::PushBlock(std::unique_ptr<IHeaderBlock> block)
{
    m_blocks.emplace(std::move(block));
    m_blocks.top()->OnOpen(this);
}

void HeaderParserState::PopBlock()
{
    // Leave at least one block on the stack which should be the only "none" block
    if (m_blocks.size() > 1)
    {
        std::unique_ptr<IHeaderBlock> poppedBlock = std::move(m_blocks.top());
        m_blocks.pop();
        poppedBlock->OnClose(this);
        m_blocks.top()->OnChildBlockClose(this, poppedBlock.get());
    }
}
