#pragma once
#include "LoadingException.h"

class UnexpectedEndOfFileException final : public LoadingException
{
public:
    UnexpectedEndOfFileException();

    std::string DetailedMessage() override;
    char const* what() const override;
};
