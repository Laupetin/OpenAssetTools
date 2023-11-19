#pragma once
#include "Utils/ClassUtils.h"

#include <exception>
#include <string>

class WritingException final : public std::exception
{
    std::string m_error_message;

public:
    explicit WritingException(std::string message);

    _NODISCARD char const* what() const noexcept override;
    _NODISCARD const std::string& Message() const;
};
