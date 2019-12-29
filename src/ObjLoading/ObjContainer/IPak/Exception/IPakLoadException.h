#pragma once
#include <exception>
#include <string>

class IPakLoadException final : public std::exception
{
    std::string m_message;

public:
    explicit IPakLoadException(std::string message);

    const std::string& DetailedMessage() const;
    char const* what() const override;
};
