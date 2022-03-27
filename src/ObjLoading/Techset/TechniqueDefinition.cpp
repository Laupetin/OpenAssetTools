#include "TechniqueDefinition.h"

using namespace techset;

ShaderArgumentLiteralConstant::ShaderArgumentLiteralConstant()
    : m_value{}
{
}

ShaderArgumentLiteralConstant::ShaderArgumentLiteralConstant(const float v0, const float v1, const float v2, const float v3)
    : m_value{v0, v1, v2, v3}
{
}

ShaderArgumentLiteralConstant::ShaderArgumentLiteralConstant(float value[4])
    : m_value{value[0], value[1], value[2], value[3]}
{
}

bool techset::operator<(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs)
{
    if (lhs.m_value[0] < rhs.m_value[0])
        return true;
    if (lhs.m_value[0] > rhs.m_value[0])
        return false;
    if (lhs.m_value[1] < rhs.m_value[1])
        return true;
    if (lhs.m_value[1] > rhs.m_value[1])
        return false;
    if (lhs.m_value[2] < rhs.m_value[2])
        return true;
    if (lhs.m_value[2] > rhs.m_value[2])
        return false;
    if (lhs.m_value[3] < rhs.m_value[3])
        return true;
    if (lhs.m_value[3] > rhs.m_value[3])
        return false;
    return false;
}

bool techset::operator<=(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs)
{
    return !(rhs < lhs);
}

bool techset::operator>(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs)
{
    return rhs < lhs;
}

bool techset::operator>=(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs)
{
    return !(lhs < rhs);
}
