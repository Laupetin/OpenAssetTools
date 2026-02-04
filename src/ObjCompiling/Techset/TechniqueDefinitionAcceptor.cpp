#include "TechniqueDefinitionAcceptor.h"

using namespace techset;

ShaderArgument::ShaderArgument()
    : m_argument_index_specified(false),
      m_argument_index(0u)
{
}

ShaderArgument::ShaderArgument(std::string argumentName)
    : m_argument_name(std::move(argumentName)),
      m_argument_index_specified(false),
      m_argument_index(0u)
{
}

ShaderArgument::ShaderArgument(std::string argumentName, const unsigned argumentIndex)
    : m_argument_name(std::move(argumentName)),
      m_argument_index_specified(true),
      m_argument_index(argumentIndex)
{
}

ShaderArgumentCodeSource::ShaderArgumentCodeSource()
    : m_index_accessor_specified(false),
      m_index_accessor(0u)
{
}

ShaderArgumentCodeSource::ShaderArgumentCodeSource(std::vector<std::string> accessors)
    : m_accessors(std::move(accessors)),
      m_index_accessor_specified(false),
      m_index_accessor(0u)
{
}

ShaderArgumentCodeSource::ShaderArgumentCodeSource(std::vector<std::string> accessors, const unsigned indexAccessor)
    : m_accessors(std::move(accessors)),
      m_index_accessor_specified(true),
      m_index_accessor(indexAccessor)
{
}

ShaderArgumentLiteralSource::ShaderArgumentLiteralSource()
    : m_value{}
{
}

ShaderArgumentLiteralSource::ShaderArgumentLiteralSource(const float v0, const float v1, const float v2, const float v3)
    : m_value{v0, v1, v2, v3}
{
}

ShaderArgumentLiteralSource::ShaderArgumentLiteralSource(float value[4])
    : m_value{value[0], value[1], value[2], value[3]}
{
}

namespace techset
{
    bool operator<(const ShaderArgumentLiteralSource& lhs, const ShaderArgumentLiteralSource& rhs)
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

    bool operator<=(const ShaderArgumentLiteralSource& lhs, const ShaderArgumentLiteralSource& rhs)
    {
        return !(rhs < lhs);
    }

    bool operator>(const ShaderArgumentLiteralSource& lhs, const ShaderArgumentLiteralSource& rhs)
    {
        return rhs < lhs;
    }

    bool operator>=(const ShaderArgumentLiteralSource& lhs, const ShaderArgumentLiteralSource& rhs)
    {
        return !(lhs < rhs);
    }
} // namespace techset

ShaderArgumentMaterialSource::ShaderArgumentMaterialSource()
    : m_is_hash(false),
      m_hash(0u)
{
}

ShaderArgumentMaterialSource::ShaderArgumentMaterialSource(const unsigned hash)
    : m_is_hash(true),
      m_hash(hash)
{
}

ShaderArgumentMaterialSource::ShaderArgumentMaterialSource(std::string name)
    : m_is_hash(false),
      m_hash(0u),
      m_name(std::move(name))
{
}
