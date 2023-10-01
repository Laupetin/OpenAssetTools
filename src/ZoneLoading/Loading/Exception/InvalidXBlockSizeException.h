#pragma once
#include <cstdint>
#include "LoadingException.h"

class InvalidXBlockSizeException final : public LoadingException
{
    uint64_t m_size;
    uint64_t m_max;

public:
    InvalidXBlockSizeException(uint64_t size, uint64_t max);

    std::string DetailedMessage() override;
    char const* what() const noexcept override;
};
