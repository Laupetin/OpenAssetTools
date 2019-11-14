#pragma once
#include "LoadingException.h"
#include "Zone/XBlock.h"

class OutOfBlockBoundsException final : public LoadingException
{
    XBlock* m_block;

public:
    explicit OutOfBlockBoundsException(XBlock* block);

    std::string DetailedMessage() override;
    char const* what() const override;
};
