#pragma once

#include <memory>
#include <stack>

#include "Utils/ClassUtils.h"
#include "Parsing/Header/Block/IHeaderBlock.h"

class IHeaderBlock;
class HeaderParserState
{
    std::stack<std::unique_ptr<IHeaderBlock>> m_blocks;

public:
    HeaderParserState();

    _NODISCARD IHeaderBlock* GetBlock() const;
};
