#pragma once
#include <string>
#include <exception>

#include "Utils/ClassUtils.h"

class WritingException final : public std::exception
{
    std::string m_error_message;

public:
    explicit WritingException(std::string message);

    _NODISCARD char const* what() const override;
    _NODISCARD const std::string& Message() const;
};
