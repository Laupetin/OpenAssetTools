#pragma once
#include "LoadingException.h"

class InvalidHashException final : public LoadingException
{
public:
    std::string DetailedMessage() override;
    char const* what() const override;
};
