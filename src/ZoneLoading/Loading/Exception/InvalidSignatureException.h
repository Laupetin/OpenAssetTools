#pragma once
#include "LoadingException.h"

class InvalidSignatureException final : public LoadingException
{
public:
    std::string DetailedMessage() override;
    char const* what() const noexcept override;
};
