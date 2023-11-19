#pragma once

#include <string>

enum class FastFileBlockType
{
    TEMP,
    RUNTIME,
    DELAY,
    NORMAL
};

class FastFileBlock
{
public:
    std::string m_name;
    unsigned m_index;
    FastFileBlockType m_type;
    bool m_is_default;

    FastFileBlock(std::string name, unsigned index, FastFileBlockType type, bool isDefault);
};
