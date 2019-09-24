#pragma once
#include "LoadingException.h"

class InvalidChunkSizeException final : public LoadingException
{
    size_t m_size;
    size_t m_max;

public:
    explicit InvalidChunkSizeException(size_t size);
    InvalidChunkSizeException(size_t size, size_t max);

    std::string DetailedMessage() override;
    char const* what() const override;
};
