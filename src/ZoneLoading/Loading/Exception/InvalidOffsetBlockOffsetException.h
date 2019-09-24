#pragma once
#include "LoadingException.h"
#include "Zone/XBlock.h"

class InvalidOffsetBlockOffsetException final : public LoadingException
{
    XBlock* m_referenced_block;
    size_t m_referenced_offset;

public:
    InvalidOffsetBlockOffsetException(XBlock* block, size_t referencedOffset);

    std::string DetailedMessage() override;
    char const* what() const override;
};
