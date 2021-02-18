#include "NamespaceBuilder.h"

#include <sstream>

std::string NamespaceBuilder::Combine(const std::string& _namespace, const std::string& name)
{
    if (_namespace.empty())
        return name;

    std::ostringstream str;
    str << _namespace << "::" << name;

    return str.str();
}

bool NamespaceBuilder::IsEmpty() const
{
    return m_elements.empty();
}

void NamespaceBuilder::Push(std::string element)
{
    m_elements.emplace_back(std::move(element));
}

void NamespaceBuilder::Pop()
{
    m_elements.pop_back();
}

std::string NamespaceBuilder::ToString()
{
    std::ostringstream str;
    auto first = true;

    for(const auto& element : m_elements)
    {
        if (first)
            first = false;
        else
            str << "::";

        str << element;
    }

    return str.str();
}
