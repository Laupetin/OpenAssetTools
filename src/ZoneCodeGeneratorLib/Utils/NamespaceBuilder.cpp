#include "NamespaceBuilder.h"

#include <format>
#include <sstream>

std::string NamespaceBuilder::Combine(const std::string& _namespace, const std::string& name)
{
    if (_namespace.empty())
        return name;

    return std::format("{}::{}", _namespace, name);
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

    for (const auto& element : m_elements)
    {
        if (first)
            first = false;
        else
            str << "::";

        str << element;
    }

    return str.str();
}
