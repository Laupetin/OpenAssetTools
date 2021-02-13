#pragma once

#include "IHeaderBlock.h"

class HeaderBlockEnum final : public IHeaderBlock
{
public:
    HeaderBlockType GetType() override;
    const std::vector<sequence_t*>& GetTestsForBlock() override;
    void OnOpen() override;
    void OnClose() override;
    void OnChildBlockClose(IHeaderBlock* block) override;
};
