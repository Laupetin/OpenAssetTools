#pragma once

#include <string>

class EnumMember
{
public:
    EnumMember();
    EnumMember(std::string name, int value);

    std::string m_name;
    int m_value;
};
