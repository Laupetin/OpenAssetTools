#include "DataDefinition.h"


#include <cassert>
#include <sstream>

DataDefinition::DataDefinition(std::string _namespace, std::string name)
    : m_namespace(std::move(_namespace)),
      m_name(std::move(name))
{
    assert(!m_name.empty());
}

std::string DataDefinition::GetFullName() const
{
    std::ostringstream str;
    str << m_namespace << "::" << m_name;

    return str.str();
}
