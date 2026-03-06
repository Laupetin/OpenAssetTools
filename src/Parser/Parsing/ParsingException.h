#pragma once

#include "TokenPos.h"

#include <exception>
#include <string>

class ParsingException final : public std::exception
{
    TokenPos m_pos;
    std::string m_message;
    std::string m_full_message;

public:
    ParsingException(TokenPos position, std::string message);

    [[nodiscard]] TokenPos Position() const;
    [[nodiscard]] const std::string& Message() const;
    [[nodiscard]] std::string FullMessage() const;
    [[nodiscard]] char const* what() const noexcept override;
};
