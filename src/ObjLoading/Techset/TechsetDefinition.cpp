#include "TechsetDefinition.h"

#include <cassert>

using namespace techset;

TechsetDefinition::TechsetDefinition(const size_t techniqueTypeCount)
    : m_has_technique(techniqueTypeCount),
      m_technique_names(techniqueTypeCount)
{
}

bool TechsetDefinition::GetTechniqueByIndex(const size_t index, std::string& techniqueName) const
{
    assert(index < m_has_technique.size());
    if (index >= m_has_technique.size() || !m_has_technique[index])
        return false;

    techniqueName = m_technique_names[index];
    return true;
}

void TechsetDefinition::SetTechniqueByIndex(const size_t index, std::string techniqueName)
{
    assert(index < m_has_technique.size());
    if (index >= m_has_technique.size())
        return;

    m_has_technique[index] = true;
    m_technique_names[index] = std::move(techniqueName);
}
