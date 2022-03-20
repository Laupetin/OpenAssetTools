#pragma once

namespace utils
{
    template <typename T>
    constexpr T Align(T value, T toNext)
    {
        if (toNext > 0)
            return (value + toNext - 1) / toNext * toNext;
        return value;
    }
}
