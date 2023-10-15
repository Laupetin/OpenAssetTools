#pragma once

namespace utils
{
    template <typename T>
    constexpr T Align(const T value, const T toNext)
    {
        if (toNext > 0)
            return (value + toNext - 1) / toNext * toNext;
        return value;
    }

    template <typename T>
    constexpr T AlignToPrevious(const T value, const T toPrevious)
    {
        if (toPrevious > 0)
            return value / toPrevious * toPrevious;
        return value;
    }
}
