#pragma once
#include "LoadingException.h"

class InvalidVersionException final : public LoadingException
{
    unsigned int m_expected_version;
    unsigned int m_actual_version;

public:
    InvalidVersionException(unsigned int expectedVersion, unsigned int actualVersion);

    std::string DetailedMessage() override;
    char const* what() const noexcept override;
};
