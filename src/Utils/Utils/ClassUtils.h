#pragma once

#include <utility>

#define STR(x) #x

#if defined(_MSVC_LANG)
#define _CPP_VERSION _MSVC_LANG
#elif defined(__cplusplus)
#define _CPP_VERSION __cplusplus
#else
#define _CPP_VERSION 0
#endif

#ifdef _NODISCARD
#undef _NODISCARD
#endif

#if _CPP_VERSION >= 201703L
#define _NODISCARD [[nodiscard]]
#else
#define _NODISCARD
#endif

template <class T>
struct Movable
{
    mutable T m_val;

    // ReSharper disable once CppNonExplicitConversionOperator
    operator T() const &&
    {
        return std::move(m_val);
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    Movable(T&& in)
        : m_val(std::move(in))
    {
    }
};
