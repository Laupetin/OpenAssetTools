#pragma once

#include <memory>
#include <stack>

#include "Utils/ClassUtils.h"
#include "Utils/NamespaceBuilder.h"
#include "Parsing/Header/Block/IHeaderBlock.h"

class IHeaderBlock;
class HeaderParserState
{
    std::stack<std::unique_ptr<IHeaderBlock>> m_blocks;

public:
    NamespaceBuilder m_namespace;

    HeaderParserState();

    _NODISCARD IHeaderBlock* GetBlock() const;
    void PushBlock(std::unique_ptr<IHeaderBlock> block);
    void PopBlock();
};
