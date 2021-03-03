#pragma once
#include "LoadingException.h"
#include "Zone/XBlock.h"

class BlockOverflowException final : public LoadingException
{
    XBlock* m_block;

public:
    explicit BlockOverflowException(XBlock* block);

    std::string DetailedMessage() override;
    char const* what() const noexcept override;
};
