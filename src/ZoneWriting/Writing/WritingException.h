#pragma once

#include <exception>
#include <string>

class WritingException final : public std::exception
{
    std::string m_error_message;

public:
    explicit WritingException(std::string message);

    [[nodiscard]] char const* what() const noexcept override;
    [[nodiscard]] const std::string& Message() const;
};
