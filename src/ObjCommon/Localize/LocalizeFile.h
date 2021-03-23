#pragma once
#include <string>

class LocalizeFileEntry
{
public:
    std::string m_key;
    std::string m_value;

    LocalizeFileEntry();
    LocalizeFileEntry(std::string key, std::string value);
};
