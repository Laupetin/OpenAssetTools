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

    static T dot(const Quaternion& q1, const Quaternion& q2)
    {
        return static_cast<T>((q1.m_x * q2.m_x) + (q1.m_y * q2.m_y) + (q1.m_z * q2.m_z) + (q1.m_w * q2.m_w));
    }

    static Quaternion& conj(Quaternion& result)
    {
        result.m_x = -result.m_x;
        result.m_y = -result.m_y;
        result.m_z = -result.m_z;
        return result;
    }

    static Quaternion& invert(Quaternion& result)
    {
        // from game programming gems p198
        // do result = conj( q ) / norm( q )
        Quaternion::conj(result);

        // return if norm() is near 0 (divide by 0 would result in NaN)
        T l = result.lengthSquared();
        if (l < static_cast<T>(0.0001))
        {
            return result;
        }

        T l_inv = static_cast<T>(1.0) / l;
        result.m_x *= l_inv;
        result.m_y *= l_inv;
        result.m_z *= l_inv;
        result.m_w *= l_inv;
        return result;
    }

    T lengthSquared() const
    {
        return Quaternion::dot(*this, *this);
    }

    T length() const
    {
        return sqrt(lengthSquared());
    }

    void Normalize()
    {
        const auto l = length();

        // return if no magnitude (already as normalized as possible)
        if (l < static_cast<T>(0.0001))
            return;

        T inverseLength = static_cast<T>(1.0) / l;
        m_x *= inverseLength;
        m_y *= inverseLength;
        m_z *= inverseLength;
        m_w *= inverseLength;
    }

    friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
    {
        return Quaternion(lhs.m_x + rhs.m_x, lhs.m_y + rhs.m_y, lhs.m_z + rhs.m_z, lhs.m_w + rhs.m_w);
    }

    friend Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
    {
        return Quaternion(lhs.m_x - rhs.m_x, lhs.m_y - rhs.m_y, lhs.m_z - rhs.m_z, lhs.m_w - rhs.m_w);
    }

    friend Quaternion& operator+=(Quaternion& lhs, const Quaternion& rhs)
    {
        lhs.m_x += rhs.m_x;
        lhs.m_y += rhs.m_y;
        lhs.m_z += rhs.m_z;
        lhs.m_w += rhs.m_w;

        return lhs;
    }

    friend Quaternion& operator-=(Quaternion& lhs, const Quaternion& rhs)
    {
        lhs.m_x -= rhs.m_x;
        lhs.m_y -= rhs.m_y;
        lhs.m_z -= rhs.m_z;
        lhs.m_w -= rhs.m_w;

        return lhs;
    }

    friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
    {
        const T x2 = lhs.m_w * rhs.m_x + lhs.m_x * rhs.m_w + lhs.m_y * rhs.m_z - lhs.m_z * rhs.m_y;
        const T y2 = lhs.m_w * rhs.m_y + lhs.m_y * rhs.m_w + lhs.m_z * rhs.m_x - lhs.m_x * rhs.m_z;
        const T z2 = lhs.m_w * rhs.m_z + lhs.m_z * rhs.m_w + lhs.m_x * rhs.m_y - lhs.m_y * rhs.m_x;
        const T w2 = lhs.m_w * rhs.m_w - lhs.m_x * rhs.m_x - lhs.m_y * rhs.m_y - lhs.m_z * rhs.m_z;

        return Quaternion(x2, y2, z2, w2);
    }

    friend Quaternion operator/(const Quaternion& lhs, const Quaternion& rhs)
    {
        Quaternion rhsInv = rhs;
        Quaternion::invert(rhsInv);
        return lhs * rhsInv;
    }

    friend Quaternion& operator*=(Quaternion& lhs, const Quaternion& rhs)
    {
        const T x2 = lhs.m_w * rhs.m_x + lhs.m_x * rhs.m_w + lhs.m_y * rhs.m_z - lhs.m_z * rhs.m_y;
        const T y2 = lhs.m_w * rhs.m_y + lhs.m_y * rhs.m_w + lhs.m_z * rhs.m_x - lhs.m_x * rhs.m_z;
        const T z2 = lhs.m_w * rhs.m_z + lhs.m_z * rhs.m_w + lhs.m_x * rhs.m_y - lhs.m_y * rhs.m_x;
        const T w2 = lhs.m_w * rhs.m_w - lhs.m_x * rhs.m_x - lhs.m_y * rhs.m_y - lhs.m_z * rhs.m_z;

        lhs.m_x = x2;
        lhs.m_y = y2;
        lhs.m_z = z2;
        lhs.m_w = w2;

        return lhs;
    }

    friend Quaternion& operator/=(Quaternion& lhs, const Quaternion& rhs)
    {
        Quaternion rhsInv = rhs;
        Quaternion::invert(rhsInv);
        lhs *= rhsInv;
        return lhs;
    }
};

typedef Quaternion<float> Quaternion32;
typedef Quaternion<double> Quaternion64;
