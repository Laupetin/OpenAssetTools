#pragma once

#include <functional>

template <typename BaseIteratorType, typename SourceType, typename TargetType>
class TransformIterator
{
    BaseIteratorType m_base;
    std::function<TargetType(SourceType)> m_transform;

public:
    TransformIterator(BaseIteratorType base, std::function<TargetType(SourceType)> transform)
        : m_base(base), m_transform(transform)
    {
    }

    TransformIterator& operator++()
    {
        ++m_base;
        return *this;
    }

    bool operator==(TransformIterator other) const { return m_base == other.m_base; }
    bool operator!=(TransformIterator other) const { return !(m_base == other.m_base); }
    TargetType operator*() { return m_transform(*m_base); }
};
