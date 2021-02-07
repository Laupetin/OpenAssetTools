#pragma once

#include <string>

class EnumMember
{
public:
    std::string m_name;
    long long m_value;

    EnumMember();
    EnumMember(std::string name, long long value);
};