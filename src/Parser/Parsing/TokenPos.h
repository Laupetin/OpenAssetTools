#pragma once

#include <functional>
#include <string>

class TokenPos
{
    static const std::string EMPTY_FILENAME;

public:
    std::reference_wrapper<const std::string> m_filename;
    int m_line;
    int m_column;

    TokenPos();
    TokenPos(const std::string& filename, int line, int column);
};
