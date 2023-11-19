#pragma once

#include "Matrix.h"
#include "Utils/ClassUtils.h"

template<typename T> class Quaternion
{
public:
    T m_x;
    T m_y;
    T m_z;
    T m_w;

    Quaternion()
    {
        m_x = T(0);
        m_y = T(0);
        m_z = T(0);
        m_w = T(1);
    }

    Quaternion(T x, T y, T z, T w)
    {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;
    }

    _NODISCARD Matrix<T> ToMatrix() const
    {
        const T xx = m_x * m_x;
        const T xy = m_x * m_y;
        const T xz = m_x * m_z;
        const T xw = m_x * m_w;
        const T yy = m_y * m_y;
        const T yz = m_y * m_z;
        const T yw = m_y * m_w;
        const T zz = m_z * m_z;
        const T zw = m_z * m_w;

        const T m00 = 1 - 2 * yy - 2 * zz;
        const T m01 = 2 * xy - 2 * zw;
        const T m02 = 2 * xz + 2 * yw;
        const T m10 = 2 * xy + 2 * zw;
        const T m11 = 1 - 2 * xx - 2 * zz;
        const T m12 = 2 * yz - 2 * xw;
        const T m20 = 2 * xz - 2 * yw;
        const T m21 = 2 * yz + 2 * xw;
        const T m22 = 1 - 2 * xx - 2 * yy;

        return Matrix<T>(m00, m01, m02, 0, m10, m11, m12, 0, m20, m21, m22, 0, 0, 0, 0, T(1.0));
    }
};

typedef Quaternion<float> Quaternion32;
typedef Quaternion<double> Quaternion64;
