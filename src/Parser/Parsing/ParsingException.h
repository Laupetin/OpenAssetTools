#pragma once

#include "TokenPos.h"
#include "Utils/ClassUtils.h"

#include <exception>
#include <string>

class ParsingException final : public std::exception
{
    TokenPos m_pos;
    std::string m_message;
    std::string m_full_message;

public:
    ParsingException(TokenPos position, std::string message);

    _NODISCARD TokenPos Position() const;
    _NODISCARD const std::string& Message() const;
    _NODISCARD std::string FullMessage() const;
    _NODISCARD char const* what() const noexcept override;
};
