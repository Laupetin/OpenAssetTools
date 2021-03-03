#pragma once
#include "LoadingException.h"
#include "Zone/ZoneTypes.h"

class InvalidOffsetBlockException final : public LoadingException
{
    block_t m_referenced_block;

public:
    explicit InvalidOffsetBlockException(block_t referencedBlock);

    std::string DetailedMessage() override;
    char const* what() const noexcept override;
};
