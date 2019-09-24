#pragma once
#include "LoadingException.h"

class InvalidMagicException final : public LoadingException
{
    const char* m_expected_magic;

public:
    explicit InvalidMagicException(const char* expectedMagic);

    std::string DetailedMessage() override;
    char const* what() const override;
};
