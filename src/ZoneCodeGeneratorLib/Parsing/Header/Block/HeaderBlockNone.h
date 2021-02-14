#pragma once

#include "IHeaderBlock.h"

class HeaderBlockNone final : public IHeaderBlock
{
public:
    HeaderBlockType GetType() override;
    const std::vector<sequence_t*>& GetTestsForBlock() override;
    void OnOpen(HeaderParserState* state) override;
    void OnClose(HeaderParserState* state) override;
    void OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) override;
};
