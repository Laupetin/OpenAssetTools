#include "CommonTechset.h"

#include <algorithm>

namespace techset
{
    CommonTechniqueTypeNames::CommonTechniqueTypeNames(const char** names, const size_t nameCount)
        : m_names(nameCount)
    {
        std::copy(names, &names[nameCount], m_names.data());

        m_technique_type_lookup.reserve(nameCount);
        for (size_t i = 0; i < nameCount; i++)
            m_technique_type_lookup.emplace(names[i], i);
    }

    const char* CommonTechniqueTypeNames::GetTechniqueTypeName(const size_t techniqueTypeIndex) const
    {
        if (techniqueTypeIndex >= m_names.size())
            return nullptr;

        return m_names[techniqueTypeIndex];
    }

    [[nodiscard]] std::optional<size_t> CommonTechniqueTypeNames::GetTechniqueTypeByName(const std::string& name) const
    {
        const auto foundValue = m_technique_type_lookup.find(name);
        if (foundValue != m_technique_type_lookup.end())
            return foundValue->second;

        return std::nullopt;
    }

    size_t CommonTechniqueTypeNames::GetTechniqueTypeCount() const
    {
        return m_names.size();
    }

    CommonTechset::CommonTechset(const size_t techniqueTypeCount)
        : m_technique_names(techniqueTypeCount)
    {
    }

    CommonTechset::CommonTechset(std::string name, std::vector<std::string> techniqueNames)
        : m_name(std::move(name)),
          m_technique_names(std::move(techniqueNames))
    {
    }
} // namespace techset
