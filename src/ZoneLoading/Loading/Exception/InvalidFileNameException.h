#pragma once
#include "LoadingException.h"

class InvalidFileNameException final : public LoadingException
{
public:
    InvalidFileNameException(const std::string& actualFileName, const std::string& expectedFileName);

    std::string DetailedMessage() override;
    char const* what() const noexcept override;

    std::string m_actual_file_name;
    std::string m_expected_file_name;
};
