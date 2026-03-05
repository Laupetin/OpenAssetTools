#pragma once

#include <exception>
#include <string>

class XChunkException final : public std::exception
{
    std::string m_message;

public:
    explicit XChunkException(std::string message);

    [[nodiscard]] char const* what() const noexcept override;
    [[nodiscard]] const std::string& Message() const;
};
