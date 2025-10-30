#include "CommonTechset.h"

#include <algorithm>

techset::CommonTechniqueTypeNames::CommonTechniqueTypeNames(const char** names, const size_t nameCount)
    : m_names(nameCount)
{
    std::copy(names, &names[nameCount], m_names.data());
}

const char* techset::CommonTechniqueTypeNames::GetTechniqueTypeName(const size_t techniqueTypeIndex) const
{
    if (techniqueTypeIndex >= m_names.size())
        return nullptr;

    return m_names[techniqueTypeIndex];
}

size_t techset::CommonTechniqueTypeNames::GetTechniqueTypeCount() const
{
    return m_names.size();
}
