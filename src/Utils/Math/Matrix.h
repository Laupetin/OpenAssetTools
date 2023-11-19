#pragma once

template<typename T> class Matrix
{
public:
    T m_data[4][4];

    Matrix()
        : m_data{
            {T(1.0), 0,      0,      0     },
            {0,      T(1.0), 0,      0     },
            {0,      0,      T(1.0), 0     },
            {0,      0,      0,      T(1.0)}
    }
    {
    }

    Matrix(T d00, T d01, T d02, T d03, T d10, T d11, T d12, T d13, T d20, T d21, T d22, T d23, T d30, T d31, T d32, T d33)
        : m_data{
            {d00, d01, d02, d03},
            {d10, d11, d12, d13},
            {d20, d21, d22, d23},
            {d30, d31, d32, d33}
    }
    {
    }

    static Matrix<T> Identity()
    {
        return Matrix();
    }
};

typedef Matrix<float> Matrix32;
typedef Matrix<double> Matrix64;
