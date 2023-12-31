#pragma once
#include <string>

class CommonLocalizeEntry
{
public:
    std::string m_key;
    std::string m_value;

    CommonLocalizeEntry();
    CommonLocalizeEntry(std::string key, std::string value);
};
