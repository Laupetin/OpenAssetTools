#pragma once

namespace menu
{
    class MenuFileLexing
    {
    public:
        enum class MultiChar
        {
            SHIFT_LEFT,
            SHIFT_RIGHT,
            GREATER_EQUAL,
            LESS_EQUAL,
            EQUALS,
            NOT_EQUAL,
            LOGICAL_AND,
            LOGICAL_OR
        };

        MenuFileLexing() = delete;
    };
}