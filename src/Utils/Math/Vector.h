#pragma once
#include "Utils/ClassUtils.h"

#include <cassert>

template<typename T> class Vector2
{
    T m_value[2];

public:
    Vector2()
        : m_value{T(0), T(0)}
    {
    }

    Vector2(T x, T y)
        : m_value{x, y}
    {
    }

    explicit Vector2(const T* value)
        : m_value{value[0], value[1]}
    {
    }

    _NODISCARD T& operator()(const size_t index)
    {
        assert(index < 2);
        return m_value[index];
    }

    _NODISCARD const T& operator()(const size_t index) const
    {
        assert(index < 2);
        return m_value[index];
    }

    _NODISCARD T& x()
    {
        return m_value[0];
    }

    _NODISCARD T& y()
    {
        return m_value[1];
    }

    _NODISCARD const T& x() const
    {
        return m_value[0];
    }

    _NODISCARD const T& y() const
    {
        return m_value[1];
    }
};

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

template<typename T> class Vector3
{
    T m_value[3];

public:
    Vector3()
        : m_value{T(0), T(0), T(0)}
    {
    }

    Vector3(T x, T y, T z)
        : m_value{x, y, z}
    {
    }

    explicit Vector3(const T* value)
        : m_value{value[0], value[1], value[2]}
    {
    }

    _NODISCARD T& operator()(const size_t index)
    {
        assert(index < 3);
        return m_value[index];
    }

    _NODISCARD const T& operator()(const size_t index) const
    {
        assert(index < 3);
        return m_value[index];
    }

    _NODISCARD T& x()
    {
        return m_value[0];
    }

    _NODISCARD T& y()
    {
        return m_value[1];
    }

    _NODISCARD T& z()
    {
        return m_value[2];
    }

    _NODISCARD const T& x() const
    {
        return m_value[0];
    }

    _NODISCARD const T& y() const
    {
        return m_value[1];
    }

    _NODISCARD const T& z() const
    {
        return m_value[2];
    }

    _NODISCARD T& r()
    {
        return m_value[0];
    }

    _NODISCARD T& g()
    {
        return m_value[1];
    }

    _NODISCARD T& b()
    {
        return m_value[2];
    }

    _NODISCARD const T& r() const
    {
        return m_value[0];
    }

    _NODISCARD const T& g() const
    {
        return m_value[1];
    }

    _NODISCARD const T& b() const
    {
        return m_value[2];
    }
};

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

template<typename T> class Vector4
{
    T m_value[4];

public:
    Vector4()
        : m_value{T(0), T(0), T(0), T(0)}
    {
    }

    Vector4(T x, T y, T z, T w)
        : m_value{x, y, z, w}
    {
    }

    explicit Vector4(const T* value)
        : m_value{value[0], value[1], value[2], value[3]}
    {
    }

    _NODISCARD T& operator()(const size_t index)
    {
        assert(index < 4);
        return m_value[index];
    }

    _NODISCARD const T& operator()(const size_t index) const
    {
        assert(index < 4);
        return m_value[index];
    }

    _NODISCARD T& x()
    {
        return m_value[0];
    }

    _NODISCARD T& y()
    {
        return m_value[1];
    }

    _NODISCARD T& z()
    {
        return m_value[2];
    }

    _NODISCARD T& w()
    {
        return m_value[3];
    }

    _NODISCARD const T& x() const
    {
        return m_value[0];
    }

    _NODISCARD const T& y() const
    {
        return m_value[1];
    }

    _NODISCARD const T& z() const
    {
        return m_value[2];
    }

    _NODISCARD const T& w() const
    {
        return m_value[3];
    }

    _NODISCARD T& r()
    {
        return m_value[0];
    }

    _NODISCARD T& g()
    {
        return m_value[1];
    }

    _NODISCARD T& b()
    {
        return m_value[2];
    }

    _NODISCARD T& a()
    {
        return m_value[3];
    }

    _NODISCARD const T& r() const
    {
        return m_value[0];
    }

    _NODISCARD const T& g() const
    {
        return m_value[1];
    }

    _NODISCARD const T& b() const
    {
        return m_value[2];
    }

    _NODISCARD const T& a() const
    {
        return m_value[3];
    }
};

typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;
