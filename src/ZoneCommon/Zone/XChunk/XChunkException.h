#pragma once
#include <exception>
#include <string>

#include "Utils/ClassUtils.h"

class XChunkException final : public std::exception
{
    std::string m_message;

public:
    explicit XChunkException(std::string message);

    _NODISCARD char const* what() const override;
    _NODISCARD const std::string& Message() const;
};
