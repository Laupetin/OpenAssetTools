#pragma once

#include "LoadingException.h"
#include "Zone/ZoneTypes.h"

#include <cstdlib>

class InvalidLookupPositionException final : public LoadingException
{
public:
    InvalidLookupPositionException(block_t block, size_t offset);

    std::string DetailedMessage() override;
    char const* what() const noexcept override;

private:
    block_t m_block;
    size_t m_offset;
};
