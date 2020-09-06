#pragma once
#include "LoadingException.h"

class TooManyAuthedGroupsException final : public LoadingException
{
public:
    std::string DetailedMessage() override;
    char const* what() const override;
};
