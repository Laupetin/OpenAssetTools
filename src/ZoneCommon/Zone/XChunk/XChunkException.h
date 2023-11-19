#pragma once
#include "Utils/ClassUtils.h"

#include <exception>
#include <string>

class XChunkException final : public std::exception
{
    std::string m_message;

public:
    explicit XChunkException(std::string message);

    _NODISCARD char const* what() const noexcept override;
    _NODISCARD const std::string& Message() const;
};
