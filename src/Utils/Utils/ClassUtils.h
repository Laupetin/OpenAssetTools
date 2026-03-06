#pragma once

#include <utility>

#define STR(x) #x

template<class T> struct Movable
{
    mutable T m_val;

    // ReSharper disable once CppNonExplicitConversionOperator
    operator T() const&&
    {
        return std::move(m_val);
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    Movable(T&& in)
        : m_val(std::move(in))
    {
    }
};
