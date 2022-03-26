#pragma once

#include <string>
#include <vector>

namespace techset
{
    class TechsetDefinition
    {
        std::vector<bool> m_has_technique;
        std::vector<std::string> m_technique_names;

    public:
        explicit TechsetDefinition(size_t techniqueTypeCount);
        bool GetTechniqueByIndex(size_t index, std::string& techniqueName) const;
        void SetTechniqueByIndex(size_t index, std::string techniqueName);
    };
}
