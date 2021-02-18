#pragma once

#include "IHeaderBlock.h"
#include "IHeaderBlockNameHolder.h"

class HeaderBlockStruct final : public IHeaderBlock, public IHeaderBlockNameHolder
{
    std::string m_variable_name;

public:
    HeaderBlockType GetType() override;
    const std::vector<sequence_t*>& GetTestsForBlock() override;
    void OnOpen(HeaderParserState* state) override;
    void OnClose(HeaderParserState* state) override;
    void OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) override;
    
    void SetBlockName(const TokenPos& nameTokenPos, std::string name) override;
};
