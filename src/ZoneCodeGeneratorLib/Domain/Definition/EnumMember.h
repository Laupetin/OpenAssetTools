#pragma once

#include <string>

class EnumMember
{
public:
    std::string m_name;
    int m_value;

    EnumMember();
    EnumMember(std::string name, int value);
};