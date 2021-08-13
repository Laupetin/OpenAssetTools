#pragma once

template<typename T>
class Vector3
{
public:
    union
    {
        struct
        {
            T m_x;
            T m_y;
            T m_z;
        };
        T m_data[3];
    } u;
};

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

template<typename T>
class Vector4
{
public:
    union
    {
        struct
        {
            T m_x;
            T m_y;
            T m_z;
            T m_w;
        };
        T m_data[4];
    } u;
};

typedef Vector3<float> Vector4f;
typedef Vector3<double> Vector4d;