#pragma once

#include "IHeaderBlock.h"
#include "IHeaderBlockVariableDefining.h"

class HeaderBlockStruct final : public IHeaderBlock, public IHeaderBlockVariableDefining
{
    std::string m_variable_name;

public:
    HeaderBlockType GetType() override;
    const std::vector<sequence_t*>& GetTestsForBlock() override;
    void OnOpen(HeaderParserState* state) override;
    void OnClose(HeaderParserState* state) override;
    void OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) override;

    void SetVariableName(std::string name) override;
};
