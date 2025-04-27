#pragma once

#include <functional>
#include <string>

class TokenPos
{
public:
    TokenPos();
    TokenPos(const std::string& filename, size_t line, size_t column);

    std::reference_wrapper<const std::string> m_filename;
    size_t m_line;
    size_t m_column;
};
